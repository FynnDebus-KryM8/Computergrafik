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

#include "mesh.h"

//=============================================================================

/// class that creates a sphere with a desired tessellation degree and renders it
class Sphere :public Mesh
{
public:

    /// default constructor
    /// \param resolution the degree of the tessellation of the sphere
    Sphere(unsigned int resolution=10);


    /// render mesh of the sphere
    void draw(GLenum mode=GL_TRIANGLES) override;


private:

    /// generate sphere vertices/triangles and OpenGL buffers
    void initGlArrays();


private:

    /// tessellation resolution
    unsigned int resolution_;
};


//=============================================================================
