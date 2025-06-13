#pragma once
#include "raylib.h"
#include "Hittable.h"
#include "HittableList.h"
#include "Sphere.h"
#include "CustomCamera.h"
#include <iostream>

class Renderer {
public:
    Renderer(int samples, int depth, const CustomCamera& camera);

    void Render();
    void UpdateRenderPass(int pass);
    void ExportRender(const char* name) const;

private:
    void Initialize();
    Vector4 CalculatePixelColor(int x, int y);
    Vector4 RayColor(const Ray& r, int depth);
    Vector2 SampleSquare();

private:
    Image m_FinalImage;
    Texture2D m_Texture2D;

    HittableList world;
    const CustomCamera* m_camera = nullptr;

    int m_ScreenWidth;
    int m_ScreenHeight;
    float m_AspectRatio;

    int m_samples = 50;
    int m_maxDepth = 5;

    int m_FrameIndex = 1;
    Vector4* m_AccumulationData = nullptr;
    std::vector<uint32_t> m_ImageHorIter, m_ImageVerIter;
};