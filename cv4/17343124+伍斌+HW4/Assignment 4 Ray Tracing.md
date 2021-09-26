

# Assignment 4: Ray Tracing

姓名：伍斌

学号：17343124



[TOC]



---

## Task 1

阅读 《Ray Tracing in One Weekend》 的 Chapter 3 ，完成射线类和简单的摄像机构建，并渲染一个渐变的蓝色天空背景图。贴出效果图，简述你遇到的问题以及是如何解决的。

**代码**

添加ray.h，增加color函数，修改render函数。

```c++
#ifndef RAYH
#define RAYH
#include "vec3.h"


class ray
{
public:
	ray() {}
	ray(const vec3& a, const vec3& b) { A = a; B = b; }
	vec3 origin()const {return A; }
	vec3 direction()const { return B; }
	vec3 point_at_parameter(float t)const { return A + t * B; }

	vec3 A;
	vec3 B;
};

#endif
```

```c++
vec3 color(const ray& r) {
	vec3 unit_direction = unit_vector(r.direction());
	float t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}
```

```c++
void rendering()
{

	printf("CGAssignment4 (built %s at %s) \n", __DATE__, __TIME__);
	std::cout << "Ray-tracing based rendering launched..." << std::endl;
	int nx = gWidth;
	int ny = gHeight;

	double startFrame = clock();

	vec3 lower_left_corner(-2.0, -1.0, -1.0);
	vec3 horizontal(4.0, 0.0, 0.0);
	vec3 vertical(0.0, 2.0, 0.0);
	vec3 origin(0.0, 0.0, 0.0);

	// The main ray-tracing based rendering loop
	// TODO: finish your own ray-tracing renderer according to the given tutorials
	for (int j = ny - 1; j >= 0; j--)
	{
		for (int i = 0; i < nx; i++)
		{

			//float r = float(i) / float(nx);
			//float g = float(j) / float(ny);
			//float b = 0.2f;

			double u = static_cast<float>(i) / static_cast<float>(nx);
			double v = static_cast<float>(j) / static_cast<float>(ny);
			ray l(origin, lower_left_corner + u * horizontal + v * vertical);
			vec3 col = color(l);

			// Note: call writeRGBToCanvas() to write the pixel once you finish the calculation of the color 
			writeRGBToCanvas(col.r(), col.g(),col.b(), i, j);
		}
	}
	double endFrame = clock();
	double timeConsuming = static_cast<double>(endFrame - startFrame) / CLOCKS_PER_SEC;

	std::cout << "Ray-tracing based rendering over..." << std::endl;
	std::cout << "The rendering task takes " << timeConsuming << " seconds" << std::endl;
}
```

**实现效果**

![image-20210527203824946](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210527203824946.png)

**遇到的问题&解决**

跟着教程做没有遇到什么问题。



---

## Task 2

阅读 《Ray Tracing in One Weekend》 的 Chapter 4 、 Chapter 5 和 Chapter 6 ，为场景添加并渲染一个简单的球形物体。贴出效果图，简述你遇到的问题以及是如何解决的。

### Chapter 4

**代码**

增加hit_sphere函数，修改color函数。

```c++
bool hit_sphere(const vec3& center, float radius, const ray& r) {
	vec3 oc = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	float b = 2.0 * dot(oc, r.direction());
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - 4 * a * c;
	return (discriminant > 0);
}


vec3 color(const ray& r) {
	if (hit_sphere(vec3(0, 0, -1), 0.5, r))
		return vec3(1, 0, 0);
	vec3 unit_direction = unit_vector(r.direction());
	float t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}
```

**实现效果**

![image-20210527205820470](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210527205820470.png)



### Chapter 5

#### 1.修改hit_sphere函数和color函数。

**代码**

