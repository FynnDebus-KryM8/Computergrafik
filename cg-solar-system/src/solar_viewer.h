//=============================================================================
//
//   Exercise code for the lecture "Computer Graphics"
//     by Prof. Mario Botsch, TU Dortmund
//
//   Copyright (C) by Computer Graphics Group, TU Dortmund
//
//=============================================================================
#pragma once
//=============================================================================

#include "gl.h"
#include "Window.h"
// #include "glfw_window.h"
#include "glmath.h"

#include "shader.h"
#include "texture.h"
#include "space_object.h"
#include "sphere.h"
#include "ship.h"


//=============================================================================

//! OpenGL viewer that handles all the rendering for us
// class Solar_viewer : public GLFW_window
class Solar_viewer : public Window
{
public:
    //! default constructor
    //! \_title the window's title
    //! \_width the window's width
    //! \_height the window's height
    Solar_viewer(const char* _title, int _width, int _height);

protected:
    //! function that is called on the creation of the widget for the initialisation of OpenGL
    virtual void initialize() override;

    //! resize function - called when the window size is changed
    virtual void resize(int width, int height) override;

    //! render function - called when the window should be refreshed
    virtual void render() override;

    //! keyboard interaction
    virtual void keyboard(int key, int scancode, int action, int mods) override;

    //! mouse interaction
    virtual void mouse(int button, int action, int mods) override;

    //! mouse motion
    virtual void motion(double xpos, double ypos) override;

    //! mouse wheel
    virtual void scroll(double xoffset, double yoffset) override;

    //! update function on every timer event (controls the animation)
    virtual void timer() override;

private:
    //! function that draws the planet system
    //! \param projection the projection matrix for the scene
    //! \param view the view matrix for the scene
    void draw_scene(mat4& projection, mat4& view);

    //! sets a random day in the future and updates the planets positions to that day
    void randomize_planets();

    //! the sun object
    Space_Object sun_;
    //! the mercury object
    Space_Object mercury_;
    //! the venus object
    Space_Object venus_;
    //! the earth object
    Earth earth_;
    //! the moon object
    Moon moon_;
    //! the mars object
    Space_Object mars_;
    //! the jupiter object
    Space_Object jupiter_;
    //! the saturn object
    Space_Object saturn_;
    //! the stars object
    Space_Object stars_;
    /// spaceship object
    Ship ship_;
    //! some convenience vectors
    std::vector<Space_Object*> planets_;
    std::vector<Space_Object*> space_objects_;

    //! the mesh that is drawn for each spherical object in the scene
    Sphere unit_sphere_mesh_;

    //! default color shader (renders only the texture)
    Shader color_shader_;
    //! phong shader (renders texture and basic illumination)
    Shader phong_shader_;
    //! earth shader (renders the earth using multi texturing)
    Shader earth_shader_;
    Shader sun_shader_;

    //! interval for the animation timer
    bool timer_active_;
    //! update factor for the animation
    float time_step_;

    //! state whether the rendering should be in color or not
    bool greyscale_;

    //! state whether the rendering should be in wireframe (outlines of triangles)
    bool wireframe_;

    //! state whether the rendering should be in stereo or not
    enum
    {
        MONO,
        STEREO_ANAGLYPH,
        STEREO_SPLIT
    } view_mode_;

    //! the field of view for the camera
    float fovy_;
    //! the near plane for the virtual camera
    float near_;
    //! the far plane for the virtual camera
    float far_;

    //! which planet are we looking at (control with key 1-6)
    Space_Object* look_at_;

    //! eye's distance in radii from the observed planet
    float dist_factor_;
    /// true, if we look at the spaceship
    bool in_ship_;
    //! true if left mouse button is pressed
    bool mouse_down_;

    //! true if mouse is inside window while dragging
    bool mouse_inside_;

    //! some camera mouse rotation variables
    float last_x_, last_y_;
    bool last_point_ok_;
    vec3 last_point_3d_;

    //! rotation of eye position around look_at_'s position
    mat4 camera_rotation_;
};

//=============================================================================
