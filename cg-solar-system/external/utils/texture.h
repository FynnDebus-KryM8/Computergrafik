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
#include <vector>

//=============================================================================

/// class that handles texture io and GPU upload
class Texture
{
public:

    /// default constructor
    Texture();

    /// default destructor
    ~Texture();

    /// creates an empty texture on the GPU
    /// \param unit texture unit (important for use of multiple textures in shader)
    /// \param type the type of the texture (1D, 2D, 3D etc.)
    /// \param minfilter interpolation filter for minification
    /// \param magfilter interpolation filter for magnification
    /// \param wrap texture coordinates wrap preference
    void init(GLenum unit, GLenum type, GLint minfilter, GLint magfilter, GLint wrap);

    /// uploads a texture to the GPU
    /// \param filename the location and name of the texture for upload
    bool loadPNG(std::string filename);

    /// creates a texture from  a vector of floats [r,g,b,a , r,g,b,a , r,g,b,a , ...]
    void loadFromVector(const std::vector<float> &img, int width, int height);

    /// activates this texture for the predefined unit
    void bind();

    /// returns the texture id
    GLint id() const { return id_; }

private:

    /// texture ID on GPU
    GLuint id_;

    /// texture unit (important for use of multiple textures in shader)
    GLenum unit_;

    /// the type of the texture (TEXTURE_1D,TEXTURE_2D, etc.)
    GLint  type_;

    /// the minfilter setting
    GLint minfilter_;

};

//=============================================================================