```c++
float hit_sphere(const vec3& center, float radius, const ray& r) {
	vec3 oc = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	float b = 2.0 * dot(oc, r.direction());
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - 4 * a * c;
	if (discriminant < 0) {
		return -1.0;
	}
	else {
		return (-b - sqrt(discriminant)) / (2.0 * a);
	}
}


vec3 color(const ray& r) {
	float t = hit_sphere(vec3(0, 0, -1), 0.5, r);
	if (t > 0.0) {
		vec3 N = unit_vector(r.point_at_parameter(t) - vec3(0, 0, -1));
		return 0.5 * vec3(N.x() + 1.0, N.y() + 1.0, N.z() + 1.0);
	}
	vec3 unit_direction = unit_vector(r.direction());
	t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}
```

**实现效果**

![image-20210527211449617](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210527211449617.png)



#### 2.封装hitable、sphere和sphere_list

封装的代码和教程完全一致，这里略去。

修改color函数和render函数。

```c++
vec3 color(const ray& r, hitable* world) {
	hit_record rec;
	if (world->hit(r, 0.0, FLT_MAX, rec)) {
		return 0.5 * vec3(rec.normal.x() + 1, rec.normal.y() + 1, rec.normal.z() + 1);
	}
	else {
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}
}

void rendering()
{

	printf("CGAssignment4 (built %s at %s) \n", __DATE__, __TIME__);
	std::cout << "Ray-tracing based rendering launched..." << std::endl;
	int nx = gWidth;
	int ny = gHeight;

	double startFrame = clock();

	vec3 lower_left_corner(-2.0, -1.0, -1.0);
	vec3 horizontal(4.0, 0.0, 0.0);
	vec3 vertical(0.0, 2.0, 0.0);
	vec3 origin(0.0, 0.0, 0.0);

	hitable* list[2];
	list[0] = new sphere(vec3(0, 0, -1), 0.5);
	list[1] = new sphere(vec3(0, -100.5, -1), 100);
	hitable* world = new hitable_list(list, 2);

	// The main ray-tracing based rendering loop
	// TODO: finish your own ray-tracing renderer according to the given tutorials
	for (int j = ny - 1; j >= 0; j--)
	{
		for (int i = 0; i < nx; i++)
		{

			//float r = float(i) / float(nx);
			//float g = float(j) / float(ny);
			//float b = 0.2f;


			float u = float(i) / float(nx);
			float v = float(j) / float(ny);

			ray l(origin, lower_left_corner + u * horizontal + v * vertical);
			vec3 p = l.point_at_parameter(2.0);
			vec3 col = color(l, world);

			// Note: call writeRGBToCanvas() to write the pixel once you finish the calculation of the color 
			writeRGBToCanvas(col.r(), col.g(), col.b(), i, j);
		}
	}
	double endFrame = clock();
	double timeConsuming = static_cast<double>(endFrame - startFrame) / CLOCKS_PER_SEC;

	std::cout << "Ray-tracing based rendering over..." << std::endl;
	std::cout << "The rendering task takes " << timeConsuming << " seconds" << std::endl;
}

```

**实现效果**

![image-20210528002249507](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210528002249507.png)



### Chapter 6 

封装camera，修改render函数。

```c++
#ifndef CAMERAH
#define CAMERAH

#include"ray.h"

class camera {
public:
	camera(){
		lower_left_corner = vec3(-2.0, -1.0, -1.0);
		horizontal = vec3(4.0, 0.0, 0.0);
		vertical = vec3(0.0, 2.0, 0.0);
		origin = vec3(0.0, 0.0, 0.0);
	}
	ray get_ray(float u, float v) {
		return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
	}

	vec3 origin;
	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
};
#endif // !CAMERAH
```

