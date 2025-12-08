//=============================================================================
//
//   Exercise code for the lecture "Computer Graphics"
//     by Prof. Mario Botsch, TU Dortmund
//
//   Copyright (C) by Computer Graphics Group, TU Dortmund
//
//=============================================================================

#include "solar_viewer.h"
#include "glmath.h"
#include <stdlib.h> /* srand, rand */
#include <time.h>   /* time */

using namespace std;

//=============================================================================

// start version scales
#define D_RADIUS_SCALE_SMALL 15.0f
#define D_RADIUS_SCALE_BIG 6.0f
#define D_DISTANCE_SCALE_SMALL 0.03f
#define D_DISTANCE_SCALE_BIG 0.015f
#define D_DISTANCE_SCALE_MOON 1.5f

// better version <-- use this as soon as you finished the first 2 tasks
//#define D_RADIUS_SCALE_SMALL 12.0f
//#define D_RADIUS_SCALE_BIG 7.0f
//#define D_DISTANCE_SCALE_SMALL 0.07f
//#define D_DISTANCE_SCALE_BIG 0.07f
//#define D_DISTANCE_SCALE_MOON 1.5f

// realistic version <-- use this just if you are interested in realistic scales
// or if you want to get a 'lost in space' feeling (use the spaceship and try to reach some of the other planets)
//#define D_RADIUS_SCALE_SMALL 1.0f
//#define D_RADIUS_SCALE_BIG 1.0f
//#define D_DISTANCE_SCALE_SMALL 1.0f
//#define D_DISTANCE_SCALE_BIG 1.0f
//#define D_DISTANCE_SCALE_MOON 1.0f

Solar_viewer::Solar_viewer(const char* _title, int _width, int _height)
    : Window(_title, _width, _height),
      sun_("sun", 0.0f, 2.0f * (float)(M_PI) / 26.0f, 1.0f, 0.0f, .0f),
      mercury_("mercury", 2.0f * (float)(M_PI) / 116.0f,
               2.0f * (float)(M_PI) / 58.5f, D_RADIUS_SCALE_SMALL * 0.0035f,
               D_DISTANCE_SCALE_SMALL * 50.5f, 0.0f),
      venus_("venus", 2.0f * (float)(M_PI) / 225.0f,
             2.0f * (float)(M_PI) / 243.0f, D_RADIUS_SCALE_SMALL * 0.0087f,
             D_DISTANCE_SCALE_SMALL * 78.4f, 0.0f),
      earth_("earth", 2.0f * (float)(M_PI) / 365.0f, 2.0f * (float)(M_PI),
             D_RADIUS_SCALE_SMALL * 0.0091f, D_DISTANCE_SCALE_SMALL * 109.6f,
             0.0f),
      moon_("moon", 2.0f * (float)(M_PI) / 27.0f, 0.0f,
            D_RADIUS_SCALE_SMALL * 0.0024f, D_DISTANCE_SCALE_MOON * 0.26f,
            0.0f),
      mars_("mars", 2.0f * (float)(M_PI) / 687.0f, 2.0f * (float)(M_PI) / 1.88,
            D_RADIUS_SCALE_SMALL * 0.0048f, D_DISTANCE_SCALE_SMALL * 179.4f,
            0.0f),
      jupiter_("jupiter", 2.0f * (float)(M_PI) / 4335.0f,
               2.0f * (float)(M_PI) / 0.41f, D_RADIUS_SCALE_BIG * 0.1027f,
               D_DISTANCE_SCALE_BIG * 586.6f, 0.0f),
      saturn_("saturn", 2.0f * (float)(M_PI) / 10715.0f,
              2.0f * (float)(M_PI) / 0.44f, D_RADIUS_SCALE_BIG * 0.0866f,
              D_DISTANCE_SCALE_BIG * 1079.7f, 26.73f),
      stars_("stars", 0.0f, 0.0f, D_DISTANCE_SCALE_BIG * 2500.0f, .0f, .0f),
      unit_sphere_mesh_(50)
{
    moon_.parent_planet_ = &earth_;

    // convenience vector that holds all planets and moon
    planets_.resize(7);
    planets_[0] = &mercury_;
    planets_[1] = &venus_;
    planets_[2] = &earth_;
    planets_[3] = &mars_;
    planets_[4] = &jupiter_;
    planets_[5] = &saturn_;
    planets_[6] = &moon_;

    // convenience vector that holds all celestial objects
    space_objects_.resize(9);
    for (int i = 0; i < 7; i++)
        space_objects_[i + 1] = planets_[i];
    space_objects_[0] = &sun_;
    space_objects_[8] = &stars_;

    // start animation
    timer_active_ = true;
    time_step_ = 1.0f / 24.0f; // one hour

    // rendering parameters
    greyscale_ = false;
    wireframe_ = false;
    view_mode_ = MONO;
    fovy_ = 45;
    near_ = 0.01f;
    far_ = 20;

    // viewing angle
    look_at_ = &earth_;
    dist_factor_ = 4.5f;

    ship_.position_ = vec4(look_at_->distance_, 0.15f, 0.0f, 1.0f) -
                      vec4(0.0f, 0.0f, dist_factor_ * look_at_->radius_, 0.0f);
    ship_.direction_ = vec4(0.0f, 0.0f, 1.0f, 0.0f);
    //ship_.radius_ = 0.03*earth_.radius_;
    in_ship_ = false;

    mouse_down_ = false;
    last_x_ = -1;
    last_y_ = -1;
    camera_rotation_ = mat4::identity();

    srand((unsigned int)time(NULL));
}

