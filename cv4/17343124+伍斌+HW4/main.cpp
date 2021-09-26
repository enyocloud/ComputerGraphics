/*The MIT License (MIT)

Copyright (c) 2021-Present, Wencong Yang (yangwc3@mail2.sysu.edu.cn).

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.*/

#include <array>
#include <vector>
#include <thread>
#include <iostream>
#include<float.h>

#include "WindowsApp.h"
#include "vec3.h"
#include "ray.h"
#include "sphere.h"
#include "hitable.h"
#include "hitablelist.h"
#include "camera.h"
#include "material.h"
#include"aabb.h"
#include"bvh_node.h"

typedef std::array<float, 3> Pixel;					//RGB pixel
static std::vector<std::vector<Pixel>> gCanvas;		//Canvas

// The width and height of the screen
constexpr int gHeight = 400;
constexpr int gWidth = 800;

void rendering();

int main(int argc, char* args[])
{
	// Create window app handle
	WindowsApp::ptr winApp = WindowsApp::getInstance(gWidth, gHeight, "CGAssignment4: Ray Tracing");
	if (winApp == nullptr)
	{
		std::cerr << "Error: failed to create a window handler" << std::endl;
		return -1;
	}

	// Memory allocation for canvas
	gCanvas.resize(gHeight, std::vector<Pixel>(gWidth));

	// Launch the rendering thread
	// Note: we run the rendering task in another thread to avoid GUI blocking
	std::thread renderingThread(rendering);

	// Window app loop
	while (!winApp->shouldWindowClose())
	{
		// Process event
		winApp->processEvent();

		// Display to the screen
		winApp->updateScreenSurface(gCanvas);

	}

	renderingThread.join();

	return 0;
}

void writeRGBToCanvas(const float& r, const float& g, const float& b, int x, int y)
{
	// Out-of-range detection
	if (x < 0 || x >= gWidth)
	{
		std::cerr << "Warnning: try to write the pixel out of range: (x,y) -> (" << x << "," << y << ")" << std::endl;
		return;
	}

	if (y < 0 || y >= gHeight)
	{
		std::cerr << "Warnning: try to write the pixel out of range: (x,y) -> (" << x << "," << y << ")" << std::endl;
		return;
	}

	// Note: x -> the column number, y -> the row number
	gCanvas[y][x] = Pixel({ r,g,b });

}

//float hit_sphere(const vec3& center, float radius, const ray& r) {
//	vec3 oc = r.origin() - center;
//	float a = dot(r.direction(), r.direction());
//	float b = 2.0 * dot(oc, r.direction());
//	float c = dot(oc, oc) - radius * radius;
//	float discriminant = b * b - 4 * a * c;
//	if (discriminant < 0) {
//		return -1.0;
//	}
//	else {
//		return (-b - sqrt(discriminant)) / (2.0 * a);
//	}
//}


//vec3 color(const ray& r) {
//	float t = hit_sphere(vec3(0, 0, -1), 0.5, r);
//	if (t > 0.0) {
//		vec3 N = unit_vector(r.point_at_parameter(t) - vec3(0, 0, -1));
//		return 0.5 * vec3(N.x() + 1.0, N.y() + 1.0, N.z() + 1.0);
//	}
//	vec3 unit_direction = unit_vector(r.direction());
//	t = 0.5 * (unit_direction.y() + 1.0);
//	return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
//}



vec3 color(const ray& r, hitable* world,int depth) {
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec)) {
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, world, depth + 1);
		}
		else {
			return vec3(0, 0, 0);
		}
	}
	else {
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}
}

//vec3 color(const ray& r, hitable* world) {
//	hit_record rec;
//	if (world->hit(r, 0.001, FLT_MAX, rec)) {
//		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
//		return 0.5 * color(ray(rec.p, target - rec.p), world);
//	}
//	else {
//		vec3 unit_direction = unit_vector(r.direction());
//		float t = 0.5 * (unit_direction.y() + 1.0);
//		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
//	}
//}

//vec3 color(const ray& r, hitable* world) {
//	hit_record rec;
//	if (world->hit(r, 0.0, FLT_MAX, rec)) {
//		return 0.5 * vec3(rec.normal.x() + 1, rec.normal.y() + 1, rec.normal.z() + 1);
//	}
//	else {
//		vec3 unit_direction = unit_vector(r.direction());
//		float t = 0.5 * (unit_direction.y() + 1.0);
//		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
//	}
//}

