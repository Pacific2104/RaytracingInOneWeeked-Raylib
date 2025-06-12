#pragma once
#include "raylib.h"
#include <iostream>
#include "Hittable.h"
#include "HittableList.h"
#include "Sphere.h"

class Renderer {
public:
    Renderer();

    void Render(int x, int y);
    void UpdateRenderPass(int pass);
    void ExportRender(const char* name) const;

private:
    Vector4 CalculatePixelColor(int x, int y);
    Vector4 TraceRay(int x, int y);
    float HitSphere(const Vector3& center, float radius, const Ray& r);
    Vector3 RayColor(const Ray& r, const Hittable& world, int depth);
    Vector3 SampleSquare();

private:
    Image m_FinalImage;
    Texture2D m_Texture2D;

    HittableList world;

    int m_ScreenWidth;
    int m_ScreenHeight;
    float m_AspectRatio;

    int samples = 50;
    int maxDepth = 5;

    int m_FrameIndex = 1;
    Vector4* m_AccumulationData = nullptr;
};