```c++
void rendering()
{

	printf("CGAssignment4 (built %s at %s) \n", __DATE__, __TIME__);
	std::cout << "Ray-tracing based rendering launched..." << std::endl;
	int nx = gWidth;
	int ny = gHeight;

	double startFrame = clock();

	hitable* list[2];
	list[0] = new sphere(vec3(0, 0, -1), 0.5);
	list[1] = new sphere(vec3(0, -100.5, -1), 100);
	hitable* world = new hitable_list(list, 2);

	camera cam;
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
			vec3 col = color(l, world);

			// Note: call writeRGBToCanvas() to write the pixel once you finish the calculation of the color 
			writeRGBToCanvas(col.r(), col.g(), col.b(), i, j);
		}
	}
	double endFrame = clock();
	double timeConsuming = static_cast<double>(endFrame - startFrame) / CLOCKS_PER_SEC;

	std::cout << "Ray-tracing based rendering over..." << std::endl;
	std::cout << "The rendering task takes " << timeConsuming << " seconds" << std::endl;
}
```

**实现效果**

![image-20210528003540738](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210528003540738.png)

边缘部分与上一步对比可以看出锯齿现象减弱。



### 遇到的问题&解决

在chapter5部分按照教程所给的进行修改时出现了上天空也出现地面的情况，在同学的帮助下调整了参数恢复正常。



---

## Task3

阅读 《Ray Tracing in One Weekend》 的 Chapter 7 、 Chapter 8 和 Chapter 9 ，为场景
中的球形物体添加漫反射材质、金属材质和电解质材质，并渲染相应的材质效果图。贴出效果图，简述
你遇到的问题以及是如何解决的。

### Chapter 7

**代码**

```c++
vec3 random_in_unit_sphere() {
	vec3 p;
	do {
		p = 2.0 * vec3(rand() % 10000 / 10000.0, rand() % 10000 / 10000.0, rand() % 10000 / 10000.0) - vec3(1, 1, 1);
	} while (p.squared_length() >= 1.0);
	return p;
}

vec3 color(const ray& r, hitable* world) {
	hit_record rec;
	if (world->hit(r, 0.0, FLT_MAX, rec)) {
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		return 0.5 * color(ray(rec.p, target - rec.p), world);
	}
	else {
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}
}

```

**实现效果**

![image-20210528005107503](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210528005107503.png)

**开方**

```c++
	writeRGBToCanvas(sqrt(col.r()), sqrt(col.g()), sqrt(col.b()), i, j);
```

**实现效果**

![image-20210528005859253](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210528005859253.png)

**调整参数**

```c++
if (world->hit(r, 0.001, FLT_MAX, rec))
```

**实现效果**

![image-20210528010007923](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210528010007923.png)



### Chapter 8

#### 1、引入材质material，创建子类lambertian和metal，增加函数reflect，并修改sphere、color和render。

```c++
#ifndef MATERIALH
#define MATERIALH
#include "ray.h"
#include "vec3.h"
#include "hitable.h"

vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2 * dot(v, n) * n;
}
vec3 random_in_unit_sphere();

class material
{
public:
    // 根据入射光、物体(rec),求出反射光方向及其光照衰减系数
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
};


// 金属: 镜面反射
class metal : public material
{
public:
    metal(const vec3& a) : albedo(a) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
    {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }
    vec3 albedo;
};


//金属：漫反射
class lambertian : public material
{
public:
    lambertian(const vec3& a) : albedo(a) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
    {
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        scattered = ray(rec.p, target - rec.p);
        attenuation = albedo;
        return true;
    }
    vec3 albedo;
};

#endif
```

hitable和sphere添加指针。

```c++
#ifndef HITABLEH
#define HITABLEH

#include "ray.h"
class material;
struct hit_record
{
    float t;
    vec3 p;
    vec3 normal;
    material* mat_ptr;
};
class hitable
{
public:
    virtual bool hit(const ray& r, float t_min, float tmax, hit_record& rec) const = 0;
};

#endif // !HITABLEH
```

