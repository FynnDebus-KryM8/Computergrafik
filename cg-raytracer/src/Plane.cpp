//=============================================================================
//
//   Exercise code for the lecture
//   "Computer Graphics"
//   by Prof. Dr. Mario Botsch, TU Dortmund
//
//   Copyright (C) Computer Graphics Group, TU Dortmund.
//
//=============================================================================

//== INCLUDES =================================================================

#include "Plane.h"
#include <cfloat>

//== CLASS DEFINITION =========================================================

Plane::Plane(const vec3 &c, const vec3 &n) : center_(c), normal_(n) {
}

//-----------------------------------------------------------------------------

bool Plane::intersect(const Ray &ray, vec3 &intersection_point,
                      vec3 &intersection_normal, vec3 &intersection_diffuse,
                      double &intersection_distance) const {
    intersection_diffuse = material_.diffuse;

    double d = dot(normal_, center_);
    double n_o = dot(normal_, ray.origin_);
    double n_dir = dot(normal_, ray.direction_);

    if (n_dir == 0) return false;
    double t = (d - n_o) / (n_dir);

    if (t > 1e-5) {
        //std::cout << t << std::endl;
        intersection_distance = DBL_MAX;
        if (t < intersection_distance)
            intersection_distance = t;

        // was the intersection not just a numerical problem?
        if (intersection_distance != DBL_MAX) {
            //std::cout << n_dir << std::endl;
            // return intersection data
            intersection_point = ray(intersection_distance);
            intersection_normal = normal_;

            return true;
        }
    }

    /** \todo
 * - compute the intersection of the plane with `ray`
 * - if ray and plane are parallel there is no intersection
 * - otherwise compute intersection data and store it in `intersection_point`, `intersection_normal`, and `intersection_distance`.
 * - return whether there is an intersection for t>1e-5 (avoids "shadow acne").
*/


    return false;
}

//=============================================================================
