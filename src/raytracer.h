#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <vector>
#include <stdio.h>
#include <iostream>
#include <memory>
#include <algorithm>

//promijeniti pristup
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
    inline bool isPointInRectangle(const Vector3f &m, const Vector3f &a, const Vector3f &b, const Vector3f &d)
    {
        const float am_ab = glm::dot(a*m, a*b);
        const float ab_ab = glm::dot(a*b, a*b);

        const float am_ad = glm::dot(a*m, a*d);
        const float ad_ad = glm::dot(a*d, a*d);

        if((0.0f <= am_ab && am_ab <= ab_ab) && (0.0f <= am_ad && am_ad <= ad_ad))
            return true;

        return false;
    }
    inline bool isEqual(const glm::float32 &x, const glm::float32 &y)
    {
      const glm::float32 epsilon = 0.25f;
      return glm::abs(x - y) <= epsilon * glm::abs(x);
    }
    inline bool isPointBetweenTwoPoints(const Vector3f &a, const Vector3f &b, const Vector3f &c)
    {
        if(isEqual(glm::length(c - a) + glm::length(b - c), glm::length(b - a)))
        {
            return true;
        }

        return false;
    }

    inline void debugVec3f(const Vector3f &vec)
    {
        std::cout << vec.x << " " << vec.y << " " << vec.z << std::endl;
    }
    inline void debugFloat (const float &fl)
    {
        std::cout << fl << std::endl;
    }
    inline void debugString (const std::string &str)
    {
        std::cout << str << std::endl;
    }

    class Ray
    {
    public:
        Ray() : start(Vector3f()), dir(Vector3f())
        {
            inv_dir = 1.0f / dir;
            sign[0] = (inv_dir.x < 0);
            sign[1] = (inv_dir.y < 0);
            sign[2] = (inv_dir.z < 0);
        }
        Ray(const Vector3f &st, const Vector3f &d) : start(st), dir(d)
        {
            inv_dir = 1.0f / dir;
            sign[0] = (inv_dir.x < 0);
            sign[1] = (inv_dir.y < 0);
            sign[2] = (inv_dir.z < 0);
        }
        Ray(const Vector3f &st, const Vector3f &d, const bool is_light) : start(st), dir(d), isLightRay(is_light)
        {
            inv_dir = 1.0f / dir;
            sign[0] = (inv_dir.x < 0);
            sign[1] = (inv_dir.y < 0);
            sign[2] = (inv_dir.z < 0);
        }
        Vector3f start, dir, inv_dir;
        int sign[3];
        bool isLightRay = false;
    };
    class Material
    {
    public:
        const Color& diffuse() const {return _diffuse; }
        const float& reflection() const {return _reflection; }

        void setDiffuse(const Color diffuse) { _diffuse = diffuse; }
        void setReflection(const float reflection) { _reflection = reflection; }

        Material(const Color &d, const float &refl) : _diffuse(d), _reflection(refl)  {}
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

        DisplacementMap(const Texture& d_map, const float& du_) : displacement_map(d_map), du(du_), max_displacement(calculate_max_displacement()) {}
    protected:
        Texture displacement_map;
        const float du;
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

        SphereDisplacementMap(const Texture& d_map, const float& du_) : DisplacementMap(d_map, du_) {}
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
        PointLight(const Vector3f &p, const Color &col, const glm::float32 &intens) : pos(p) , lightColor(col), intensity(intens){}
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
            const float r2 = glm::pow(glm::length(getDirection(surfacePoint)), 1.0f);
            return (lightColor * intensity) / (4.0f * glm::pi<float>() * r2);
        }
        RealisticPointLight(const Vector3f &p, const Color &col, const glm::float32 &intens) : PointLight(p, col, intens){}
    };

    class SolidObject
    {
    public:
         SolidObject(const Vector3f &pos, const Material &mat) : position(pos), material(mat) {}
         virtual bool intersect(const Ray &ray, intersectionList& list) const = 0;
         virtual Material surfaceMaterial(const Vector3f& /*surfacePoint*/) const { return material; }

         virtual const Vector3f getMinPoint() const = 0;
         virtual const Vector3f getMaxPoint() const = 0;

         Vector3f position;
    protected:
         Material material;
    };
    struct less_so_x
    {
        bool operator()(const SolidObject* a, const SolidObject* b) const { return a->position.x < b->position.x; }
    };
    struct less_so_y
    {
        bool operator()(const SolidObject* a, const SolidObject* b) const { return a->position.y < b->position.y; }
    };
    struct less_so_z
    {
        bool operator()(const SolidObject* a, const SolidObject* b) const { return a->position.z < b->position.z; }
    };


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
        Triangle(const Vector3f &_v0, const Vector3f &_v1, const Vector3f &_v2, const Material &_mat) : SolidObject(Vector3f(0.0f,0.0f,0.0f), _mat), v{_v0, _v1, _v2}
        {
            position = (1.0f / 3.0f) * (v[0] + v[1] + v[2]);
        }
        bool intersect(const Ray &ray, intersectionList& list) const;

        const Vector3f getMinPoint() const;
        const Vector3f getMaxPoint() const;
    private:
        const Vector3f v[3];
    };

    class AABB : public SolidObject
    {
        typedef std::unique_ptr<SolidObject> ptr;
        typedef std::vector<SolidObject*> SolidObjects;

    public:
        AABB(const Vector3f &_min, const Vector3f &_max) : SolidObject(Vector3f(0.0f,0.0f,0.0f), Material(Color(), 0.0f)), bounds { _min, _max } {}
        AABB(SolidObjects& objects, int bla) : SolidObject(Vector3f(0.0f,0.0f,0.0f), Material(Color(), 0.0f))
        {
            bla--;
            const int MIN_OBJECTS_PER_LEAF = 2;

            if(objects.size() <= 0)
                return;
            else if(objects.size() == MIN_OBJECTS_PER_LEAF)
            {
                left = objects[0];
                right = objects[1];
                return;
            }

            Vector3f min = objects[0]->getMinPoint();
            Vector3f max = objects[0]->getMaxPoint();

            for(unsigned i = 1; i < objects.size(); ++i)
            {
                if(objects[i]->getMinPoint() == Vector3f(-INFINITY) || objects[i]->getMaxPoint() == Vector3f(INFINITY))
                    continue;

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

            Vector3f axis_select = max - min;
            glm::float32 max_axis = glm::max(glm::max(axis_select.x, axis_select.y), axis_select.z);

            if(max.x - min.x == max_axis)
            {
                std::sort(objects.begin(), objects.end(), less_so_x{});
                if(bla >= 0) debugString("X");
            }
            else if(max.y - min.y == max_axis)
            {
                std::sort(objects.begin(), objects.end(), less_so_y{});
                if(bla >= 0) debugString("Y");
            }
            else if(max.z - min.z == max_axis)
            {
                std::sort(objects.begin(), objects.end(), less_so_z{});
                if(bla >= 0) debugString("Z");
            }

            if(bla >= 0)
            {
                for(unsigned i = 0; i < objects.size(); ++i)
                {
                    debugVec3f(objects[i]->position);
                }
                debugString(" ");
            }

            for(unsigned i = 0; i < objects.size(); ++i)
            {
                if((max.x - min.x == max_axis && objects[i]->position.x > min.x + max_axis / 2.0f) ||
                   (max.y - min.y == max_axis && objects[i]->position.y > min.y + max_axis / 2.0f) ||
                   (max.z - min.z == max_axis && objects[i]->position.z > min.z + max_axis / 2.0f))
                {
                    /*if(bla >= 0)
                    {
                        debugVec3f(axis_select);
                        debugVec3f(min);
                        debugVec3f(max);
                        debugFloat(i);
                        debugString(" ");
                    }*/

                    SolidObjects::const_iterator first = objects.begin();
                    SolidObjects::const_iterator mid = objects.begin() + i;
                    SolidObjects::const_iterator last = objects.end();

                    SolidObjects first_subset(first, mid);
                    SolidObjects second_subset(mid, last);

                    left = new AABB(first_subset, bla-1);
                    right = new AABB(second_subset, bla-1);

                    return;
                }
            }
        }

        bool intersect(const Ray &ray, intersectionList& list) const;

        const Vector3f getMinPoint() const;
        const Vector3f getMaxPoint() const;

        int hits = 0;
    private:
        Vector3f bounds[2];
        SolidObject *left;
        SolidObject *right;
        void inchits()
        {
            hits++;
        }
    };

    /*
    class AABBNode
    {
        typedef std::unique_ptr<AABBNode> ptr;
        typedef std::vector<SolidObject*> SolidObjects;
    public:
        AABBNode(SolidObject &o) : obj(&o) {}
        AABBNode(SolidObject &o, ptr &l, ptr &r) : obj(&o)
        {
            left = std::move(l);
            right = std::move(r);
        }

        bool is_leaf()
        {
            return isLeaf;
        }

        void set_leaf(bool s)
        {
            isLeaf = s;
        }

        void buildTopDownTree(const SolidObjects& objects, const int numObjects)
        {
            if(objects.size() == 0)
                return;

            const unsigned MIN_OBJECTS_PER_LEAF = 2;

            obj = new AABB(objects, numObjects);

            if(numObjects <= MIN_OBJECTS_PER_LEAF)
            {
                isLeaf = false;
                //obj =
            }
        }

    private:
        SolidObject *obj = NULL;
        ptr left;
        ptr right;

        bool isLeaf = false;
        int numObjects = 0;
    };
    */
}

#endif // RAYTRACER_H