```c++
class sphere : public hitable
{
public:
    sphere() {}
    sphere(vec3 cen, float r, material* p) : center(cen), radius(r), mat_ptr(p) {};
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const
    {
        {
            vec3 oc = r.origin() - center;
            float a = dot(r.direction(), r.direction());
            float b = dot(oc, r.direction());
            float c = dot(oc, oc) - radius * radius;
            float discriminant = b * b - a * c;
            // 计算出交点(方程的解)
            if (discriminant > 0)
            {
                // 第一个解(较小值)
                float temp = (-b - sqrt(b * b - a * c)) / a;
                if (temp < t_max && temp > t_min)
                {
                    rec.t = temp;
                    rec.p = r.point_at_parameter(rec.t);
                    rec.normal = (rec.p - center) / radius; // 法向量方向"偏离"物体
                    rec.mat_ptr = mat_ptr;
                    return true;
                }
                // 第二个解(较大值)
                temp = (-b + sqrt(b * b - a * c)) / a;
                if (temp < t_max && temp > t_min)
                {
                    rec.t = temp;
                    rec.p = r.point_at_parameter(rec.t);
                    rec.normal = (rec.p - center) / radius;
                    rec.mat_ptr = mat_ptr;
                    return true;
                }
            }

            return false;
        }
    }
    vec3 center;
    float radius;
    material* mat_ptr;
};
#endif // !SPHEREH
```

color和render按照教程上的代码修改，完全一致，这里略去。

**实现效果**

![image-20210528183209490](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210528183209490.png)

#### 2、引入fuzz参数，修改metal类
```c++
// 金属: 镜面反射
class metal : public material
{
public:
    metal(const vec3 &a, float f) : albedo(a) { fuzz = f > 1 ? 1 : f; }
    virtual bool scatter(const ray &r_in, const hit_record &rec, vec3 &attenuation, ray &scattered) const
    {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }
    vec3 albedo;
    float fuzz; // 高光效果(反射光模糊系数)(最大为1,越大则反射光偏离范围越大，效果越模糊)
};
```

左侧f=0.5，右侧f=1.0。

```c++
list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2),0.3));
list[3] = new sphere(vec3(-1, 0, -1), 0.5, new metal(vec3(0.8, 0.8, 0.8),1.0));
```

**实现效果**

![image-20210528183436861](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210528183436861.png)



### Chapter 9

#### 1.增加了dielectric电介质材质类。

**代码**

```c++
// 电介质: 镜面反射 or 折射
class dielectric : public material
{
public:
    dielectric(float ri) : ref_idx(ri){}

    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation,
        ray& scattered) const {
        vec3 outward_normal;
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        float ni_over_nt;
        attenuation = vec3(1.0, 1.0, 1.0);
        vec3 refracted;
        if (dot(r_in.direction(), rec.normal) > 0) {
            outward_normal = -rec.normal;
            ni_over_nt = ref_idx;
        }
        else {
            outward_normal = rec.normal;
            ni_over_nt = 1.0 / ref_idx;
        }
        if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
                scattered = ray(rec.p, refracted);
        }
        else {
            scattered = ray(rec.p, reflected);
        }
        return true;
    }
    float ref_idx;
};
```

```c++
bool refract(const vec3& v, const  vec3& n, float ni_over_nt, vec3& refracted) {
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);
    if (discriminant > 0) {
        refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
        return true;
    }
    else
        return false;
}
```

**实现效果**

![image-20210528184819590](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210528184819590.png)



#### 2、改进dielectric反射/折射处理，引入schlick函数。

**代码**

```c++
float schlick(float cosine, float ref_idx) {
    float r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine),5);
}
```

```c++
// 电介质: 镜面反射 or 折射
class dielectric : public material
{
public:
    dielectric(float ri) : ref_idx(ri){}

    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation,
        ray& scattered) const {
        vec3 outward_normal;
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        float ni_over_nt;
        attenuation = vec3(1.0, 1.0, 1.0);
        vec3 refracted;
        float reflect_prob;
        float cosine;
        if (dot(r_in.direction(), rec.normal) > 0) {
            outward_normal = -rec.normal;
            ni_over_nt = ref_idx;
            cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }
        else {
            outward_normal = rec.normal;
            ni_over_nt = 1.0 / ref_idx;
            cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }

        if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
        {
            reflect_prob = schlick(cosine, ref_idx);
        }
        else
        {
            scattered = ray(rec.p, reflected);
            reflect_prob = 1.0;
        }
        if (drand48() < reflect_prob)
        {
            scattered = ray(rec.p, reflected);
        }
        else
        {
            scattered = ray(rec.p, refracted);
        }
        return true;
    }
    float ref_idx;
};
```

