#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <vector>
#include <stdio.h>
#include <iostream>
#include <memory>
#include <algorithm>
#include <iomanip>

#include "../lib/glm/glm.hpp"
#include "../lib/glm/gtc/constants.hpp"
#include "../lib/glm/gtx/perpendicular.hpp"

namespace raytracer
{
    struct Intersection;

    typedef glm::vec3 Vector3f;
    typedef glm::vec2 Vector2f;
    typedef glm::vec3 Color;
    typedef std::vector<Intersection> intersectionList;

    enum Axis { x, y, z };

    inline Vector2f UV_mapping(const Vector3f& normal, const float& height, const float& width)
    {
        const float u = 0.5f - (glm::atan(normal.z, normal.x)) / (2.0f*glm::pi<float>());
        const float v = 0.5f - (glm::asin(normal.y)) / (glm::pi<float>());

        return Vector2f((int)glm::floor(u * width), (int)glm::floor(v * height));
    }
    inline bool isPointInSphere(const Vector3f &point, const Vector3f &position, const float &custom_radius)
    {
        if(glm::pow(point.x - position.x, 2.0f) +
           glm::pow(point.y - position.y, 2.0f) +
           glm::pow(point.z - position.z, 2.0f) <= custom_radius * custom_radius)
        {
           return true;
        }

        return false;
    }

    inline void debugVec3f(const Vector3f &vec)
    {
        std::cout << vec.x << " " << vec.y << " " << vec.z << std::endl;
    }
    inline void debugVec3f(const Vector3f &vec, const std::string &str)
    {
        std::cout << str << " " << vec.x << " " << vec.y << " " << vec.z << std::endl;
    }
    inline void debugFloat (const float &fl)
    {
        std::cout << fl << std::endl;
    }
    inline void debugFloat (const float &fl, const std::string &str)
    {
        std::cout << str << " " << fl << std::endl;
    }
    inline void debugString (const std::string &str)
    {
        std::cout << str << std::endl;
    }

    class Ray
    {
    public:
        Ray() : start(Vector3f()), dir(Vector3f()) { calc_cache(); }
        Ray(const Vector3f &st, const Vector3f &d) : start(st), dir(d) { calc_cache(); }
        Ray(const Vector3f &st, const Vector3f &d, const bool is_shadow) : start(st), dir(d), isShadowRay(is_shadow) { calc_cache(); }
        Vector3f start, dir, inv_dir;
        int sign[3];
        bool isShadowRay = false;
    private:
        void calc_cache()
        {
            inv_dir = 1.0f / dir;
            sign[0] = (inv_dir.x < 0);
            sign[1] = (inv_dir.y < 0);
            sign[2] = (inv_dir.z < 0);
        }
    };
    class Material
    {
    public:
        const Color& diffuse() const {return _diffuse; }
        const float& reflection() const {return _reflection; }

        void setDiffuse(const Color diffuse) { _diffuse = diffuse; }
        void setReflection(const float reflection) { _reflection = reflection; }

        Material(const Color &d, const float &refl) : _diffuse(glm::clamp(d, 0.0f, 1.0f)), _reflection(glm::clamp(refl, 0.0f, 1.0f))  {}
    private:
        Color _diffuse;
        float _reflection;
    };
    class Texture
    {
    public:
        const unsigned& width() const {return w;}
        const unsigned& height() const {return h;}
        const Color& texture(const unsigned &x, const unsigned &y) const {return t[y * w + x];}
        Texture(const std::vector<Color> &t, const unsigned &w, const unsigned &h) :w(w), h(h), t(t) {}
    private:
        const unsigned w, h;
        const std::vector<Color> &t;
    };
    class TextureMap
    {
    public:
        virtual Color getTextureMapping(Vector3f& point) const = 0;

        TextureMap(const Texture& t_map) : texture_map(t_map) {}
    //protected:
        Texture texture_map;
    };
    class DisplacementMap
    {
    public:
        virtual float getDispMapping(Vector3f& point) const = 0;

        const float& get_max_displacement() const {return max_displacement;}

        const float& get_du() const {return du;}
        const float& get_step() const {return step;}
        const float& get_epsilon() const {return epsilon;}

        void set_du(const float _du) {du = _du;}
        void set_step(const float _step) {step = _step;}
        void set_epsilon(const float _eps) {epsilon = _eps;}

