// Copyright 2011-2019 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "Window.h"

#if __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

Window* Window::instance_ = nullptr;

Window::Window(const char* title, int width, int height)
    : width_(width),
      height_(height),
      scaling_(1)
{
    // initialize glfw window
    if (!glfwInit())
        exit(EXIT_FAILURE);

    // request core profile and OpenGL version 3.2
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window_)
    {
        std::cerr << "Cannot create GLFW window.\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window_);
    instance_ = this;

    // check for sufficient OpenGL version
    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    GLint glversion = 10 * major + minor;
#ifdef __EMSCRIPTEN__
    if (glversion < 30)
    {
        // clang-format will line-break strings, which will break EM_ASM
        // clang-format off
        std::cerr << "Cannot get WebGL2 context. Try using Firefox or Chrome/Chromium.\n";
        EM_ASM(alert("Cannot get WebGL2 context. Try using Firefox or Chrome/Chromium."));
        // clang-format on
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
#else
    if (glversion < 32)
    {
        std::cerr << "Cannot get modern OpenGL context.\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
#endif

    // enable v-sync
    glfwSwapInterval(1);

    // now that we have a GL context, initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cerr << "Error initializing GLEW: " << glewGetErrorString(err)
                  << std::endl;
        exit(1);
    }

    // debug: print GL and GLSL version
    std::cout << "GLEW   " << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "GL     " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL   " << glGetString(GL_SHADING_LANGUAGE_VERSION)
              << std::endl;

    // call glGetError once to clear error queue
    glGetError();

    // detect highDPI framebuffer scaling and UI scaling
    // this part is OS dependent:
    // MacOS: just ratio of framebuffer size and window size
    // Linux: use new GLFW content scaling
    // Emscripten: use device pixel ratio
    int window_width, window_height, framebuffer_width, framebuffer_height;
    glfwGetWindowSize(window_, &window_width, &window_height);
    glfwGetFramebufferSize(window_, &framebuffer_width, &framebuffer_height);
    width_ = framebuffer_width;
    height_ = framebuffer_height;
#ifndef __EMSCRIPTEN__
    scaling_ = framebuffer_width / window_width;
    if (scaling_ != 1)
        std::cout << "highDPI scaling: " << scaling_ << std::endl;
#else
    pixel_ratio_ = emscripten_get_device_pixel_ratio();
    if (pixel_ratio_ != 1)
        std::cout << "highDPI scaling: " << pixel_ratio_ << std::endl;
#endif

    // register glfw callbacks
    glfwSetErrorCallback(glfw_error);
    glfwSetCharCallback(window_, glfw_character);
    glfwSetKeyCallback(window_, glfw_keyboard);
    glfwSetCursorPosCallback(window_, glfw_motion);
    glfwSetMouseButtonCallback(window_, glfw_mouse);
    glfwSetScrollCallback(window_, glfw_scroll);
    glfwSetFramebufferSizeCallback(window_, glfw_resize);

    // init mouse button state and modifiers
    for (bool& i : button_)
        i = false;
    ctrl_pressed_ = shift_pressed_ = alt_pressed_ = false;
}

Window::~Window()
{
    glfwTerminate(); // this automatically destroys remaining windows
}

int Window::run()
{
    // initialize OpenGL
    initialize();

#if __EMSCRIPTEN__
    emscripten_set_main_loop(Window::render_frame, 0, 1);
#else
    while (!glfwWindowShouldClose(window_))
    {
        Window::render_frame();
    }
#endif
    return EXIT_SUCCESS;
}

void Window::render_frame()
{
    glfwMakeContextCurrent(instance_->window_);

#if __EMSCRIPTEN__
    // determine correct canvas/framebuffer size
    int w, h, f;
    double dw, dh;
    emscripten_get_canvas_element_size("#canvas", &w, &h);
    emscripten_get_element_css_size("#canvas", &dw, &dh);
    double s = instance_->pixel_ratio_;
    if (w != int(dw * s) || h != int(dh * s))
    {
        // set canvas size to match element css size
        w = int(dw * s);
        h = int(dh * s);
        emscripten_set_canvas_element_size("#canvas", w, h);
        glfw_resize(instance_->window_, w, h);
    }
#endif

    // do some computations
    instance_->timer();

    // draw scene
    instance_->render();

#if __EMSCRIPTEN__
    // to avoid problems with premultiplied alpha in WebGL,
    // clear alpha values to 1.0
    // see here: https://webgl2fundamentals.org/webgl/lessons/webgl-and-alpha.html
    // (no way to disable premultiplied-alpha in emscripten-glfw)
    GLfloat rgba[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, rgba);
    glClearColor(1, 1, 1, 1);
    glColorMask(false, false, false, true);
    glClear(GL_COLOR_BUFFER_BIT);
    glColorMask(true, true, true, true);
    glClearColor(rgba[0], rgba[1], rgba[2], rgba[3]);
#endif

    // swap buffers
    glfwSwapBuffers(instance_->window_);

    // handle events
    glfwPollEvents();
}

void Window::glfw_error(int error, const char* description)
{
    std::cerr << "error (" << error << "):" << description << std::endl;
}

void Window::glfw_character(GLFWwindow* window, unsigned int c)
{
    instance_->character(c);
}

void Window::glfw_keyboard(GLFWwindow* window, int key, int scancode,
                           int action, int mods)
{
    // remember modifier status
    switch (key)
    {
        case GLFW_KEY_LEFT_CONTROL:
        case GLFW_KEY_RIGHT_CONTROL:
            instance_->ctrl_pressed_ = (action != GLFW_RELEASE);
            break;
        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_RIGHT_SHIFT:
            instance_->shift_pressed_ = (action != GLFW_RELEASE);
            break;
        case GLFW_KEY_LEFT_ALT:
        case GLFW_KEY_RIGHT_ALT:
            instance_->alt_pressed_ = (action != GLFW_RELEASE);
            break;
    }

    // send event to window
    instance_->keyboard(key, scancode, action, mods);
}

void Window::character(unsigned int c)
{
    switch (c)
    {
        case 63: // question mark
            break;
    }
}

void Window::keyboard(int key, int /*code*/, int action, int /*mods*/)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;

    switch (key)
    {
#ifndef __EMSCRIPTEN__
        case GLFW_KEY_ESCAPE:
        case GLFW_KEY_Q:
        {
            exit(0);
            break;
        }
#endif
        case GLFW_KEY_F:
        {
            if (!is_fullscreen())
                enter_fullscreen();
            else
                exit_fullscreen();
            break;
        }
    }
}

