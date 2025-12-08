//=============================================================================
//
//   Exercise code for the lecture "Computer Graphics"
//     by Prof. Mario Botsch, TU Dortmund
//
//   Copyright (C) by Computer Graphics Group, TU Dortmund
//
//=============================================================================

#include "sphere.h"
#include "glmath.h"
#include <iostream>

//=============================================================================


Sphere::Sphere(unsigned int resolution) :
    Mesh(), resolution_(resolution)
{
}


//-----------------------------------------------------------------------------


void Sphere::initGlArrays()
{
    const unsigned int v_resolution =     resolution_;
    const unsigned int u_resolution = 2 * resolution_;
    const unsigned int n_vertices   = (v_resolution) * u_resolution;
    const unsigned int n_triangles  = 2 * (v_resolution-1) * (u_resolution-1);

    GLfloat *positions = new GLfloat[3*n_vertices];
    GLfloat *normals   = new GLfloat[3*n_vertices];
    GLfloat *texcoords = new GLfloat[2*n_vertices];
    GLuint  *indices   = new GLuint[3*n_triangles];

    unsigned int p(0), n(0), t(0), i(0);

    // generate vertices
    for (unsigned int iv=0; iv<v_resolution; ++iv)
    {
        for (unsigned int iu=0; iu<u_resolution; ++iu)
        {
            float u = (float) iu / (float) (u_resolution-1);
            float v = (float) iv / (float) (v_resolution-1);

            float theta = u * 2.0 * M_PI;
            float phi   = v * M_PI;

            float x = cos(theta) * sin(phi);
            float y = cos(phi);
            float z = sin(theta) * sin(phi);

            positions[p++] = x;
            positions[p++] = y;
            positions[p++] = z;

            normals[n++] = x;
            normals[n++] = y;
            normals[n++] = z;

            texcoords[t++] = 1.0-u;
            texcoords[t++] = 1.0-v;
        }
    }


    // generate triangles
    for (unsigned int v=0; v<v_resolution-1; ++v)
    {
        for (unsigned int u=0; u<u_resolution-1; ++u)
        {
            unsigned int i0 = (u  ) + (v  ) * u_resolution;
            unsigned int i1 = (u+1) + (v  ) * u_resolution;
            unsigned int i2 = (u+1) + (v+1) * u_resolution;
            unsigned int i3 = (u  ) + (v+1) * u_resolution;

            indices[i++] = i0;
            indices[i++] = i1;
            indices[i++] = i2;

            indices[i++] = i0;
            indices[i++] = i2;
            indices[i++] = i3;
        }
    }

    initialize(positions, normals, texcoords, indices, n_triangles, n_vertices, 3*n_triangles);

    // clean up
    delete[] positions;
    delete[] normals;
    delete[] texcoords;
    delete[] indices;
}


//-----------------------------------------------------------------------------


void Sphere::draw(GLenum mode)
{
    if (n_indices_==0) initGlArrays();

    glBindVertexArray(vao_);
    glDrawElements(mode, n_indices_, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);
}


//=============================================================================