//-----------------------------------------------------------------------------

void Solar_viewer::initialize()
{
    // set initial state
    glClearColor(1, 1, 1, 0);
    glEnable(GL_DEPTH_TEST);

    // define blending function
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // setup sun & stars texture in channel 0
    sun_.texture_.init(GL_TEXTURE0, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR,
                       GL_LINEAR, GL_REPEAT);
    stars_.texture_.init(GL_TEXTURE0, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR,
                         GL_LINEAR, GL_REPEAT);

    // setup planet's texture in channel 0
    for (Space_Object* planet : planets_)
    {
        planet->texture_.init(GL_TEXTURE0, GL_TEXTURE_2D,
                              GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
    }

    // setup earth special textures in tex channels 1,2,3
    earth_.night_.init(GL_TEXTURE1, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR,
                       GL_LINEAR, GL_REPEAT);
    earth_.cloud_.init(GL_TEXTURE2, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR,
                       GL_LINEAR, GL_REPEAT);
    earth_.gloss_.init(GL_TEXTURE3, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR,
                       GL_LINEAR, GL_REPEAT);

    ship_.texture_.init(GL_TEXTURE0, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR,
                        GL_LINEAR, GL_REPEAT);

    // load texture files
#ifndef BASE_PATH
#if defined(__EMSCRIPTEN__)
#define BASE_PATH ("./")
#elif defined(_WIN32) || defined(__XCODE__)
#define BASE_PATH ("../../")
#else
#define BASE_PATH ("../")
#endif
#endif

    std::string texture_path =
        std::string(BASE_PATH) + std::string("textures/");

    sun_.texture_.loadPNG(texture_path + string("sun.png"));
    stars_.texture_.loadPNG(texture_path + string("stars2.png"));
    mercury_.texture_.loadPNG(texture_path + string("mercury.png"));
    venus_.texture_.loadPNG(texture_path + string("venus.png"));
    earth_.texture_.loadPNG(texture_path + string("earth_day.png"));
    mars_.texture_.loadPNG(texture_path + string("mars.png"));
    jupiter_.texture_.loadPNG(texture_path + string("jupiter.png"));
    saturn_.texture_.loadPNG(texture_path + string("saturn.png"));
    moon_.texture_.loadPNG(texture_path + string("moon.png"));
    earth_.night_.loadPNG(texture_path + string("earth_night.png"));
    earth_.cloud_.loadPNG(texture_path + string("earth_clouds.png"));
    earth_.gloss_.loadPNG(texture_path + string("earth_gloss.png"));

    //  setup ship
    ship_.texture_.loadPNG(texture_path + string("ship.png"));
    ship_.load_model(std::string(BASE_PATH) + string("meshes/spaceship.off"));
    //    ship_.initGlArrays();

    // setup shader
    std::string shader_path =
        std::string(BASE_PATH) + std::string("src/shader/");

    bool check_ok = true;
    check_ok &= color_shader_.load(shader_path + string("color.vert"),
                                   shader_path + string("color.frag"));
    check_ok &= sun_shader_.load(shader_path + string("sun.vert"),
                                 shader_path + string("sun.frag"));
    check_ok &= phong_shader_.load(shader_path + string("phong.vert"),
                                   shader_path + string("phong.frag"));
    check_ok &= earth_shader_.load(shader_path + string("earth.vert"),
                                   shader_path + string("earth.frag"));

    if (!check_ok)
    {
        std::cerr << "Shader problem!" << std::endl;
        exit(-2);
    }
}

//-----------------------------------------------------------------------------

void Solar_viewer::keyboard(int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
            case GLFW_KEY_1:
            case GLFW_KEY_2:
            case GLFW_KEY_3:
            case GLFW_KEY_4:
            case GLFW_KEY_5:
            case GLFW_KEY_6:
            case GLFW_KEY_7:
            case GLFW_KEY_8:
            {
                in_ship_ = false;
                look_at_ = space_objects_[key - 49];
                break;
            }

            case GLFW_KEY_F:
            {
                in_ship_ = !in_ship_;
                if (in_ship_)
                    time_step_ = 0.002f;
                break;
            }
            case GLFW_KEY_S:
            case GLFW_KEY_W:
            {
                if (in_ship_)
                {
                    float accel = (key == GLFW_KEY_W) ? 0.0002 : -0.0002;
                    ship_.accelerate(accel);
                }
                break;
            }

            case GLFW_KEY_D:
            case GLFW_KEY_A:
            {
                if (in_ship_)
                {
                    float accel = (key == GLFW_KEY_A) ? 0.02 : -0.02;
                    ship_.accelerate_angular(accel);
                }
                break;
            }
            case GLFW_KEY_LEFT:
            {
                camera_rotation_ = mat4::rotate_y(-10.0) * camera_rotation_;
                break;
            }

            case GLFW_KEY_RIGHT:
            {
                camera_rotation_ = mat4::rotate_y(10.0) * camera_rotation_;
                break;
            }

            case GLFW_KEY_DOWN:
            {
                camera_rotation_ = camera_rotation_ * mat4::rotate_x(-10.0);
                break;
            }

            case GLFW_KEY_UP:
            {
                camera_rotation_ = camera_rotation_ * mat4::rotate_x(10.0);
                break;
            }
            case GLFW_KEY_9:
            {
                dist_factor_ = std::min(dist_factor_ + 4.0f, 20.0f);
                break;
            }
                case GLFW_KEY_0:
            {
                dist_factor_ = std::max(dist_factor_ - 4.0f, 2.5f);
                break;
            }

            /** \todo Implement the ability to change the viewer's distance to the celestial body.
         *    - key 9 should increase and key 0 should decrease the `dist_factor_`
         *    - make sure that `2.5 < dist_factor_ < 20.0`
         *  Note: the mouse wheel also activates zooming, if you finish this task.
         */


            case GLFW_KEY_M:
            {
                wireframe_ = !wireframe_;
                break;
            }

            case GLFW_KEY_R:
            {
                randomize_planets();
                break;
            }

            case GLFW_KEY_G:
            {
                greyscale_ = !greyscale_;
                break;
            }

            case GLFW_KEY_TAB:
            {
                switch (view_mode_)
                {
                    case MONO:
                        view_mode_ = STEREO_ANAGLYPH;
                        break;
                    case STEREO_ANAGLYPH:
                        view_mode_ = STEREO_SPLIT;
                        break;
                    case STEREO_SPLIT:
                        view_mode_ = MONO;
                        break;
                }
                break;
            }

            case GLFW_KEY_SPACE:
            {
                timer_active_ = !timer_active_;
                break;
            }

            case GLFW_KEY_KP_ADD:
            case GLFW_KEY_RIGHT_BRACKET:
            case GLFW_KEY_EQUAL:
            {
                if (in_ship_)
                    break;

                time_step_ *= 2.0f;
                time_step_ = std::min(time_step_, 80.0f);
                std::cout << "Time step: " << time_step_ << " days"
                          << std::endl;
                break;
            }

            case GLFW_KEY_KP_SUBTRACT:
            case GLFW_KEY_SLASH:
            case GLFW_KEY_MINUS:
            {
                if (in_ship_)
                    break;

                time_step_ *= 0.5f;
                time_step_ = std::max(time_step_, 0.01f);
                std::cout << "Time step: " << time_step_ << " days"
                          << std::endl;
                break;
            }

#ifndef __EMSCRIPTEN__
            case GLFW_KEY_ESCAPE:
            {
                exit(0);
            }
#endif
        }
    }
}

