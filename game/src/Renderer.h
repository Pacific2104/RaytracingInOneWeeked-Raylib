#pragma once
#include "raylib.h"
#include <iostream>
#include "Hittable.h"
#include "HittableList.h"
#include "Sphere.h"

class Renderer {
public:
    Renderer(int samples, int threads);

    void ExportRender(const char* name) const;

    void OnResize();
    void Render() const;

private:
    Vector4 TraceRay(int x, int y);
    float HitSphere(const Vector3& center, float radius, const Ray& r);
    Vector3 RayColor(const Ray& r, const Hittable& world);
    Vector3 SampleSquare();

private:
    Image m_FinalImage;
    Texture2D m_Texture2D;

    HittableList world;

    int m_ScreenWidth;
    int m_ScreenHeight;
    float m_AspectRatio;

    int samples = 10;
    int threadCount = 2;
};