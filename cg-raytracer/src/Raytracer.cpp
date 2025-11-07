//=============================================================================
//
//   Exercise code for the lecture
//   "Computer Graphics"
//   by Prof. Dr. Mario Botsch, TU Dortmund
//
//   Copyright (C) Computer Graphics Group, TU Dortmund.
//
//=============================================================================

//== includes =================================================================

#include "Raytracer.h"

#include <cfloat>

#include "Sphere.h"
#include "Plane.h"
#include "Mesh.h"

#include <vector>
#include <iostream>
#include <fstream>
// #include <cmath>

//-----------------------------------------------------------------------------

Raytracer::Raytracer(const std::string &filename) {
    read_scene(filename);
}

//-----------------------------------------------------------------------------

Raytracer::~Raytracer() {
    //clean up
    for (auto o: objects_) {
        delete o;
    }
}

//-----------------------------------------------------------------------------

void Raytracer::read_scene(const std::string &filename) {
    //clean up
    for (auto o: objects_) {
        delete o;
    }
    objects_.clear();
    lights_.clear();

    std::ifstream ifs(filename);
    if (!ifs) {
        std::cerr << "Cannot open file " << filename << std::endl;
        exit(1);
    }

    char line[200];
    std::string token;

    // parse file
    while (ifs && (ifs >> token) && (!ifs.eof())) {
        if (token[0] == '#') {
            ifs.getline(line, 200);
        } else if (token == "depth") {
            ifs >> max_depth_;
        } else if (token == "camera") {
            ifs >> camera_;
        } else if (token == "background") {
            ifs >> background_;
        } else if (token == "ambience") {
            ifs >> ambience_;
        } else if (token == "light") {
            Light light;
            ifs >> light;
            lights_.push_back(light);
        } else if (token == "plane") {
            auto *p = new Plane;
            ifs >> (*p);
            objects_.push_back(p);
        } else if (token == "sphere") {
            auto *sphere = new Sphere();
            ifs >> (*sphere);
            objects_.push_back(sphere);
        } else if (token == "mesh") {
            std::string fn, mode;
            ifs >> fn >> mode;

            // add path of scene-file to mesh's filename
            std::string path(filename);
            path = path.substr(0, path.find_last_of('/') + 1);
            fn = path + fn;

            auto *mesh =
                    new Mesh((mode == "FLAT" ? Mesh::FLAT : Mesh::PHONG), fn);

            ifs >> mesh->material_;

            objects_.push_back(mesh);
        }
    }
    ifs.close();

    std::cout << "\ndone (" << objects_.size() << " objects)\n";
}

//-----------------------------------------------------------------------------

void Raytracer::compute_image() {
    // allocate memory by resizing image
    image_.resize(camera_.width_, camera_.height_);

    Image tmpimage;
    tmpimage.resize(camera_.width_, camera_.height_);


#pragma omp parallel for
    for (int x = 0; x < camera_.width_; ++x) {
        for (int y = 0; y < camera_.height_; ++y) {
            Ray ray = camera_.primary_ray(x, y);
            vec3 color = trace(ray, 0);

            // avoid over-saturation
            color = min(color, vec3(1, 1, 1));

            // store pixel color
            image_(x, y) = tmpimage(x, y) = color;
        }
    }

    // You can lower this variable, if the computation takes to long
    const int subp = 4;
    const double threshold = 0.02;


    // do adaptive supersampling
#pragma omp parallel for
    for (int x = 1; x < camera_.width_ - 1; ++x) {
        for (int y = 1; y < camera_.height_ - 1; ++y) {
            // check if pixel color deviates to much from the surrounding
            vec3 color(0, 0, 0);
            vec3 c = tmpimage(x, y);
            double n = normSq(c - tmpimage(x + 1, y)) +
                       normSq(c - tmpimage(x, y + 1)) +
                       normSq(c - tmpimage(x - 1, y)) +
                       normSq(c - tmpimage(x, y - 1));

            if (n > threshold) {
                Ray ray;

                // shoot 16 rays through the pixel patch
                for (int si = 0; si < subp; si++) {
                    const double xoffset =
                            (si) / static_cast<double>(subp) - 0.5 + 1.0 / (2.0 * subp);
                    for (int sj = 0; sj < subp; sj++) {
                        const double yoffset =
                                (sj) / static_cast<double>(subp) - 0.5 + 1.0 / (2.0 * subp);
                        ray = camera_.primary_ray(
                            static_cast<double>(x) + xoffset,
                            static_cast<double>(y) + yoffset);
                        color += trace(ray, 0);
                    }
                }
                color /= subp * subp;

                // avoid over-saturation
                color = min(color, vec3(1, 1, 1));

                // store pixel color
                image_(x, y) = color;
            }
        }
    }
}

