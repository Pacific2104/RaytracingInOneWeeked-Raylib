#pragma once
#include "Hittable.h"
#include "Utils.h"

class Mat {
public:
    virtual ~Mat() = default;

    virtual bool Scatter( const Ray& r_in, const HitRecord& rec, Vector4& attenuation, Ray& scattered) const {
        return false;
    }
};

class Lambertian : public Mat {
public:
    Lambertian(const Vector4& albedo) : albedo(albedo) {}

    bool Scatter(const Ray& r_in, const HitRecord& rec, Vector4& attenuation, Ray& scattered) const override {
        auto scatter_direction = rec.normal + RandomVector3();
        // Catch degenerate scatter direction
        if (NearZero(scatter_direction))
            scatter_direction = rec.normal;
        scattered = Ray{ rec.p, scatter_direction };
        attenuation = albedo;
        return true;
    }

private:
    Vector4 albedo;
};

class Metal : public Mat {
public:
    Metal(const Vector4& albedo, float fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool Scatter(const Ray& r_in, const HitRecord& rec, Vector4& attenuation, Ray& scattered)
        const override {
        Vector3 reflected = Vector3Reflect(r_in.direction, rec.normal);
        reflected = Vector3Normalize(reflected) + (RandomVector3() * fuzz);
        scattered = Ray{ rec.p, reflected };
        attenuation = albedo;
        return true;
    }

private:
    Vector4 albedo;
    float fuzz;
};

class Dielectric : public Mat {
public:
    Dielectric(float refractionIndex) : refractionIndex(refractionIndex) {}

    bool Scatter(const Ray& r_in, const HitRecord& rec, Vector4& attenuation, Ray& scattered) const override {
        attenuation = Vector4{ 1.0, 1.0, 1.0, 1.0 };
        float ri = rec.front_face ? (1.0 / refractionIndex) : refractionIndex;

        Vector3 unitDirection = Vector3Normalize(r_in.direction);
        float cosTheta = std::fmin(Vector3DotProduct(Vector3Negate(unitDirection), rec.normal), 1.0);
        float sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);

        bool cannot_refract = ri * sinTheta > 1.0;
        Vector3 direction;

        if (cannot_refract || reflectance(cosTheta, ri) > RandomFloat())
            direction = Vector3Reflect(unitDirection, rec.normal);
        else
            direction = Vector3Refract(unitDirection, rec.normal, ri);

        scattered = Ray{ rec.p, direction };
        return true;
    }

private:
    // Refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    float refractionIndex;

    static float reflectance(float cosine, float refraction_index) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow((1 - cosine), 5);
    }
};