// fullscreen handling from here:
// https://github.com/emscripten-core/emscripten/issues/5124

#ifdef __EMSCRIPTEN__

bool Window::is_fullscreen() const
{
    EmscriptenFullscreenChangeEvent fsce;
    emscripten_get_fullscreen_status(&fsce);
    return fsce.isFullscreen;
}

void Window::enter_fullscreen()
{
    // get screen size
    int w = EM_ASM_INT({ return screen.width; });
    int h = EM_ASM_INT({ return screen.height; });

    // Workaround https://github.com/kripken/emscripten/issues/5124#issuecomment-292849872
    EM_ASM(JSEvents.inEventHandler = true);
    EM_ASM(JSEvents.currentEventHandler = {allowsDeferredCalls : true});

    // remember window size
    glfwGetWindowSize(window_, &backup_width_, &backup_height_);

    // setting window to screen size triggers fullscreen mode
    glfwSetWindowSize(window_, w, h);
}

void Window::exit_fullscreen()
{
    emscripten_exit_fullscreen();
    glfwSetWindowSize(window_, backup_width_, backup_height_);
}

#else

bool Window::is_fullscreen() const
{
    return glfwGetWindowMonitor(window_) != nullptr;
}

void Window::enter_fullscreen()
{
    // get monitor
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();

    // get resolution
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    // remember window position and size
    glfwGetWindowPos(window_, &backup_xpos_, &backup_ypos_);
    glfwGetWindowSize(window_, &backup_width_, &backup_height_);

    // switch to fullscreen on primary monitor
    glfwSetWindowMonitor(window_, monitor, 0, 0, mode->width, mode->height,
                         GLFW_DONT_CARE);
}

void Window::exit_fullscreen()
{
    glfwSetWindowMonitor(window_, nullptr, backup_xpos_, backup_ypos_,
                         backup_width_, backup_height_, GLFW_DONT_CARE);
}

#endif

void Window::glfw_motion(GLFWwindow* /*window*/, double xpos, double ypos)
{
    // correct for highDPI scaling
    instance_->motion(instance_->scaling_ * xpos, instance_->scaling_ * ypos);
}

void Window::glfw_mouse(GLFWwindow* window, int button, int action, int mods)
{
    instance_->button_[button] = (action == GLFW_PRESS);
    instance_->mouse(button, action, mods);
}

void Window::glfw_scroll(GLFWwindow* window, double xoffset, double yoffset)
{
#ifdef __EMSCRIPTEN__
    yoffset = -yoffset;

    // thresholding for cross-browser handling
    const float t = 1;
    if (yoffset > t)
        yoffset = t;
    else if (yoffset < -t)
        yoffset = -t;
#endif

    instance_->scroll(xoffset, yoffset);
}

void Window::glfw_resize(GLFWwindow* /*window*/, int width, int height)
{
    instance_->width_ = width;
    instance_->height_ = height;
    instance_->resize(width, height);
}

void Window::cursor_pos(double& x, double& y) const
{
    glfwGetCursorPos(window_, &x, &y);
    x *= instance_->scaling_;
    y *= instance_->scaling_;
}

bool Window::map_to_sphere(const int point_x, const int point_y, vec3& result)
{
    if ((point_x >= 0) && (point_x <= width_) && (point_y >= 0) &&
        (point_y <= height_))
    {
        double w = width_;
        double h = height_;
        double x = (double)(point_x - 0.5*w) / w;
        double y = (double)(0.5*h - point_y) / h;
        double sinx = sin(M_PI * x * 0.5);
        double siny = sin(M_PI * y * 0.5);
        double sinx2siny2 = sinx * sinx + siny * siny;

        result[0] = sinx;
        result[1] = siny;
        result[2] = sinx2siny2 < 1.0 ? sqrt(1.0 - sinx2siny2) : 0.0;

        return true;
    }
    else
        return false;
}