修改rendering：

```c++
	list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));
	list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
	list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2),0.3));
	list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));
	list[4] = new sphere(vec3(-1, 0, -1), -0.45, new dielectric(1.5));
```

**实现效果**

![image-20210528190306781](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210528190306781.png)



### 遇到的问题&解决

Chapter 9教程中给出的代码存在问题。

```c++
virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation,
ray& scattered) const{
  vec3 outward_normal;
  vec3 reflected = reflect(r_in.direction(), rec.normal);
  float ni_over_nt;
  attenuation = vec3(1.0, 1.0, 0.0);//错误1：过滤了蓝光
  vec3 refracted;
  if (dot(r_in.direction(), rec.normal) > 0){
    outward_normal = -rec.normal;
    ni_over_nt = ref_idx;
 }
  else{
    outward_normal = rec.normal;
    ni_over_nt = 1.0 / ref_idx;
 }
  if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)){
    scattered = ray(rec.p, refracted);
 }
  else{
    scattered = ray(rec.p, reflected);
   return false;//错误2：导致不考虑折射时也不考虑反射
 }
  return true;
}
```

如果不修改，实现效果是这样的：

![image-20210528191059987](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210528191059987.png)

修改后方符合题意。



---

## Task 4

阅读 《Ray Tracing in One Weekend》 的 Chapter 10 和 Chapter 11 ，实现摄像机的聚焦模糊效果。贴出效果图，简述你遇到的问题以及是如何解决的。

### Chapter 10

#### 1、修改camera类，引入vfov、aspect。

**代码**

```c++
#ifndef CAMERAH
#define CAMERAH

#include"ray.h"

class camera {
public:
	camera(float vfov, float aspect) {
		float theta = vfov * M_PI / 180;
		float half_height = tan(theta / 2);
		float half_width = aspect * half_height;
		lower_left_corner = vec3(-half_width, -half_height, -1.0);
		horizontal = vec3(2 * half_width, 0.0, 0.0);
		vertical = vec3(0.0, 2 * half_height, 0.0);
		origin = vec3(0.0, 0.0, 0.0);
	}

	ray get_ray(float u, float v) {
		return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
	}

	vec3 origin;
	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
};
#endif // !CAMERAH
```

rendering中替换：

```c++
	float R = cos(M_PI / 4);
	hitable* list[2];
	list[0] = new sphere(vec3(-R, 0, -1), R, new lambertian(vec3(0, 0, 1)));
	list[1] = new sphere(vec3( R, 0, -1), R, new lambertian(vec3(1, 0, 0)));
	hitable* world = new hitable_list(list, 2);

	camera cam(90,float(nx)/float(ny));
```

**实现效果**

![image-20210528194138287](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210528194138287.png)



#### 2、引入lookfrom和lookat。

**代码**

```c++
#ifndef CAMERAH
#define CAMERAH

#include"ray.h"

class camera {
public:
	camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect)
	{
		vec3 u, v, w;
		float theta = vfov * M_PI / 180;
		float half_height = tan(theta / 2);
		float half_width = aspect * half_height;
		origin = lookfrom;                  // 相机位置
		w = unit_vector(lookfrom - lookat); // 用相机视角的(u,v,w)代替原来的(x,y,z)
		u = unit_vector(cross(vup, w));
		v = cross(w, u); // 叉乘
		lower_left_corner = vec3(-half_width, -half_height, -1.0);
		lower_left_corner = origin - half_width * u - half_height * v - w;
		horizontal = 2 * half_width * u;
		vertical = 2 * half_height * v;
	}


	ray get_ray(float u, float v) {
		return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
	}

	vec3 origin;
	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
};
#endif // !CAMERAH

```