        DisplacementMap(const Texture& d_map, const float& _du, const float& _stp, const float& _eps) : displacement_map(d_map), du(glm::abs(_du)), step(glm::abs(_stp)), epsilon(glm::abs(_eps)), max_displacement(calculate_max_displacement()) {}
    protected:
        Texture displacement_map;
        float du;
        float step;
        float epsilon;
        const float max_displacement;
    private:
        float calculate_max_displacement() const
        {
            float max_disp = 0.0f;

            for(unsigned y = 0; y < displacement_map.height(); ++y)
            {
                for(unsigned x = 0; x < displacement_map.width(); ++x)
                {
                    const Color& map = displacement_map.texture(x, y);
                    float df = 0.3f * map.r + 0.59f * map.g + 0.11f * map.b;

                    if(df > max_disp) max_disp = df;
                 }
            }

            return max_disp;
        }
    };

    class SphereTextureMap : public TextureMap
    {
    public:
        Color getTextureMapping(Vector3f& normal) const
        {
            const Vector2f &UV = UV_mapping(normal, texture_map.height(), texture_map.width());
            return texture_map.texture(UV.x, UV.y);
        }

        SphereTextureMap(const Texture& t_map) : TextureMap(t_map) {}
    };
    class SphereDisplacementMap : public DisplacementMap
    {
    public:
        float getDispMapping(Vector3f& point) const
        {
            const Vector2f &UV = UV_mapping(point, displacement_map.height(), displacement_map.width());
            const Color &color = displacement_map.texture(UV.x, UV.y);
            const float df = 0.3f * (color.r) + 0.59f * (color.g) + 0.11f * (color.b);
            return df * du;
        }

        SphereDisplacementMap(const Texture& d_map, const float& _du, const float& _stp, const float& _eps) : DisplacementMap(d_map, _du, _stp, _eps) {}
    };

    class Light
    {
    public:
        virtual Vector3f getDirection(const Vector3f& surfacePoint) const = 0;
        virtual Color getIntensity(const Vector3f& surfacePoint) const = 0;
    };
    class PointLight : public Light
    {
    public:
        Vector3f getDirection(const Vector3f& surfacePoint) const
        {
            return pos - surfacePoint;
        }
        Color getIntensity(const Vector3f& /*surfacePoint*/) const
        {
            return lightColor * intensity;
        }
        PointLight(const Vector3f &p, const Color &col, const glm::float32 &intens) : pos(p) , lightColor(glm::clamp(col, 0.0f, 1.0f)), intensity(glm::abs(intens)){}
    protected:
        Vector3f pos;
        Color lightColor;
        glm::float32 intensity;
    };
    class RealisticPointLight : public PointLight
    {
    public:
        Color getIntensity(const Vector3f& surfacePoint) const
        {
            const float r2 = glm::pow(glm::length(getDirection(surfacePoint)), exp);
            return (lightColor * intensity) / (4.0f * glm::pi<float>() * r2);
        }
        RealisticPointLight(const Vector3f &p, const Color &col, const glm::float32 &intens) : PointLight(p, col, intens), exp(2.0f){}
        RealisticPointLight(const Vector3f &p, const Color &col, const glm::float32 &intens, const glm::float32 &exponent) : PointLight(p, col, intens), exp(exponent){}
    private:
        glm::float32 exp;
    };

    class SolidObject
    {
    public:
         SolidObject(const Vector3f &pos, const Material &mat) : centroid(pos), material(mat) {}
         virtual bool intersect(const Ray &ray, intersectionList& list) const = 0;
         virtual Material surfaceMaterial(const Vector3f& /*surfacePoint*/) const { return material; }

         virtual const Vector3f getMinPoint() const = 0;
         virtual const Vector3f getMaxPoint() const = 0;

         Vector3f centroid;
    protected:
         Material material;
    };
    typedef std::vector<SolidObject*> SolidObjects;

    struct Intersection
    {
        glm::float32 distance;
        Vector3f point;
        Vector3f normal;
        const SolidObject *solid;
    };
    class Sphere : public SolidObject
    {
    public:
        Sphere(const Vector3f &pos, const float &rad, const Material &mat) : SolidObject(pos, mat), radius(rad)
        {
            intersect_func = &raytracer::Sphere::normal_intersect_wrapper;
            material_func = &raytracer::Sphere::get_normal_material;
        }
        Sphere(const Vector3f &pos, const float &rad, const Material &mat, const SphereTextureMap &txt) : SolidObject(pos, mat), radius(rad), texture_map(&txt)
        {
            intersect_func = &raytracer::Sphere::normal_intersect_wrapper;
            material_func = &raytracer::Sphere::get_texture_material;
        }
        Sphere(const Vector3f &pos, const float &rad, const Material &mat, const SphereDisplacementMap &disp) : SolidObject(pos, mat), radius(rad), displacement_map(&disp)
        {
            intersect_func = &raytracer::Sphere::disp_mapping_intersect_wrapper;
            material_func = &raytracer::Sphere::get_normal_material;
        }
        Sphere(const Vector3f &pos, const float &rad, const Material &mat, const SphereTextureMap &txt, const SphereDisplacementMap &disp) : SolidObject(pos, mat), radius(rad), texture_map(&txt), displacement_map(&disp)
        {
            intersect_func = &raytracer::Sphere::disp_mapping_intersect_wrapper;
            material_func = &raytracer::Sphere::get_texture_material;
        }