//-----------------------------------------------------------------------------

void Solar_viewer::mouse(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
    {
        last_point_ok_ = map_to_sphere(last_x_, last_y_, last_point_3d_);
        mouse_down_ = true;
        mouse_inside_ = true;
    }
    else
        last_point_ok_ = false;

    if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE)
    {
        mouse_down_ = false;
        last_x_ = -1;
        last_y_ = -1;
    }
}

//-----------------------------------------------------------------------------

void Solar_viewer::motion(double xpos, double ypos)
{
    if (mouse_down_)
    {
        if (last_x_ < 0 || last_y_ < 0)
        {
            last_x_ = xpos;
            last_y_ = ypos;
            return;
        }

        last_x_ = xpos;
        last_y_ = ypos;
        if (last_point_ok_)
        {
            vec3 newPoint3D;
            bool newPointok = map_to_sphere(xpos, ypos, newPoint3D);

            if (newPointok)
            {
                vec3 axis = normalize(cross(last_point_3d_, newPoint3D));
                float cosAngle = dot(last_point_3d_, newPoint3D);

                if (fabs(cosAngle) < 1.0)
                {
                    float angle = -2.0 * acos(cosAngle) * 180.0 / M_PI;
                    if (!shift_pressed())
                    {
                        if (in_ship_)
                        {
                            angle *= -axis[1];
                            ship_.angle_ -= angle;
                        }
                        else
                        {
                        // constrain to y rotation
                        angle *= axis[1];
                        axis = vec3(0, 1, 0);

                        camera_rotation_ =
                            mat4::rotate_angle_axis(angle, axis) *
                            camera_rotation_;

                        }
                    }
                    else
                    {

                            // constrain to x rotation
                            angle *= axis[0];
                            axis = vec3(1, 0, 0);

                            camera_rotation_ =
                                camera_rotation_ *
                                mat4::rotate_angle_axis(angle, axis);
                    }
                }
            }
        }
    }
    last_point_ok_ = map_to_sphere(last_x_, last_y_, last_point_3d_);
}