//-----------------------------------------------------------------------------

void Raytracer::write_image(const std::string &filename) {
    image_.write_png(filename);
}

//-----------------------------------------------------------------------------

vec3 Raytracer::trace(const Ray &ray, const int depth) {
    // stop if recursion depth (=number of reflection) is too large
    if (depth > max_depth_)
        return {0.0, 0.0, 0.0};

    // Find first intersection with an object. If an intersection is found,
    // it is stored in object, point, normal, and t.
    Material material;
    vec3 point;
    vec3 normal;
    double t;
    if (!intersect_scene(ray, material, point, normal, t)) {
        return background_;
    }

    // compute local Phong lighting (ambient+diffuse+specular)

    vec3 color = lighting(point, normal, -ray.direction_, material);

    //std::cout << color << std::endl;

    /** \todo
     * Compute reflections by recursive ray tracing:
     * - check whether `object` is reflective by checking its `material.mirror`
     * - check recursion depth
     * - generate reflected ray, compute its color contribution, and mix it with
     * the color computed by local Phong lighthing (use `material.mirror` as weight)
     * - check whether your recursive algorithm reflects the ray `max_depth_` times
     */
    if (material.mirror > 0.0) {
        Ray reflected_ray = Ray(point, reflect(ray.direction_, normal));
        return (1-material.mirror) * color + (material.mirror * trace(reflected_ray, (depth + 1)));
    }
    return color;
}

//-----------------------------------------------------------------------------

bool Raytracer::intersect_scene(const Ray &ray, Material &intersection_material,
                                vec3 &intersection_point,
                                vec3 &intersection_normal,
                                double &intersection_distance) const {
    double t, tmin(DBL_MAX);
    vec3 p, n, d;

    for (Object_ptr o: objects_) // for each object
    {
        if (o->intersect(ray, p, n, d, t)) // does ray intersect object?
        {
            if (t < tmin) // is intersection point the currently closest one?
            {
                tmin = t;
                intersection_material = o->material_;
                intersection_material.diffuse = d;
                intersection_point = p;
                intersection_normal = n;
                intersection_distance = t;
            }
        }
    }

    return (tmin < DBL_MAX);
}

//-----------------------------------------------------------------------------

vec3 Raytracer::lighting(const vec3 &point, const vec3 &normal,
                         const vec3 &view, const Material &material) const {
    vec3 color(0.0, 0.0, 0.0);

    color += ambience_ * material.ambient;

    for (Light l: lights_) {
        vec3 l_ = normalize(l.position - point); //light-vector

        //shadow ray casting
        Ray shadow_ray = Ray(point, l_);
        Material _material;
        vec3 _point;
        vec3 _normal;
        double t;
        if (intersect_scene(shadow_ray, _material, _point, _normal, t)) {
            if (t > 0.0 && t < norm(l.position - point)) {
                //std::cout << t << std::endl;
                continue;
            }
        }

        vec3 r = mirror(l_, normal);
        double n_l = dot(normal, l_); // cosine of angle between normal and light-vector
        double r_v = dot(r, view); //cosine of angle between view-vector and reflected light mirror

        if (n_l > 0.0) {
            color += l.color * material.diffuse * n_l; //diffuse light
            if (r_v > 0.0) {
                color += l.color * material.specular * pow(r_v, material.shininess); //specular light
            }
        }
    }


    /** \todo
    * Compute the Phong lighting:
    * - start with global ambient contribution
    * - for each light source (stored in vector `lights_`) add diffuse and specular contribution
    * - only add diffuse and specular light if object is not in shadow
    *
    * Hints:
    * - All object specific material parameters (material's diffuse, specular, shininess) can be found in `material`.
    * - The ambient light intensity parameter is defined as `ambience_`.
    * - Use the lights' member `color` for both diffuse and specular light intensity.
    * - Feel free to use the existing vector functions in `utils/vec3.h` e.g. mirror, reflect, norm, dot, normalize
    */


    return color;
}

//=============================================================================