        bool intersect(const Ray &ray, intersectionList& list) const;

        Material surfaceMaterial(const Vector3f &surfacePoint) const;

        const Vector3f getMinPoint() const;
        const Vector3f getMaxPoint() const;

        float radius;
    private:     
        bool normal_intersect(const Ray &ray, Intersection &intsc, const glm::float32 &custom_radius) const;
        bool disp_mapping_intersect(const Ray &ray, intersectionList &list,const glm::float32 start_t) const;

        bool normal_intersect_wrapper(const Ray &ray, intersectionList &list) const;
        bool disp_mapping_intersect_wrapper(const Ray &ray, intersectionList &list) const;

        Material get_normal_material(const Vector3f& surfacePoint) const;
        Material get_texture_material(const Vector3f& surfacePoint) const;

        typedef bool (raytracer::Sphere::*IntersectFunc) (const Ray&, intersectionList&) const;
        IntersectFunc intersect_func;

        typedef Material (raytracer::Sphere::*MaterialFunc) (const Vector3f&) const;
        MaterialFunc material_func;

        const SphereTextureMap *texture_map = NULL;
        const SphereDisplacementMap *displacement_map = NULL;
    };
    class Plane : public SolidObject
    {
    public:
        Plane(const Vector3f &pos, const Vector3f &n, const Material &mat) : SolidObject(pos, mat), normal(n) {/*empty*/}
        bool intersect(const Ray &ray, intersectionList& list) const;

        const Vector3f getMinPoint() const;
        const Vector3f getMaxPoint() const;
    private:
        Vector3f normal;
    };
    class Triangle : public SolidObject
    {
    public:
        Triangle(const Vector3f &_v0, const Vector3f &_v1, const Vector3f &_v2, const Material &_mat, const bool &_culling = false) :
            SolidObject(Vector3f(0.0f,0.0f,0.0f), _mat), v{_v0, _v1, _v2}, culling(_culling)
        {
            centroid = (1.0f / 3.0f) * (v[0] + v[1] + v[2]);
        }
        bool intersect(const Ray &ray, intersectionList& list) const;

        const Vector3f getMinPoint() const;
        const Vector3f getMaxPoint() const;
    private:
        const Vector3f v[3];
        bool culling = false;
    };

    class AABB : public SolidObject
    {
        typedef std::unique_ptr<SolidObject> ptr;

    public:
        AABB(const Vector3f &_min, const Vector3f &_max) : SolidObject(Vector3f(0.0f,0.0f,0.0f), Material(Color(), 0.0f)), bounds { _min, _max } {}
        AABB(const SolidObjects& objects) : SolidObject(Vector3f(), Material(Color(), 0.0f))
        {
           /*
           for(unsigned i = 0; i < objects.size(); ++i)
           {
               debugVec3f(objects[i]->centroid);
           }
           debugString("");
           */

           if(objects.size() == 0)
               return;
           else if(objects.size() == 1)
           {
               left = objects[0];
               return;
           }
           else if(objects.size() == 2)
           {
               ComputeBoundingBox(objects);
               left = objects[0];
               right = objects[1];
               return;
           }

           ComputeBoundingBox(objects);
           Axis longest_axis = x;
           glm::float32 splitting_point = 0.0f;
           bool first_pass = true;

           splitting_point = ComputeSpatialMedian(longest_axis);

           SolidObjects subset_1;
           SolidObjects subset_2;

           create_subsets:
           for(unsigned int i = 0; i < objects.size(); ++i)
           {
               if(longest_axis == x)
               {
                   if(objects[i]->centroid.x < splitting_point)
                       subset_1.push_back(objects[i]);
                   else
                       subset_2.push_back(objects[i]);
               }
               else if(longest_axis == y)
               {
                   if(objects[i]->centroid.y < splitting_point)
                       subset_1.push_back(objects[i]);
                   else
                       subset_2.push_back(objects[i]);
               }
               else if(longest_axis == z)
               {
                   if(objects[i]->centroid.z < splitting_point)
                       subset_1.push_back(objects[i]);
                   else
                       subset_2.push_back(objects[i]);
               }
           }

           if(subset_1.empty() || subset_2.empty())
           {
               if(!first_pass)
               {
                   debugString("Unable to build AABB tree from all input objects. Maybe three or more objects have the same centroid?");
                   return;
               }
               subset_1.clear();
               subset_2.clear();

               splitting_point = ComputeObjectMean(longest_axis, objects);
               first_pass = false;
               goto create_subsets;
           }

           left = new AABB(subset_1);
           right = new AABB(subset_2);
        }