//void rendering()
//{
//
//	printf("CGAssignment4 (built %s at %s) \n", __DATE__, __TIME__);
//	std::cout << "Ray-tracing based rendering launched..." << std::endl;
//	int nx = gWidth;
//	int ny = gHeight;
//
//	double startFrame = clock();
//
//	//vec3 lower_left_corner(-2.0, -1.0, -1.0);
//	//vec3 horizontal(4.0, 0.0, 0.0);
//	//vec3 vertical(0.0, 2.0, 0.0);
//	//vec3 origin(0.0, 0.0, 0.0);
//
//	hitable* list[5];
//	list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));
//	list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
//	list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2),0.3));
//	list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));
//	list[4] = new sphere(vec3(-1, 0, -1), -0.45, new dielectric(1.5));
//	
//	hitable* world = new hitable_list(list, 5);
//
//	vec3 lookfrom(3, 3, 2);
//	vec3 lookat(0, 0, -1);
//	float disk_to_focus = (lookfrom - lookat).length();
//	float aperture = 2.0;
//
//	camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny),aperture,disk_to_focus);
//
//	//camera cam(vec3(-2, 2, 1), vec3(0, 0, -1), vec3(0, 1, 0), 90, float(nx) / float(ny));
//
//	//float R = cos(M_PI / 4);
//	//hitable* list[2];
//	//list[0] = new sphere(vec3(-R, 0, -1), R, new lambertian(vec3(0, 0, 1)));
//	//list[1] = new sphere(vec3( R, 0, -1), R, new lambertian(vec3(1, 0, 0)));
//	//hitable* world = new hitable_list(list, 2);
//	//camera cam(90,float(nx)/float(ny));
//	// The main ray-tracing based rendering loop
//	// TODO: finish your own ray-tracing renderer according to the given tutorials
//	for (int j = ny - 1; j >= 0; j--)
//	{
//		for (int i = 0; i < nx; i++)
//		{
//
//			//float r = float(i) / float(nx);
//			//float g = float(j) / float(ny);
//			//float b = 0.2f;
//
//
//			float u = float(i) / float(nx);
//			float v = float(j) / float(ny);
//
//			ray l = cam.get_ray(u, v);
//			vec3 p = l.point_at_parameter(2.0);
//			vec3 col = color(l, world, 0);
//
//			// Note: call writeRGBToCanvas() to write the pixel once you finish the calculation of the color 
//			writeRGBToCanvas(sqrt(col.r()), sqrt(col.g()), sqrt(col.b()), i, j);
//		}
//	}
//	double endFrame = clock();
//	double timeConsuming = static_cast<double>(endFrame - startFrame) / CLOCKS_PER_SEC;
//
//	std::cout << "Ray-tracing based rendering over..." << std::endl;
//	std::cout << "The rendering task takes " << timeConsuming << " seconds" << std::endl;
//}

hitable* random_scene() {
	int n = 500;
	hitable** list = new hitable * [n + 1];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
	int i = 1;
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = drand48();
			vec3 center(a + 0.9 * drand48(), 0.2, b + 0.9 * drand48());
			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				if (choose_mat < 0.8) {
					list[i++] = new sphere(center, 0.2, new lambertian(vec3(drand48() * drand48(), drand48() * drand48(), drand48() * drand48())));
				}
				else if (choose_mat < 0.95) {
					list[i++] = new sphere(center, 0.2,
						new metal(vec3(0.5 * (1 + drand48()), 0.5 * (1 + drand48()), 0.5 * (1 + drand48())), 0.5 * (1 + drand48())));
				}
				else{
					list[i++] = new sphere(center, 0.2, new dielectric(1.5));
				}
			}
		}
	}

	list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

	return new hitable_list(list, i);
}

void rendering()
{

	printf("CGAssignment4 (built %s at %s) \n", __DATE__, __TIME__);
	std::cout << "Ray-tracing based rendering launched..." << std::endl;
	int nx = 1200;
	int ny = 800;
	int ns = 10;

	double startFrame = clock();

	hitable* world = random_scene();

	vec3 lookfrom(13, 3, 2);
	vec3 lookat(0, 0, 0);
	float disk_to_focus = 10.0;
	float aperture = 0.1;

	camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, disk_to_focus);

	// The main ray-tracing based rendering loop
	// TODO: finish your own ray-tracing renderer according to the given tutorials
	for (int j = ny - 1; j >= 0; j--)
	{
		for (int i = 0; i < nx; i++)
		{

			float u = float(i) / float(nx);
			float v = float(j) / float(ny);

			ray l = cam.get_ray(u, v);
			vec3 p = l.point_at_parameter(2.0);
			vec3 col = color(l, world, 0);

			// Note: call writeRGBToCanvas() to write the pixel once you finish the calculation of the color 
			writeRGBToCanvas(sqrt(col.r()), sqrt(col.g()), sqrt(col.b()), i, j);
		}
	}
	double endFrame = clock();
	double timeConsuming = static_cast<double>(endFrame - startFrame) / CLOCKS_PER_SEC;

	std::cout << "Ray-tracing based rendering over..." << std::endl;
	std::cout << "The rendering task takes " << timeConsuming << " seconds" << std::endl;
}

