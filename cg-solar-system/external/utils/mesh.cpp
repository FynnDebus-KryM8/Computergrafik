//=============================================================================
//
//   Exercise code for the lecture "Computer Graphics"
//     by Prof. Mario Botsch, TU Dortmund
//
//   Copyright (C) by Computer Graphics Group, TU Dortmund
//
//=============================================================================

#include "mesh.h"

//=============================================================================


Mesh::Mesh() :
    n_indices_(0),
    vao_(0),
    vbo_(0),
    nbo_(0),
    tbo_(0),
    ibo_(0)
{
}


//-----------------------------------------------------------------------------


Mesh::~Mesh()
{
    if (vbo_)  glDeleteBuffers(1, &vbo_);
    if (nbo_)  glDeleteBuffers(1, &nbo_);
    if (tbo_)  glDeleteBuffers(1, &tbo_);
    if (ibo_)  glDeleteBuffers(1, &ibo_);
    if (vao_)  glDeleteVertexArrays(1, &vao_);
}

void Mesh::draw(GLenum mode)
{
    if (n_indices_==0)
    {
        std::cerr << "Mesh not initialized!" << std::endl;
        return;
    }

    glBindVertexArray(vao_);
    glDrawElements(mode, n_indices_, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);
}


//-----------------------------------------------------------------------------


void Mesh::initialize(GLfloat *positions, GLfloat *normals, GLfloat *texcoords, GLuint  *indices, int n_triangles, int n_vertices, int n_indices)
{
    n_indices_ = n_indices;

    // generate vertex array object
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);


    // vertex positions -> attribute 0
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, 3*n_vertices*sizeof(float), positions, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // normal vectors -> attribute 1
    glGenBuffers(1, &nbo_);
    glBindBuffer(GL_ARRAY_BUFFER, nbo_);
    glBufferData(GL_ARRAY_BUFFER, 3*n_vertices*sizeof(float), normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // texture coordinates -> attribute 2
    glGenBuffers(1, &tbo_);
    glBindBuffer(GL_ARRAY_BUFFER, tbo_);
    glBufferData(GL_ARRAY_BUFFER, 2*n_vertices*sizeof(float), texcoords, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // triangle indices
    glGenBuffers(1, &ibo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*n_triangles*sizeof(GLuint), indices, GL_STATIC_DRAW);
}

//-----------------------------------------------------------------------------

bool Mesh::initialize(const std::vector<vec3> &_positions, const std::vector<vec3> &_normals, const std::vector<float> &u_coordinates, const std::vector<float> &v_coordinates, const std::vector<unsigned int> &_indices)
{
    const unsigned int n_vertices = _positions.size();
    const unsigned int n_triangles = _indices.size()/3;

    // some error checks:
    if(_normals.size() != n_vertices || u_coordinates.size() != n_vertices || v_coordinates.size() != n_vertices)
    {
        std::cerr << "\n!!!!!\n!!!!!\nError: positions, normals and texcoords must have the same size!\n!!!!!\n!!!!!\n" << std::endl;
        return false;
    }
    unsigned int maxi = 0;
    for(auto i : _indices)
    {
        if(i > maxi)
            maxi = i;
    }
    if(maxi >= n_vertices && n_vertices > 0)
    {
        std::cerr << "\n!!!!!\n!!!!!\nError: At least one of your indices points to an undefined vertex!\n!!!!!\n!!!!!\n" << std::endl;
        return false;
    }
    if(_indices.size() % 3 != 0)
    {
        std::cerr << "\n!!!!!\n!!!!!\nError: Your index vectors' size is not a multiple of 3!\n!!!!!\n!!!!!\n" << std::endl;
        return false;
    }

    GLfloat *positions = new GLfloat[3 * n_vertices];
    GLfloat *normals = new GLfloat[3 * n_vertices];
    GLuint  *indices = new GLuint[3 * n_triangles];
    GLfloat *texcoords = new GLfloat[2 * n_vertices];

    for(size_t p = 0; p < _positions.size(); p++)
    {
        positions[3*p + 0] = _positions[p].x;
        positions[3*p + 1] = _positions[p].y;
        positions[3*p + 2] = _positions[p].z;

        normals[3*p + 0] = _normals[p][0];
        normals[3*p + 1] = _normals[p][1];
        normals[3*p + 2] = _normals[p][2];

        texcoords[2*p + 0] = u_coordinates[p];
        texcoords[2*p + 1] = v_coordinates[p];
    }

    for(size_t i = 0; i < _indices.size(); i++)
    {
        indices[i] = _indices[i];
    }

    initialize(positions, normals, texcoords, indices, n_triangles, n_vertices, 3*n_triangles);

    delete[] positions;
    delete[] normals;
    delete[] texcoords;
    delete[] indices;

    return true;
}

//=============================================================================
