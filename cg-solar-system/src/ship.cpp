#include "ship.h"
#include <fstream>
#include <string>

Ship::Ship()
    :Mesh(),
      position_(0, 0, 0, 1), direction_(0, 0, 1, 0), speed_(0), angle_(0), angular_speed_(0), radius_(0.01)
{}

bool Ship::load_model(std::string _filename)
{

    // implements a simple OFF reader

    // open file
    std::ifstream ifs(_filename);
    if (!ifs)
    {
        std::cerr << "Can't open " << _filename << "\n";
        return false;
    }

    // read OFF header
    std::string s;
    ifs >> s;
    if (s != "OFF")
    {
        std::cerr << "No OFF file\n";
        return false;
    }

    unsigned int nV, nF, dummy, i, j;
    ifs >> nV >> nF >> dummy;
    std::cout << _filename << ": " << nV << " vertices, " << nF << " triangles\n";

    vertices_.resize(nV);
    // read vertices
    for (i = 0; i<nV; ++i)
    {
        ifs >> vertices_[i];
    }

    indices_.resize(nF * 3);
    // read faces
    for (i = 0; i<nF; ++i)
    {
        ifs >> dummy;
        if (dummy == 3)
        {
            for (j = 0; j < 3; j++)
            {
                ifs >> indices_[3 * i + j];
            }
        }
        else
        {
            std::cerr << "No triangle mesh\n";
            return false;
        }
    }

    // close file
    ifs.close();
    compute_normals();
    initGlArrays();

    return true;
}

void Ship::compute_normals()
{
    int nF = indices_.size()/3;
    int nV = vertices_.size();

    vertex_normals_.resize(nV, vec3(0, 0, 0));
    for (int i = 0; i<nF; i++)
    {
        vec3 a, b;
        a = vertices_[indices_[3 * i + 1]] - vertices_[indices_[3 * i + 0]];
        b = vertices_[indices_[3 * i + 2]] - vertices_[indices_[3 * i + 0]];
        face_normals_.push_back(normalize(cross(a, b)));

        vertex_normals_[indices_[3 * i + 0]] += face_normals_[i];
        vertex_normals_[indices_[3 * i + 1]] += face_normals_[i];
        vertex_normals_[indices_[3 * i + 2]] += face_normals_[i];
    }

    for (int i = 0; i<nV; i++)
    {
        vertex_normals_[i] = normalize(vertex_normals_[i]);
    }
}

void Ship::accelerate(float speedup)
{
    speed_ += speedup;
    speed_ = std::max(0.0f, std::min(0.03f,speed_));
}

void Ship::accelerate_angular(vec3 angular_speedup)
{
    angular_speed_ += angular_speedup;
}

void Ship::update_ship()
{
    angle_ += angular_speed_;
    angular_speed_ *= 0.98;
    mat4 rot = mat4::rotate_x(angle_.x) * mat4::rotate_y(angle_.y) * mat4::rotate_z(angle_.z);
    direction_ = rot*vec4(1,1,1,0);
    position_ += speed_*direction_;
    model_matrix_ = mat4::translate(vec3(position_[0], position_[1], position_[2]))*rot*mat4::scale(radius_);
}

void Ship::draw(GLenum mode)
{
    if (n_indices_==0) initGlArrays();

    glBindVertexArray(vao_);
    glDrawElements(mode, n_indices_, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);

}

bool Ship::initGlArrays()
{
    std::vector<float> texcoords;
    // generate texcoords
    for (unsigned int v = 0; v<vertices_.size(); ++v)
    {
        texcoords.push_back(0.5);
    }

    if(!initialize(vertices_, vertex_normals_, texcoords, texcoords, indices_))
    {
        std::cerr << "\nError: Ship cannot be initialized!\n" << std::endl;
        return false;
    }

    return true;
}
