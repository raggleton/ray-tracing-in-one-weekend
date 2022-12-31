#include <iostream>

#include "rtWeekend.h"
#include "hittable_list.h"
#include "sphere.h"
#include "color.h"
#include "camera.h"


double hit_sphere(const point3 &center, double radius, const ray &r) {
    // determine t when ray `r` intersects sphere centeres at `center` with radius `radius`
    vec3 oc = r.origin() - center;
    auto a = dot(r.direction(), r.direction());
    auto b = 2.0 * dot(oc, r.direction());
    auto c = dot(oc, oc) - radius * radius;
    auto discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return -1.0;
    } else {
        return (-b - sqrt(discriminant)) / (2.0 * a);
    }
}

color ray_color(const ray& r, const hittable& world, int depth, bool uniform_scatter) {
    // if intersects any hittable object in our `world`, set color using unit vector
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0, 0, 0);

    if (world.hit(r, 0.0001, infinity, rec)) {
        if (uniform_scatter) {
            // alternate diffuse: don't displace from hit point - 
            // just pick a scatter direction from all angles uniformly,
            // no dependence on angle wrt normal
            point3 target = rec.p + random_in_hemisphere(rec.normal);
            return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth-1, true);
        } else {
            // diffuse: modulate target by a random amount within a unit sphere,
            // do recursively
            point3 target = rec.p + rec.normal + random_unit_vector();
            return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth-1, false);
        }
    }
    // otherwise, color using background gradient
    vec3 unit_direction = unit_vector(r.direction());
    // t is a parameter that goes from 0 to 1 as y goes from -1 to 1
    // use t to create a gradient from blue (t=1) to white (t=0)
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}


int main() {

    // Image
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;
    const bool uniform_scatter = true;

    // World
    hittable_list world;
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5)); // normal sphere
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100)); // "ground"

    // Camera
    camera cam;

    // Render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; --j) {
        // use cerr as cout for image data
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0, 0, 0);
            // anti-aliasing:
            // for each pixel, create multiple rays that fall within that pixel
            // then we color the pixel with the average of those rays
            for (int s = 0; s < samples_per_pixel; s++) {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth, uniform_scatter);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }
    std::cerr << "\nDone\n";
}