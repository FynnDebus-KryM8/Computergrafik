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
#include <set>

//=============================================================================

/// shader class for easy handling of the shader
class Shader
{
public:

    /// default constructor
    Shader();
    /// default destructor
    ~Shader();


    /// load (from file), compile, and link vertex and fragment shader,
    /// optionially also a geometry shader
    /// \param vfile string with the adress to the vertex shader
    /// \param ffile string with the adress to the fragment shader
    /// \param gfile optional string with the adress to the geometry shader
    bool load(std::string vfile, std::string ffile, const char* gfile=NULL);

    /// deletes all shader and frees GPU shader capacities
    void  cleanup();

    /// enable/bind this shader program
    void use();
    /// disable/unbind this shader program
    void disable();

    /// upload bool uniform
    /// \param name string of the uniform name
    /// \param value the value for the uniform
    void set_uniform(const char* name, bool value);
    /// upload float uniform
    /// \param name string of the uniform name
    /// \param value the value for the uniform
    void set_uniform(const char* name, float value);
    /// upload int uniform
    /// \param name string of the uniform name
    /// \param value the value for the uniform
    void set_uniform(const char* name, int   value);
    /// upload vec3 uniform
    /// \param name string of the uniform name
    /// \param vec the value for the uniform
    void set_uniform(const char* name, const vec3& vec);
    /// upload vec4 uniform
    /// \param name string of the uniform name
    /// \param vec the value for the uniform
    void set_uniform(const char* name, const vec4& vec);
    /// upload mat3 uniform
    /// \param name string of the uniform name
    /// \param mat the value for the uniform
    void set_uniform(const char* name, const mat3& mat);
    /// upload mat4 uniform
    /// \param name string of the uniform name
    /// \param mat the value for the uniform
    void set_uniform(const char* name, const mat4& mat);


private:
    /// loads a vertex/fragmend/geometry shader from a file and compiles it
    /// \param filename the location and name of the shader
    /// \param type the type of the shader (vertex, geometry, fragment)
    GLint load_and_compile(const char* filename, GLenum type);

private:
    /// id of the linked shader program
    GLint pid_;
    /// id of the vertex shader
    GLint vid_;
    /// id of the fragmend shader
    GLint fid_;
    /// id of the geometry shader
    GLint gid_;


    /// store invalid uniform names to suppress printing multiple times
    std::set<std::string> invalid_names_;
};

//=============================================================================
