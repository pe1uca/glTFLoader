#pragma once
#ifndef _GEOMETRY_3D_H_
#define _GEOMETRY_3D_H_

#include "vectors.h"
#include "matrices.h"

#define AABBSphere(aabb, sphere) SphereAABB(sphere, aabb)
#define OBBSphere(obb, sphere) SphereOBB(sphere, obb)
#define PlaneSphere(plane, sphere) SpherePlane(sphere, plane)
#define OBBAABB(obb, aabb) AABBOBB(aabb, obb)
#define PlaneAABB(plane, aabb) AABBPlane(aabb, plane)
#define PlaneOBB(plane, obb) OBBPlane(obb, plane)

typedef vec3 Point;

#pragma warning(push)
#pragma warning(disable:4201)
typedef struct Line {
	Point start;
	Point end;

	inline Line() {}
	inline Line(const Point& s, const Point& e) :
		start(s), end(e) {}
} Line;
typedef struct Ray {
	Point origin;
	vec3 direction;

	inline Ray() : direction(0.0f, 0.0f, 1.0f) {}
	inline Ray(const Point& o, const vec3& d) :
		origin(o), direction(d)
	{
		NormalizeDirection();
	}
	inline void NormalizeDirection()
	{
		Normalize(direction);
	}
} Ray;
typedef struct Sphere {
	Point position;
	float radius;

	inline Sphere() : radius(1.0f) {}
	inline Sphere(const Point& p, float r) :
		position(p), radius(r) {}
} Sphere;
typedef struct AABB {
	Point position;
	vec3 size;

	inline AABB() : size(1, 1, 1) {}
	inline AABB(const Point& p, const vec3& s) :
		position(p), size(s) {}
} AABB;
typedef struct OBB {
	Point position;
	vec3 size;
	mat3 orientation;

	inline OBB() : size(1, 1, 1) {}
	inline OBB(const Point& p, const vec3& s) :
		position(p), size(s) {}
	inline OBB(const Point& p, const vec3& s, const mat3& o) :
		position(p), size(s), orientation(o) {}
} OBB;
typedef struct Plane {
	vec3 normal;
	float distance;

	inline Plane() : normal(1, 0, 0) {}
	inline Plane(const vec3& n, float d) :
		normal(n), distance(d) {}
} Plane;
typedef struct Triangle {
	union {
		struct {
			Point a;
			Point b;
			Point c;
		};
		Point points[3];
		float values[9];
	};

	inline Triangle() {}
	inline Triangle(const Point& p1, const Point& p2, const Point& p3) :
		a(p1), b(p2), c(p3) {}
} Triangle;
typedef struct Interval {
	float min;
	float max;
} Interval;
#pragma warning(pop)

float Lenght(const Line& line);
float LenghtSq(const Line& line);

Ray FromPoints(const Point& from, const Point& to);

vec3 GetMin(const AABB& aabb);
vec3 GetMax(const AABB& aabb);
AABB FromMinMax(const vec3& min, const vec3& max);

float PlaneEquation(const Point& point, const Plane& plane);

bool PointInSphere(const Point& point, const Sphere& sphere);
Point ClosestPoint(const Sphere& sphere, const Point& point);

bool PointInAABB(const Point& point, const AABB& aabb);
Point ClosestPoint(const AABB& aabb, const Point& point);

bool PointInOBB(const Point& point, const OBB& obb);
Point ClosestPoint(const OBB& obb, const Point& point);

bool PointOnPlane(const Point& point, const Plane& plane);
Point ClosestPoint(const Plane& plane, const Point& point);

bool PointOnLine(const Point& point, const Line& line);
Point ClosestPoint(const Line& line, const Point& point);

bool PointOnRay(const Point& point, const Ray& ray);
Point ClosestPoint(const Ray& ray, const Point& point);

bool SphereSphere(const Sphere& s1, const Sphere& s2);
bool SphereAABB(const Sphere& sphere, const AABB& aabb);
bool SphereOBB(const Sphere& sphere, const OBB& obb);
bool SpherePlane(const Sphere& sphere, const Plane& plane);

Interval GetInterval(const AABB& aabb, const vec3& axis);
Interval GetInterval(const OBB& obb, const vec3& axis);
bool OverlapOnAxis(const AABB& aabb, const OBB& obb, const vec3& axis);
bool OverlapOnAxis(const OBB& obb1, const OBB& obb2, const vec3& axis);

bool AABBAABB(const AABB& aabb1, const AABB& aabb2);
bool AABBOBB(const AABB& aabb, const OBB& obb);
bool AABBPlane(const AABB& aabb, const Plane& plane);

bool OBBOBB(const OBB& obb1, const OBB& obb2);
bool OBBPlane(const OBB& obb, const Plane& plane);

bool PlanePlane(const Plane& plane1, const Plane& plane2);

float Raycast(const Sphere& sphere, const Ray& ray);
float Raycast(const AABB& aabb, const Ray& ray);
float Raycast(const OBB& obb, const Ray& ray);
float Raycast(const Plane& plane, const Ray& ray);

bool Linetest(const Sphere& sphere, const Line& line);
bool Linetest(const AABB& aabb, const Line& line);
bool Linetest(const OBB& obb, const Line& line);
bool Linetest(const Plane& plane, const Line& line);


#endif // !_GEOMETRY_3D_H_
