#pragma once
#include "raylib.h"
#include "raymath.h"
#include "Utils.h"

class CustomCamera
{
public:
    double vfov = 20;    // Vertical view angle (field of view)
    Vector3 lookFrom = { 13, 2, 3 };   // Point camera is looking from
    Vector3 lookAt = { 0, 0, 0 };   // Point camera is looking at
    Vector3 vUp = {0, 1, 0};   // Camera-relative "up" direction

    double defocus_angle = 0.6;  // Variation angle of rays through each pixel
    double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus

public:
    CustomCamera() {
        image_width = GetScreenWidth();
        image_height = GetScreenHeight();
        aspect_ratio = (float)image_width / (float)image_height;

        center = lookFrom;

        // Determine viewport dimensions.
        auto theta = vfov * DEG2RAD;
        auto h = std::tan(theta / 2); 
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (double(image_width) / image_height);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = Vector3Normalize(lookFrom - lookAt);
        u = Vector3Normalize(Vector3CrossProduct(vUp, w));
        v = Vector3CrossProduct(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        Vector3 viewport_u = u * viewport_width;    // Vector across viewport horizontal edge
        Vector3 viewport_v = v * -viewport_height;  // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - (w * focus_dist) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + (pixel_delta_u + pixel_delta_v) * 0.5f;

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * std::tan((defocus_angle / 2) * DEG2RAD);
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    Ray GetRay(int x, int y) const {
        Vector2 offset = SampleSquare();
        Vector3 pixel_sample = pixel00_loc
            + (pixel_delta_u * (x + offset.x))
            + (pixel_delta_v * (y + offset.y));

        auto ray_origin = (defocus_angle <= 0) ? center : DefocusDiskSample();
        Vector3 ray_direction = pixel_sample - ray_origin;
        return { ray_origin, ray_direction };
    }

private:
    double aspect_ratio = 1.0;  // Ratio of image width over height
    int image_height = 100;     // Rendered image height
    int image_width = 100;      // Rendered image width in pixel count

    Vector3 center;               // Camera center
    Vector3 pixel00_loc;          // Location of pixel 0, 0
    Vector3 pixel_delta_u;        // Offset to pixel to the right
    Vector3 pixel_delta_v;        // Offset to pixel below
    Vector3 u, v, w;              // Camera frame basis vectors
    Vector3 defocus_disk_u;       // Defocus disk horizontal radius
    Vector3 defocus_disk_v;       // Defocus disk vertical radius

private:
    Vector2 SampleSquare() const{
        return { RandomFloat() - 0.5f, RandomFloat() - 0.5f };
    }
    Vector3 DefocusDiskSample() const {
        // Returns a random point in the camera defocus disk.
        auto p = RandomInUnitDisk();
        return center + (defocus_disk_u * p.x) + (defocus_disk_v * p.y);
    }

};
