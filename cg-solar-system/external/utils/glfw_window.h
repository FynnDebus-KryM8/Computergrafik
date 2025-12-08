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
#include "glmath.h"

#include <GLFW/glfw3.h>


//== CLASS DEFINITION =========================================================


/// \addtogroup glfw_window glfw_window
/// @{


/// Simple GLFW window
class GLFW_window
{

public: //------------------------------------------------------ public methods

    //! constructor
    GLFW_window(const char* _title="", int _width=0, int _height=0);

    //! destructor
    virtual ~GLFW_window();

    //! main window loop
    int run();



private: //----------------------------- static wrapper functions for callbacks
    
    static void error__(int error, const char *description);
    static void keyboard__(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse__(GLFWwindow* window, int button, int action, int mods);
    static void resize__(GLFWwindow* window, int width, int height);
    static void motion__(GLFWwindow* window, double xpos, double ypos);
    static void scroll__(GLFWwindow* window, double xoffset, double yoffset);

    static GLFW_window *instance__;



protected: //----------------------------------- callbacks as member functions

    //! must overload: initialize all OpenGL states
    virtual void initialize() = 0;

    //! must overload: handle window resizing
    virtual void resize(int width, int height) = 0;

    //! must overload: render the scene
    virtual void render() = 0;

    //! may overload: handle keyboard events
    virtual void keyboard(int key, int scancode, int action, int mods) {}

    //! may overload: handle keyboard events
    virtual void mouse(int button, int action, int mods){}

    //! this function handles mouse motion (passive/active position)
    virtual void motion(double xpos, double ypos) {}

    //! this function handles mouse scroll events
    virtual void scroll(double xoffset, double yoffset) {}

    //! may overload: handle idle timer
    virtual void timer() {}



protected: //----------------------------------------------------- protected data

    //! virtual trackball: map 2D screen point to unit sphere. used by rotate().
    bool map_to_sphere(const int point_x, const int point_y, vec3& result);

    /// GLFW window pointer
    GLFWwindow *window_;

    /// current viewport dimension
    int  width_, height_;
};


/// @}


//=============================================================================

