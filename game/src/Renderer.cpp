#include "Renderer.h"
#include "raymath.h"
#include <execution>

#define AA 1;
#define AC 1;

namespace Utils {

    static Vector4 Vector4Clamp(const Vector4& vector, Vector4 min, Vector4 max) {
        Vector4 result = { 0.0f };

        result.x = fminf(max.x, fmaxf(min.x, vector.x));
        result.y = fminf(max.y, fmaxf(min.y, vector.y));
        result.z = fminf(max.z, fmaxf(min.z, vector.z));
        result.w = fminf(max.w, fmaxf(min.w, vector.w));

        return result;
    }
    static uint32_t PCG_Hash(uint32_t input) {
        uint32_t state = input * 747796405u + 2891336453u;
        uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 27703737u;
        return (word >> 22u) ^ word;
    }
    static float RandomFloat() {
        // Returns a random real in (0,1).
        return std::rand() / (RAND_MAX + 1.0);
    }
    static float RandomFloat(float min, float max) {
        // Returns a random real in (min,max).
        return min + (max - min) * RandomFloat();
    }
    static Vector3 RandomVector3() {
        return Vector3{ RandomFloat(), RandomFloat(), RandomFloat() };
    }
    static Vector3 RandomVector3(float min, float max) {
        return Vector3{ RandomFloat(min, max), RandomFloat(min, max), RandomFloat(min, max) };
    }
    static Vector3 InUnitSphere(){
        return Vector3Normalize(RandomVector3(-1, 1));
    }
    static float LinearToGamma(float linearComponent)
    {
        if (linearComponent > 0)
            return std::sqrt(linearComponent);
        return 0;
    }
}

Renderer::Renderer()
{
    m_ScreenWidth = GetScreenWidth();
    m_ScreenHeight = GetScreenHeight();
    m_AspectRatio = (float)m_ScreenWidth / (float)m_ScreenHeight;

    m_FinalImage = GenImageColor(m_ScreenWidth, m_ScreenHeight, RAYWHITE);
    m_Texture2D = LoadTextureFromImage(m_FinalImage);

    delete[] m_AccumulationData;
    m_AccumulationData = new Vector4[m_ScreenWidth * m_ScreenHeight];

    world.add(make_shared<Sphere>(Vector3{ 0, 0, -1 }, 0.5));
    world.add(make_shared<Sphere>(Vector3{ 0, -100.5, -1 }, 100));
}

Renderer::Renderer(int samples, int depth) : m_samples(samples), m_maxDepth(depth)
{
    m_ScreenWidth = GetScreenWidth();
    m_ScreenHeight = GetScreenHeight();
    m_AspectRatio = (float)m_ScreenWidth / (float)m_ScreenHeight;

    m_FinalImage = GenImageColor(m_ScreenWidth, m_ScreenHeight, RAYWHITE);
    m_Texture2D = LoadTextureFromImage(m_FinalImage);

    delete[] m_AccumulationData;
    m_AccumulationData = new Vector4[m_ScreenWidth * m_ScreenHeight];

    world.add(make_shared<Sphere>(Vector3{ 0, 0, -1 }, 0.5));
    world.add(make_shared<Sphere>(Vector3{ 0, -100.5, -1 }, 100));
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

    color = Utils::Vector4Clamp(color, Vector4Zero(), Vector4One());
    color.x = Utils::LinearToGamma(color.x);
    color.y = Utils::LinearToGamma(color.y);
    color.z = Utils::LinearToGamma(color.z);
    return color;
}

Vector4 Renderer::TraceRay(int x, int y)
{
#if AA
    Vector3 offset = SampleSquare();
#else
    Vector3 offset = Vector3Zeros;
#endif

    Vector2 coord = { ((float)x + offset.x) / (float)m_FinalImage.width, 
        ((float)y + offset.y) / (float)m_FinalImage.height };
    coord = Vector2SubtractValue((coord * 2.0f), 1.0f);  // Converting from  0 -> 1 to -1 -> 1
    coord.x *= m_AspectRatio; //compensating for the aspect ratio

    Vector3 rayOrigin = { 0.0f, 0.0f, 0.0f };
    Vector3 rayDirection = { coord.x, coord.y, -1.0f };

    Vector3 color = RayColor({ rayOrigin, rayDirection }, world, m_maxDepth);
    return Vector4{ color.x, color.y,color.z, 1.0f };
}

Vector3 Renderer::RayColor(const Ray& r, const Hittable& world, int depth)
{
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return Vector3{ 0, 0, 0 };
    HitRecord rec;
    if (world.Hit(r, Interval(0.001f, INFINITY), rec)) {
        Vector3 direction = rec.normal + Utils::InUnitSphere();
        return RayColor(Ray{ rec.p, direction }, world, depth-1) * 0.5f;
    }

    Vector3 unit_direction = Vector3Normalize(r.direction);
    auto a = 0.5f * (unit_direction.y + 1.0f);
    return  (Vector3Ones * (1.0f - a)) + (Vector3{ 0.5f, 0.7f, 1.0f } * a);
}

Vector3 Renderer::SampleSquare() {
    return { Utils::RandomFloat() - 0.5f,Utils::RandomFloat() - 0.5f ,0 };
}