        bool intersect(const Ray &ray, intersectionList& list) const;

        const Vector3f getMinPoint() const;
        const Vector3f getMaxPoint() const;

    private:
        void ComputeBoundingBox(const SolidObjects& objects)
        {
            Vector3f min = objects[0]->getMinPoint();
            Vector3f max = objects[0]->getMaxPoint();

            for(unsigned int i = 1; i < objects.size(); ++i)
            {
                if(objects[i]->getMinPoint().x < min.x)
                    min.x = objects[i]->getMinPoint().x;

                if(objects[i]->getMinPoint().y < min.y)
                    min.y = objects[i]->getMinPoint().y;

                if(objects[i]->getMinPoint().z < min.z)
                    min.z = objects[i]->getMinPoint().z;

                if(objects[i]->getMaxPoint().x > max.x)
                    max.x = objects[i]->getMaxPoint().x;

                if(objects[i]->getMaxPoint().y > max.y)
                    max.y = objects[i]->getMaxPoint().y;

                if(objects[i]->getMaxPoint().z > max.z)
                    max.z = objects[i]->getMaxPoint().z;
            }

            bounds[0] = min;
            bounds[1] = max;
        }
        glm::float32 ComputeSpatialMedian(Axis& longest_axis)
        {
            Vector3f MaxMin = bounds[1] - bounds[0];
            glm::float32 longest_axis_length = glm::max(glm::max(MaxMin.x, MaxMin.y), MaxMin.z);

            if(MaxMin.x == longest_axis_length)
                longest_axis = x;
            else if(MaxMin.y == longest_axis_length)
                longest_axis = y;
            else if(MaxMin.z == longest_axis_length)
                longest_axis = z;

            if(longest_axis == x)
                return bounds[0].x + longest_axis_length / 2.0f;
            else if(longest_axis == y)
                return bounds[0].y + longest_axis_length / 2.0f;
            else
                return bounds[0].z + longest_axis_length / 2.0f;
        }
        glm::float32 ComputeObjectMean(Axis& longest_axis, const SolidObjects& objects)
        {
            Vector3f min = objects[0]->centroid;
            Vector3f max = objects[0]->centroid;

            Vector3f avg_centroid;

            for(unsigned int i = 0; i < objects.size(); ++i)
            {
                avg_centroid = avg_centroid + objects[i]->centroid;

                if(objects[i]->centroid.x < min.x)
                    min.x = objects[i]->centroid.x;

                if(objects[i]->centroid.y < min.y)
                    min.y = objects[i]->centroid.y;

                if(objects[i]->centroid.z < min.z)
                    min.z = objects[i]->centroid.z;

                if(objects[i]->centroid.x > max.x)
                    max.x = objects[i]->centroid.x;

                if(objects[i]->centroid.y > max.y)
                    max.y = objects[i]->centroid.y;

                if(objects[i]->centroid.z > max.z)
                    max.z = objects[i]->centroid.z;

            }
            avg_centroid /= (float)objects.size();

            Vector3f MaxMin = max - min;
            glm::float32 longest_axis_length = glm::max(glm::max(MaxMin.x, MaxMin.y), MaxMin.z);

            if(MaxMin.x == longest_axis_length)
                longest_axis = x;
            else if(MaxMin.y == longest_axis_length)
                longest_axis = y;
            else if(MaxMin.z == longest_axis_length)
                longest_axis = z;

            if(longest_axis == x)
                return avg_centroid.x;
            else if(longest_axis == y)
                return avg_centroid.y;
            else
                return avg_centroid.z;
        }

        Vector3f bounds[2];
        SolidObject *left = NULL;
        SolidObject *right = NULL;
    };
}

#endif // RAYTRACER_H
