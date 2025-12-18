#ifndef SHIP_H
#define SHIP_H

#include "glmath.h"
#include <vector>
#include "texture.h"
#include "mesh.h"

class Ship : public Mesh
{
public:
    Ship();

    /// loads vertices and faces from .off file
    bool load_model(std::string filename);

    /// updates ships position and angle
    void update_ship();

    /// changes ship's forward speed
    void accelerate(float speedup);

    /// changes ship's angular speed
    void accelerate_angular(vec3 angular_speedup);

    /// draws the ship
    void draw(GLenum mode = GL_TRIANGLES) override;

    /// main diffuse texture for the planet
    Texture texture_;
    bool initGlArrays();
private:
    void compute_normals();


    /// vertex array
    std::vector<vec3> vertices_;
    /// triangle index array
    std::vector<unsigned int> indices_;
    /// vertex normals
    std::vector<vec3> vertex_normals_;
    /// face normals
    std::vector<vec3> face_normals_;



public:

    //! ship's model matrix
    mat4 model_matrix_;

    /// current position
    vec4 position_;

    /// current pitch-yaw-roll angle
    vec3 angle_;

    /// current direction in which the ship faces
    vec4 direction_;

    /// current forward speed (pos_ += speed*direction)
    float speed_;

    /// current angular speed (angle_ += angular_speed)
    vec3 angular_speed_;

    /// ships radius
    float radius_;
};


#endif
