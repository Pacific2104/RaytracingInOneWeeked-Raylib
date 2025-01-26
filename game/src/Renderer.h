#pragma once
#include "raylib.h"
#include <iostream>
#include "Hittable.h"
#include "HittableList.h"
#include "Sphere.h"

class Renderer {
public:
    Renderer();

    void OnResize();
    void Render() const;

private:
    Vector4 TraceRay(Vector2 coord);
    float HitSphere(const Vector3& center, float radius, const Ray& r);
    Vector3 RayColor(const Ray& r, const Hittable& world);

private:
    Image m_FinalImage;
    Texture2D m_Texture2D;

    HittableList world;

    int m_ScreenWidth;
    int m_ScreenHeight;
};