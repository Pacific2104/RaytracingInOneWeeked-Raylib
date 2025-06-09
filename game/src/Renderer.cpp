#include "Renderer.h"
#include "raymath.h"
#include <thread>

#define AA 1;
#define MT 1;

namespace Utils {

    static Vector4 Vector4Clamp(const Vector4& vector, Vector4 min, Vector4 max) {
        Vector4 result = { 0.0f };

        result.x = fminf(max.x, fmaxf(min.x, vector.x));
        result.y = fminf(max.y, fmaxf(min.y, vector.y));
        result.z = fminf(max.z, fmaxf(min.z, vector.z));
        result.w = fminf(max.w, fmaxf(min.w, vector.w));

        return result;
    }
    static float RandomFloat() {
        // Returns a random real in [0,1).
        return std::rand() / (RAND_MAX + 1.0);
    }
    static float RandomFloat(float min, float max) {
        // Returns a random real in [min,max).
        return min + (max - min) * RandomFloat();
    }
}

Renderer::Renderer(int samples, int threads) :
    samples(samples),
    threadCount(threads)
{
    m_ScreenWidth = GetScreenWidth();
    m_ScreenHeight = GetScreenHeight();
    m_AspectRatio = (float)m_ScreenWidth / (float)m_ScreenHeight;

    m_FinalImage = GenImageColor(m_ScreenWidth, m_ScreenHeight, RAYWHITE);
    m_Texture2D = LoadTextureFromImage(m_FinalImage);

    world.add(make_shared<Sphere>(Vector3{ 0, 0, -1 }, 0.5));
    world.add(make_shared<Sphere>(Vector3{ 0, -100.5, -1 }, 100));

    OnResize();
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

void Renderer::OnResize()
{
    m_ScreenWidth = GetScreenWidth();
    m_ScreenHeight = GetScreenHeight();
    m_AspectRatio = (float)m_ScreenWidth / (float)m_ScreenHeight;

    UnloadImage(m_FinalImage);
    m_FinalImage = GenImageColor(m_ScreenWidth, m_ScreenHeight, RAYWHITE);

    UnloadTexture(m_Texture2D);
    m_Texture2D = LoadTextureFromImage(m_FinalImage);

#if !AA
    samples = 1;
#endif

#if MT
    std::vector<std::thread> threads;
    for (int t = 0; t < threadCount; ++t) {
        threads.emplace_back([=]() {
            for (uint32_t y = t; y < m_ScreenWidth; y += threadCount) {
                for (uint32_t x = 0; x < m_ScreenHeight; ++x) {
                    Vector4 color = Vector4Zero();
                    for (int sample = 0; sample < samples; sample++)
                        color += TraceRay(x, y);

                    color *= 1.0f / (float)samples;
                    color = Utils::Vector4Clamp(color, Vector4Zero(), Vector4One());
                    ImageDrawPixel(&m_FinalImage, x, y, ColorFromNormalized(color));
                }
            }
        });
    }
    for (auto& thread : threads)
        thread.join();
#else
    for (int y = 0; y < m_ScreenHeight; y++)
    {
        for (int x = 0; x < m_ScreenWidth; x++)
        {
            Vector4 color = Vector4Zero();
            for (int sample = 0; sample < samples; sample++)
            {
                color += TraceRay(x, y);
            }
            color *= 1.0f / (float)samples;
            color = Utils::Vector4Clamp(color, Vector4Zero(), Vector4One());
            ImageDrawPixel(&m_FinalImage, x, y, ColorFromNormalized(color));
            //ImageDrawPixel(&m_FinalImage, x, y, GetColor(RayTracing::Random::UInt(0x000000ff, 0xffffffff)));
        }
    }
#endif
    ImageFlipVertical(&m_FinalImage);
    UpdateTexture(m_Texture2D, m_FinalImage.data);
}

void Renderer::Render() const
{
    DrawTexture(m_Texture2D, 0, 0, WHITE);
}

Vector4 Renderer::TraceRay(int x, int y)
{
#if AA
    Vector3 offset = SampleSquare();
#else
    Vector3 offset = Vector3Zero();
#endif

    Vector2 coord = { ((float)x + offset.x) / (float)m_FinalImage.width, ((float)y + offset.y) / (float)m_FinalImage.height };
    coord = Vector2SubtractValue((coord * 2.0f), 1.0f);  // Converting from  0 -> 1 to -1 -> 1
    coord.x *= m_AspectRatio; //compensating for the aspect ratio

    Vector3 rayOrigin = { 0.0f, 0.0f, 0.0f };
    Vector3 rayDirection = { coord.x, coord.y, -1.0f };

    Vector3 color = RayColor({ rayOrigin, rayDirection }, world);
    return Vector4{ color.x, color.y,color.z, 1.0f };
}

Vector3 Renderer::SampleSquare() {
    return { Utils::RandomFloat() - 0.5f,Utils::RandomFloat() - 0.5f ,0 };
}

float Renderer::HitSphere(const Vector3& center, float radius, const Ray& r)
{
    Vector3 oc = center - r.position;
    auto a = Vector3LengthSqr(r.direction);
    auto h = Vector3DotProduct(r.direction, oc);
    auto c = Vector3LengthSqr(oc) - (radius * radius);
    auto discriminant = h * h - a * c;
    if (discriminant < 0) {
        return -1.0;
    }
    else {
        return (h - std::sqrt(discriminant)) / a;
    }
}

Vector3 Renderer::RayColor(const Ray& r, const Hittable& world)
{
    /*auto t = HitSphere(Vector3{ 0.0f, 0.0f, 0.0f }, 0.5f, r);
    if (t > 0.0) {
        Vector3 N = Vector3Normalize(Utils::RayHitPos(r, t));
        return Vector3AddValue(N, 1) * 0.5f;
    }*/
    HitRecord rec;
    if (world.Hit(r, Interval(0, INFINITY), rec)) {
        return (rec.normal + Vector3{ 1, 1, 1 }) * 0.5f;
    }

    Vector3 unit_direction = Vector3Normalize(r.direction);
    auto a = 0.5f * (unit_direction.y + 1.0f);
    return  Vector3Scale(Vector3{ 1.0f, 1.0f, 1.0f }, (1.0f - a)) + Vector3Scale(Vector3{ 0.5f, 0.7f, 1.0f }, a);
}