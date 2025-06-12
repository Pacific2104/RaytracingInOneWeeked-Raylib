#pragma once
#include "raylib.h"
#include <iostream>
#include "Hittable.h"
#include "HittableList.h"
#include "Sphere.h"

class Renderer {
public:
    Renderer(int threads);

    void ExportRender(const char* name) const;

    void OnResize();
    void Render();

private:
    void UpdateTextureBuffer();
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

    int samples = 2;
    int maxDepth = 10;
    int threadCount = 1;
    int m_FrameIndex = 1;
    Vector4* m_AccumulationData = nullptr;
    std::vector<uint32_t> m_ImageHorIter, m_ImageVerIter;
};