//-----------------------------------------------------------------------------

void Solar_viewer::scroll(double xoffset, double yoffset)
{
    (yoffset > 0) ? keyboard(GLFW_KEY_9, 0, GLFW_PRESS, 0)
                  : keyboard(GLFW_KEY_0, 0, GLFW_PRESS, 0);
}

//-----------------------------------------------------------------------------

void Solar_viewer::timer()
{
    if (timer_active_)
    {
        for (Space_Object* so : space_objects_)
        {
            so->time_step(time_step_);
            so->update();
        }
        ship_.update_ship();
    }
}

//-----------------------------------------------------------------------------

void Solar_viewer::resize(int width, int height)
{
    glViewport(0, 0, width, height);
}

//-----------------------------------------------------------------------------

void Solar_viewer::render()
{
    // clear framebuffer and depth buffer first
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /** \todo Implement a kind of navigation through the solar system.
     *   - Allow rotation of the camera.
     *     `camera_rotation_` holds the current camera rotation and is updated when certain keys are pressed (see `Solar_viewer::keyboard(...)`).
     *     `dist_factor_` is the number of radii from the center of the planet to the camera.
     *   - Don't modify the view matrix directly but instead the input of `mat4::look_at(...)`
     *   - At the beginning, the camera should be translated in positive z-direction from the planet's center.
     *   - The camera should always face the planet's center. The rotation is meant to be around the planet'center not around the camera's center.
     *   - If one of the keys `1` to `7` is pressed, the camera should face the respective celestial body (compare `Solar_viewer::keyboard(...)`).
     *     `look_at_` is updated in this case and stores a pointer to the current looked at planet.
     *   - To support changing distance to the body see `Solar_viewer::keyboard(...)`
     *   - If you are in the spaceship (member in_ship_), the camera should hover slightly behind it and rotate along with it.
     * Hint: To understand what effect is desired, it helps to watch the solution videos.
     */

    mat4 projection;
    mat4 view;
    float radius;
    vec4 eye;
    vec4 center;
    vec4 up;


    // eye = vec4(7.0, 0.0, 0.0, 1.0);
    center = look_at_->position_;
    radius = look_at_->radius_;
    eye = center + camera_rotation_ * vec4(0.0f, 0.0f, 1.0f, 0.0f) * (dist_factor_ * radius);
    up = vec4(0, 1, 0, 0);

    view = mat4::look_at(vec3(eye), (vec3)center, (vec3)up);


    if (view_mode_ == MONO)
    {
        // mono rendering - setup frustum matrix
        float aspect = (float)width() / (float)height();
        float t = near_ * tan(fovy_ * (float)M_PI / 360.0f);
        float b = -t;
        float l = b * aspect;
        float r = t * aspect;

        projection = mat4::frustum(l, r, b, t, near_, far_);

        // draw every object in the scene
        draw_scene(projection, view);
    }
    else
    {
        // stereo rendering

        /** \todo (optional) Implement stereo rendering.
         *    1. Calculate parameters for the left and right eye.
         *    2. Use `mat4::frustum(...)` to construct your two projection matrices.
         *    3. Translate the `view` matrix to get one for the left and one for the right eye.
         *    3. Render two images, one for the left and one for the right eye using `draw_scene(...)`;.
         *      - For `view_mode_ == STEREO_ANAGLYPH`, look up `glColorMask` and use it to enable filtering of red/cyan.
         *		  Setting the rendering to greyscale (by pressing `g`) will show the effect best.
         *		- In view_mode_ == STEREO_SPLIT, you can use `glViewport` to render the left-eye image on the left
         *		 screen half and the right-eye image on the right screen half. Remeber to change the width of the
         *		 frustum.
         *      - Do not forget to reset viewport and color mask.
		 *
         *		Hint: You need to clear the depth buffer between the two `draw_scene` calls.
         *
         */

        double focal_distance = distance(eye, center) + 3.0 * radius;
        double eye_separation = focal_distance * 0.008;

    }
}

