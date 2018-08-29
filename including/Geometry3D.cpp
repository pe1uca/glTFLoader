#include "Geometry3D.h"
#include <cmath>
#include <cfloat>

#define CMP(x, y) \
		(fabsf((x)-(y)) <= FLT_EPSILON *	\
			fmaxf(1.0f,						\
			fmaxf(fabsf(x), fabsf(y)))		\
		)
#define CLAMP(number, minimum, maximum)		\
		number = (number < minimum) ?		\
		minimum : (number > maximum ?		\
		maximum : number)

float Lenght(const Line& line)
{
	return Magnitude(line.start - line.end);
}
float LenghtSq(const Line& line)
{
	return MagnitudeSq(line.start - line.end);
}

Ray FromPoints(const Point& from, const Point& to)
{
	return Ray(from, Normalized(to - from));
}

vec3 GetMin(const AABB& aabb)
{
	vec3 p1 = aabb.position + aabb.size;
	vec3 p2 = aabb.position - aabb.size;

	return vec3(fminf(p1.x, p2.x),
		fminf(p1.y, p2.y),
		fminf(p1.z, p2.z));
}
vec3 GetMax(const AABB& aabb)
{
	vec3 p1 = aabb.position + aabb.size;
	vec3 p2 = aabb.position - aabb.size;

	return vec3(fmaxf(p1.x, p2.x),
		fmaxf(p1.y, p2.y),
		fmaxf(p1.z, p2.z));
}
AABB FromMinMax(const vec3& min, const vec3& max)
{
	return AABB((min + max) * 0.5f, (max - min) * 0.5f);
}

float PlaneEquation(const Point& point, const Plane& plane)
{
	return Dot(point, plane.normal) - plane.distance;
}

bool PointInSphere(const Point& point, const Sphere& sphere)
{
	return MagnitudeSq(point - sphere.position) < sphere.radius * sphere.radius;
}
Point ClosestPoint(const Sphere& sphere, const Point& point)
{
	vec3 sphereToPoint = point - sphere.position;
	Normalize(sphereToPoint);
	sphereToPoint = sphereToPoint * sphere.radius;
	return sphereToPoint + sphere.position;
}

bool PointInAABB(const Point& point, const AABB& aabb)
{
	Point min = GetMin(aabb);
	Point max = GetMax(aabb);
	return point.x > min.x && point.y > min.y && point.z > min.z &&
		point.x < max.x && point.y < max.y && point.z < max.z;
}
Point ClosestPoint(const AABB& aabb, const Point& point)
{
	Point result = point;
	Point min = GetMin(aabb);
	Point max = GetMax(aabb);

	CLAMP(result.x, min.x, max.x);
	CLAMP(result.y, min.y, max.y);
	CLAMP(result.z, min.z, max.z);
	return result;
}

bool PointInOBB(const Point& point, const OBB& obb)
{
	vec3 dir = point - obb.position;
	for (int i = 0; i < 3; ++i)
	{
		const float* orientation = &obb.orientation.asArray[i * 3];
		vec3 axis(orientation[0], orientation[1], orientation[2]);
		float distance = Dot(dir, axis);
		if (distance > obb.size.asArray[i] ||
			distance < -obb.size.asArray[i])
			return false;
	}
	return true;
}
Point ClosestPoint(const OBB& obb, const Point& point)
{
	Point result = obb.position;
	vec3 dir = point - obb.position;
	for (int i = 0; i < 3; ++i)
	{
		const float* orientation = &obb.orientation.asArray[i * 3];
		vec3 axis(orientation[0], orientation[1], orientation[2]);
		float distance = Dot(dir, axis);
		CLAMP(distance, -obb.size.asArray[i], obb.size.asArray[i]);
		result = result + (axis * distance);
	}
	return result;
}

bool PointOnPlane(const Point& point, const Plane& plane)
{
	return CMP(Dot(point, plane.normal) - plane.distance, 0.0f);
}
Point ClosestPoint(const Plane& plane, const Point& point)
{
	return point - plane.normal * (Dot(plane.normal, point) - plane.distance);
}

bool PointOnLine(const Point& point, const Line& line)
{
	Point closest = ClosestPoint(line, point);
	float distanceSq = MagnitudeSq(closest - point);
	return CMP(distanceSq, 0.0f);
}
Point ClosestPoint(const Line& line, const Point& point)
{
	vec3 lVec = line.end - line.start;
	float t = Dot(point - line.start, lVec) / Dot(lVec, lVec);
	CLAMP(t, 0.0f, 1.0f);
	return line.start + lVec * t;
}

bool PointOnRay(const Point& point, const Ray& ray)
{
	if (point == ray.origin)
		return true;
	vec3 norm = point - ray.origin;
	Normalize(norm);
	return CMP(Dot(norm, ray.direction), 1.0f);
}
Point ClosestPoint(const Ray& ray, const Point& point)
{
	float t = Dot(point - ray.origin, ray.direction);
	t = fmaxf(t, 0.0f);
	return Point(ray.origin + ray.direction * t);
}