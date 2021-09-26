#ifndef SPHEREH
#define SPHEREH

#include"hitable.h"
#include "ray.h"
#include"aabb.h"

//class sphere :public hitable {
//public:
//	sphere() {}
//	sphere(vec3 cen, float r, material* p) :center(cen), radius(r), mat_ptr(p) {};
//	virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
//
//	vec3 center;
//	float radius;
//	material* mat_ptr;
//};
//
//bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec)const {
//	vec3 oc = r.origin() - center;
//	float a = dot(r.direction(), r.direction());
//	float b = dot(oc, r.direction());
//	float c = dot(oc, oc) - radius * radius;
//	float discriminant = b * b - a * c;
//	if (discriminant > 0) {
//		float temp = (-b - sqrt(discriminant)) / a;
//		if (temp < t_max && temp > t_min) {
//			rec.t = temp;
//			rec.p = r.point_at_parameter(rec.t);
//			rec.normal = (rec.p - center) / radius;
//			rec.mat_ptr = mat_ptr;
//			return true;
//		}
//		temp = (-b + sqrt(discriminant)) / a;
//		if (temp < t_max && temp > t_min) {
//			rec.t = temp;
//			rec.p = r.point_at_parameter(rec.t);
//			rec.normal = (rec.p - center) / radius;
//			rec.mat_ptr = mat_ptr;
//			return true;
//		}
//	}
//	return false;
//}

aabb surrounding_box(aabb box0, aabb box1) {
    vec3 small(fmin(box0.min().x(), box1.min().x()),
        fmin(box0.min().y(), box1.min().y()),
        fmin(box0.min().z(), box1.min().z()));
    vec3 big(fmax(box0.max().x(), box1.max().x()),
        fmax(box0.max().y(), box1.max().y()),
        fmax(box0.max().z(), box1.max().z()));
    return aabb(small, big);
}


// 球体
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
    virtual bool bounding_box(float t0, float t1, aabb& box) const {
        box = aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
        return true;
    }

    vec3 center;
    float radius;
    material* mat_ptr;
};
#endif // !SPHEREH