```c++
camera cam(vec3(-2, 2, 1), vec3(0, 0, -1), vec3(0, 1, 0), 90, float(nx) / float(ny));
```



**实现效果**

![image-20210528195648582](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210528195648582.png)



### Chapter 11

进一步修改camera，加入focus_dist和lens_radius。

**代码**

```c++
#ifndef CAMERAH
#define CAMERAH

#include"ray.h"
#include"material.h"


vec3 random_in_unit_disk(){
	vec3 p;
	do{
		p = 2.0 * vec3(drand48(), drand48(), 0) - vec3(1, 1, 0);
	} while (dot(p, p) >= 1.0);
	return p;
}
// 相关参数: 视野范围、相机位置、相机朝向、透镜半径、光晕
class camera
{
public:
	camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist)
	{
		lens_radius = aperture / 2;
		float theta = vfov * M_PI / 180;
		float half_height = tan(theta / 2);
		float half_width = aspect * half_height;
		origin = lookfrom;                  // 相机位置
		w = unit_vector(lookfrom - lookat); // 用相机视角的(u,v,w)代替原来的(x,y,z)
		u = unit_vector(cross(vup, w));
		v = cross(w, u); // 叉乘
		lower_left_corner = origin - half_width * focus_dist * u - half_height * focus_dist * v - focus_dist * w;
		horizontal = 2 * half_width * focus_dist * u;
		vertical = 2 * half_height * focus_dist * v;
	}
	ray get_ray(float s, float t)
	{
		vec3 rd = lens_radius * random_in_unit_disk();
		vec3 offset = u * rd.x() + v * rd.y();
		return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
	}
	vec3 origin;
	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 u, v, w;
	float lens_radius;
};
#endif // !CAMERAH

```

修改rendering：

```c++
	vec3 lookfrom(3, 3, 2);
	vec3 lookat(0, 0, -1);
	float disk_to_focus = (lookfrom - lookat).length();
	float aperture = 2.0;

	camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny),aperture,disk_to_focus);
```

**实现效果**

![image-20210528201810728](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210528201810728.png)



### 遇到的问题&解决

按照教程做没有遇到什么问题。



---

## Task5、

阅读 《Ray Tracing in One Weekend》 的 Chapter 12 ，渲染一张炫酷真实的图片。贴出效果图，简述你遇到的问题以及是如何解决的。

**实现效果**

部分：

![image-20210528204958985](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210528204958985.png)

调整窗口大小重新后：

![image-20210528202054610](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210528202054610.png)



**遇到的问题&解决**

按照教程做没有遇到什么问题。



---

## Task 6

阅读 《Ray Tracing The Next Week》 的 Chapter 2 ，为场景物体构建一颗BVH树（Bounding Volume Hierarchies，包围体层次结构），加速追踪的射线与场景的求交计算过程。对比有无BVH树的渲染时间，简述你遇到的问题以及是如何解决的。

针对Task5未修改前图：

![image-20210528211541787](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210528211541787.png)

修改前渲染用时：

![image-20210528211439914](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210528211439914.png)

按照教程添加bvh树后：

![image-20210528223417484](C:\Users\42405\AppData\Roaming\Typora\typora-user-images\image-20210528223417484.png)

相较没添加前确实更快一点。（代码较长且基本按照教程编写，这里就不贴了）



---

## Task 7

​		这次实验阅读量挺大的，较长的时间用在阅读教程上，一步步跟着教程去做。我个人感觉做完整个实验对课上所介绍的光追的每一步有了更切身的体会，并且通过跟着一步步去做还是对ray tracing整个过程有了更深入的理解，在完成时非常有成就感。这次的代码量比想象中少一些，因为教程基本上都给出来了，但中间还是碰上了许多问题，比如教程本身出错。但总的来说，这个教程加上助教给出的环境对于新手学习光线追踪非常友好，而且体会至深。



