#include "space_object.h"

void Space_Object::update()
{
    const mat4 translate_distance = mat4::translate(vec3(distance_, 0.0, 0.0));
    const mat4 scale_radius = mat4::scale(radius_);
    const mat4 self_rotation = mat4::rotate_y(angle_self_);
    const mat4 self_angle_tilt = mat4::rotate_z(angle_tilt_);
    const mat4 parent_rotation = mat4::rotate_y(angle_parent_); // rotates around origin
    model_matrix_ =
        parent_rotation *
            translate_distance *
                scale_radius *
                    self_rotation *
                        self_angle_tilt;

    position_ = model_matrix_ * vec4(0.0f, 0.0f, 0.0f, 1.0f);

    // std::cout << "here is: " << name_ << " with position : " << position_ << std::endl;

    /** \todo Update `model_matrix_` and position (`position_`) for each object.
    * 1. Combine translation and scaling matrices to get a result like in screenshots/planet_system_initial.png assuming:
    *   - sun at origin
    *   - all objects scaled by radius
    *   - all objects translated by `distance` in x direction
    * 2. Now use y-axis-rotation matrices in the right places to allow rotation
    *    around the object's axis (angle_self) and sun (angle_parent)
    * 3. You can also support a tilt angle if you like (`angle_tilt`) rotations
    * 4. Update the object's position using your constructed model matrix
    * Hints:
    *   - See glmath.h/cpp for an overview about implemented matrices.
    *   - Order is important!
    */
}

//-----------------------------------------------------------------------------

void Moon::update()
{
    const mat4 scale_radius = mat4::scale(radius_);
    const mat4 self_rotation = mat4::rotate_y(angle_self_);
    const mat4 self_angle_tilt = mat4::rotate_z(angle_tilt_);

    const mat4 translate_distance = mat4::translate(vec3(distance_, 0.0, 0.0));
    const mat4 parent_rotation = mat4::rotate_y(angle_parent_); // rotates around parent

    const mat4 translate_to_parent = mat4::translate(vec3(parent_planet_->distance_, 0.0, 0.0));
    const mat4 parent_parent_rotation = mat4::rotate_y(parent_planet_->angle_parent_);

    model_matrix_ =
        parent_parent_rotation *
            translate_to_parent *
                parent_rotation *
                    translate_distance *
                        scale_radius *
                            self_rotation *
                                self_angle_tilt;

    position_ = model_matrix_ * vec4(0.0f, 0.0f, 0.0f, 1.0f);

    /** \todo Update moon's `model_matrix_` and position (`position_`).
    *  The moon is a bit special, it must rotate around its `parent_planet_`. Be careful with the
    *  translation-rotation-order since rotation matrices always rotate around the current origin.
    */
}

//=============================================================================
