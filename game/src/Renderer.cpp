#include "Renderer.h"
#include "Utils.h"
#include "Material.h"

#define AA 1;
#define AC 1;

Renderer::Renderer()
{
    Initialize();
}

Renderer::Renderer(int samples, int depth) : m_samples(samples), m_maxDepth(depth)
{
    Initialize();
}
void Renderer::Initialize() {
    m_ScreenWidth = GetScreenWidth();
    m_ScreenHeight = GetScreenHeight();
    m_AspectRatio = (float)m_ScreenWidth / (float)m_ScreenHeight;

    m_FinalImage = GenImageColor(m_ScreenWidth, m_ScreenHeight, RAYWHITE);
    m_Texture2D = LoadTextureFromImage(m_FinalImage);

    delete[] m_AccumulationData;
    m_AccumulationData = new Vector4[m_ScreenWidth * m_ScreenHeight];

    auto material_ground = make_shared<Lambertian>(Vector4{ 0.8, 0.8, 0.0, 1.0 });
    auto material_center = make_shared<Lambertian>(Vector4{ 0.1, 0.2, 0.5, 1.0 });
    auto material_left = make_shared<Metal>(Vector4{ 0.8, 0.8, 0.8, 1.0 }, 0.3);
    auto material_right = make_shared<Metal>(Vector4{ 0.8, 0.6, 0.2, 1.0 }, 0.9);

    world.add(make_shared<Sphere>(Vector3{ 0.0, -100.5, -1.0}, 100.0, material_ground));
    world.add(make_shared<Sphere>(Vector3{ 0.0, 0.0, -1.2 }, 0.5, material_center));
    world.add(make_shared<Sphere>(Vector3{ -1.0, 0.0, -1.0 }, 0.5, material_left));
    world.add(make_shared<Sphere>(Vector3{ 1.0, 0.0, -1.0 }, 0.5, material_right));

#if !AA
    m_samples = 1;
#endif
}

void Renderer::ExportRender(const char* name) const
{
    char* directory = "./Renders/";
    if (!DirectoryExists(directory)) {
        MakeDirectory(directory);
    }
    char t[256];
    strcpy(t, directory);
    strcat(t, name);
    Image renderImage = ImageCopy(m_FinalImage);
    //ImageResizeNN(&renderImage, renderImage.width * 2.0f, renderImage.height * 2.0f);
    ExportImage(renderImage, t);
    UnloadImage(renderImage);
}

void Renderer::Render(int x, int y)
{
    /*{
        int ly = (y+1) % m_ScreenHeight;
        ImageDrawLine(&m_FinalImage, 0, ly, m_ScreenWidth, ly, RED);
    }*/
    Vector4 color = CalculatePixelColor(x, y);
    ImageDrawPixel(&m_FinalImage, x, y, ColorFromNormalized(color));
    UpdateTexture(m_Texture2D, m_FinalImage.data);
    DrawTexture(m_Texture2D, 0, 0, WHITE);
}
void Renderer::UpdateRenderPass(int pass)
{
    m_FrameIndex = pass;
    if (m_FrameIndex == 1)
        memset(m_AccumulationData, 0, m_ScreenWidth * m_ScreenHeight * sizeof(Vector4));
}

Vector4 Renderer::CalculatePixelColor(int x, int y)
{
    Vector4 color = Vector4Zero();
    Vector4 sampledColor = Vector4Zero();
    for (int sample = 0; sample < m_samples; sample++) {
        sampledColor += TraceRay(x, y);
    }
    sampledColor *= 1.0f / (float)m_samples;

#if AC 
    m_AccumulationData[x + y * m_ScreenWidth] += sampledColor;
    color = m_AccumulationData[x + y * m_ScreenWidth];
    color = color / m_FrameIndex;
#else
    color = sampledColor;
#endif

    color = Vector4Clamp(color, Vector4Zero(), Vector4One());
    color.x = LinearToGamma(color.x);
    color.y = LinearToGamma(color.y);
    color.z = LinearToGamma(color.z);
    return color;
}

Vector4 Renderer::TraceRay(int x, int y)
{
#if AA
    Vector2 offset = SampleSquare();
#else
    Vector2 offset = Vector3Zeros;
#endif

    Vector2 coord = { ((float)x + offset.x) / (float)m_FinalImage.width,
        ((float)y + offset.y) / (float)m_FinalImage.height };

    coord.y = 1.0f - coord.y;   // Vertical flip to correct orientation
    coord = Vector2SubtractValue((coord * 2.0f), 1.0f); // Converting from  0 -> 1 to -1 -> 1
    coord.x *= m_AspectRatio;   //compensating for the aspect ratio

    Vector3 rayOrigin = { 0.0f, 0.0f, 0.0f };
    Vector3 rayDirection = { coord.x, coord.y, -1.0f };

    return RayColor({ rayOrigin, rayDirection }, world, m_maxDepth);
}

Vector4 Renderer::RayColor(const Ray& r, const Hittable& world, int depth)
{
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return Vector4{ 0, 0, 0, 1};
    HitRecord rec;
    if (world.Hit(r, Interval(0.001f, INFINITY), rec)) {
        Ray scattered;
        Vector4 attenuation;
        if (rec.mat->Scatter(r, rec, attenuation, scattered))
            return RayColor(scattered, world, depth - 1) * attenuation;
        return Vector4{ 0, 0, 0, 1 };
    }

    Vector3 unit_direction = Vector3Normalize(r.direction);
    auto a = 0.5f * (unit_direction.y + 1.0f);
    return  (Vector4Ones * (1.0f - a)) + (Vector4{ 0.5f, 0.7f, 1.0f, 1.0f } * a);
}

Vector2 Renderer::SampleSquare() {
    return { RandomFloat() - 0.5f, RandomFloat() - 0.5f };
}
