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