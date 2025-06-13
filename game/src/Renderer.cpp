#include "Renderer.h"
#include "Utils.h"
#include "Material.h"
#include <execution>

#define AC 1;

Renderer::Renderer(int samples, int depth, const CustomCamera& camera) :
    m_samples(samples),
    m_maxDepth(depth),
    m_camera(&camera)
{
    Initialize();
}
void Renderer::Initialize() {
    m_ScreenWidth = GetScreenWidth();
    m_ScreenHeight = GetScreenHeight();
    m_AspectRatio = (float)m_ScreenWidth / (float)m_ScreenHeight;

    m_ImageHorIter.resize(m_ScreenWidth);
    m_ImageVerIter.resize(m_ScreenHeight);
    for (uint32_t i = 0; i < m_ScreenWidth; i++)
        m_ImageHorIter[i] = i;
    for (uint32_t i = 0; i < m_ScreenHeight; i++)
        m_ImageVerIter[i] = i;

    m_FinalImage = GenImageColor(m_ScreenWidth, m_ScreenHeight, RAYWHITE);
    m_Texture2D = LoadTextureFromImage(m_FinalImage);

    delete[] m_AccumulationData;
    m_AccumulationData = new Vector4[m_ScreenWidth * m_ScreenHeight];

    auto ground_material = make_shared<Lambertian>(Vector4{ 0.5, 0.5, 0.5, 1.0 });
    world.add(make_shared<Sphere>(Vector3{ 0, -1000, 0 }, 1000, ground_material));

    auto material1 = make_shared<Dielectric>(1.5);
    world.add(make_shared<Sphere>(Vector3{ 0, 1, 0 }, 1.0, material1));

    auto material2 = make_shared<Lambertian>(Vector4{ 0.4, 0.2, 0.1, 1.0 });
    world.add(make_shared<Sphere>(Vector3{ -4, 1, 0 }, 1.0, material2));

    auto material3 = make_shared<Metal>(Vector4{ 0.7, 0.6, 0.5, 1.0 }, 0.0);
    world.add(make_shared<Sphere>(Vector3{ 4, 1, 0 }, 1.0, material3));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = RandomFloat();
            Vector3 center{ a + 0.9 * RandomFloat(), 0.2, b + 0.9 * RandomFloat() };

            if (Vector3Length((center - Vector3{ 4, 0.2, 0 })) > 0.9) {
                if (choose_mat < 0.8) {
                    // diffuse
                    Vector4 albedo = { RandomFloat(),RandomFloat(), RandomFloat(), 1.0 };
                    auto sphere_material = make_shared<Lambertian>(albedo);
                    world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // Metal
                    Vector4 albedo = { RandomFloat(0.5, 1),RandomFloat(0.5, 1), RandomFloat(0.5, 1), 1.0 };
                    auto fuzz = RandomFloat(0, 0.5);
                    auto sphere_material = make_shared<Metal>(albedo, fuzz);
                    world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    auto sphere_material = make_shared<Dielectric>(1.5);
                    world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }
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

void Renderer::Render()
{
    /*{
        int ly = (y+1) % m_ScreenHeight;
        ImageDrawLine(&m_FinalImage, 0, ly, m_ScreenWidth, ly, RED);
    }*/
    std::for_each(std::execution::par, m_ImageVerIter.begin(), m_ImageVerIter.end(), [this](uint32_t y)
        {
            std::for_each(std::execution::par, m_ImageHorIter.begin(), m_ImageHorIter.end(), [this, y](uint32_t x)
                {
                    Vector4 color = CalculatePixelColor(x, y);
                    ImageDrawPixel(&m_FinalImage, x, y, ColorFromNormalized(color));
                });
        });
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
        sampledColor += RayColor(m_camera->GetRay(x, y),  m_maxDepth);
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

Vector4 Renderer::RayColor(const Ray& r, int depth)
{
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return Vector4{ 0, 0, 0, 1};
    HitRecord rec;
    if (world.Hit(r, Interval(0.001f, INFINITY), rec)) {
        Ray scattered;
        Vector4 attenuation;
        if (rec.mat->Scatter(r, rec, attenuation, scattered))
            return RayColor(scattered, depth - 1) * attenuation;
        return Vector4{ 0, 0, 0, 1 };
    }

    Vector3 unit_direction = Vector3Normalize(r.direction);
    auto a = 0.5f * (unit_direction.y + 1.0f);
    return  (Vector4Ones * (1.0f - a)) + (Vector4{ 0.5f, 0.7f, 1.0f, 1.0f } * a);
}

Vector2 Renderer::SampleSquare() {
    return { RandomFloat() - 0.5f, RandomFloat() - 0.5f };
}
