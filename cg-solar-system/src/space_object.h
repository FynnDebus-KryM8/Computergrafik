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

#include "texture.h"
#include "glmath.h"

//=============================================================================

//! default class for the planet
class Space_Object
{
public:
    //! default constructor
    Space_Object(const std::string &name, const float _angle_step_parent,
                 const float _angle_step_self, const float _radius,
                 const float _distance, const float _tilt)
        : position_(0, 0, 0, 1),
          distance_(_distance),
          radius_(_radius),
          angle_parent_(0),
          angle_self_(0),
          angle_step_parent_(_angle_step_parent),
          angle_step_self_(_angle_step_self),
          model_matrix_(mat4::identity()),
          name_(name),
          angle_tilt_(_tilt){
    }

    virtual ~Space_Object() = default;

    //! set the time for every update
    void time_step(float _days)
    {
        angle_parent_ += _days * angle_step_parent_;
        angle_self_ += _days * angle_step_self_;
    }

    virtual void update();

    //! the current position of the planet
    vec4 position_;
    //! planet's distance to parent object (sun in most cases)
    float distance_;
    //! the radius of the planet
    float radius_;

    //! current rotation around the parent object (sun in most cases)
    float angle_parent_;
    //! current rotation around planet center
    float angle_self_;
    //! tilt angle (angle between y-axis and planets rotation axis)
    float angle_tilt_;

    //! rotation update each day for angle_parent_ (in radians)
    float angle_step_parent_;
    //! rotation update each day for angle_self_ (in radians)
    float angle_step_self_;

    //! main diffuse texture
    Texture texture_;

    //! planet's model matrix
    mat4 model_matrix_;

    //! planet's name
    std::string name_;
};

//=============================================================================

//! earth is a special planet, because we need access to more textures
class Earth : public Space_Object
{
public:
    //! default constructor for the earth
    Earth(const std::string &name, const float _update,
          const float _update_self, const float _scale, const float _distance,
          const float _tilt)
        : Space_Object(name, _update, _update_self, _scale, _distance, _tilt)
    {
    }

    //! diffuse texture for the night
    Texture night_;

    //! texture with the clouds
    Texture cloud_;

    //! the gloss texture - defines where the surface si glossy or not
    Texture gloss_;

    //! the normal texture - detailed normal maps
    Texture normal_;
};

//! Moon is a special planet, because it does not evolve around the sun
class Moon : public Space_Object
{
public:
    //! default constructor for the earth
    Moon(const std::string &name, const float _update, const float _update_self,
         const float _scale, const float _distance, const float _tilt)
        : Space_Object(name, _update, _update_self, _scale, _distance, _tilt)
    {
    }

    virtual void update() override;

    Space_Object *parent_planet_;
};

//=============================================================================