//-----------------------------------------------------------------------------

void Solar_viewer::draw_scene(mat4& projection, mat4& view)
{
    // the matrices we need: model, modelview, modelview-projection, normal
    mat4 m_matrix;
    mat4 mv_matrix;
    mat4 mvp_matrix;
    mat3 n_matrix;

    // the sun is centered at the origin and -- for lighting -- considered to be a point, so that is the light position in world coordinates
    vec4 light = vec4(0.0, 0.0, 0.0, 1.0); //in world coordinates
    // convert light into camera coordinates
    light = view * light;

#ifndef __EMSCRIPTEN__
    if (wireframe_)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

    // time variable for sun shader (optional)
    static float time = 0;
    if (timer_active_)
        time += time_step_;
    if (time > 1e6)
        time = -1e5;

        /** \todo Provide the correct input and draw order for all your objects.
    * 1. Add the `stars_` object to the scene. It is also just a sphere, but drawn from inside.
    *
    * 2. For all objects in the `planets_` vector, replace the `color_shader_` by the `phong_shader_`
    *    - Look at the shader code (`shader/phong.vert`, `shader/phong.frag`).
    *    - All uniforms must be passed here (`set_uniform(...)`).
    *    - When you're done, complete the shader code.
    *
    * 3. For Earth replace `phong_shader` by `earth_shader`.
    *    - You can identify a specific planet in the `planets_` vector by its name (e.g. `if(planet->name_ == "earth")`
    *    - Look at the shader code (`shader/earth.vert`, `shader/earth.frag`).
    *    - All uniforms must be passed here (`set_uniform(...)`).
    *    - Earth uses multiple textures, in `set_uniform(...)` you must pass the correct channel (0,1,2,...).
    *      Those channels are set in `Solar_viewer::initialize()` in the first argument of `Texture::init(...)`
    *    - When you're done, complete the shader code.
    *
    *
    * 5. Render your space ship with your phong shader.
    *
    * 6. Feel free to try out your own ideas.
	*/

    // render sun with simple shader
    m_matrix = sun_.model_matrix_;
    mv_matrix = view * m_matrix;
    mvp_matrix = projection * mv_matrix;
    color_shader_.use();
    color_shader_.set_uniform("modelview_projection_matrix", mvp_matrix);
    color_shader_.set_uniform("tex", 0);
    color_shader_.set_uniform("greyscale", (int)greyscale_);
    sun_.texture_.bind();
    unit_sphere_mesh_.draw();


    for (Space_Object* planet : planets_)
    {
        m_matrix = planet->model_matrix_;
        mv_matrix = view * m_matrix;
        mvp_matrix = projection * mv_matrix;

        color_shader_.set_uniform("modelview_projection_matrix", mvp_matrix);
        planet->texture_.bind();
        unit_sphere_mesh_.draw();
    }


    // check for OpenGL errors
    glCheckError();
}

void Solar_viewer::randomize_planets()
{
    std::cout << "Randomizing planets..." << std::endl;
    float temp_dt = time_step_;
    time_step_ = (float)(rand() % 2000000);
    timer();
    time_step_ = temp_dt;
}

//=============================================================================
