#include "Geometry3D.h"
#include <cmath>
#include <cfloat>
#include <list>
#include <numeric>

#define CMP(x, y) \
		(fabsf((x)-(y)) <= FLT_EPSILON *	\
			fmaxf(1.0f,						\
			fmaxf(fabsf(x), fabsf(y)))		\
		)
#define CLAMP(number, minimum, maximum)		\
		number = (number < minimum) ?		\
		minimum : (number > maximum ?		\
		maximum : number)
#define OVERLAP(aMin, aMax, bMin, bMax) ((bMin <= aMax) && (aMin <= bMax))

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

bool SphereSphere(const Sphere& s1, const Sphere& s2)
{
	float radiiSum = s1.radius + s2.radius;
	return MagnitudeSq(s1.position - s2.position) < radiiSum * radiiSum;
}
bool SphereAABB(const Sphere& sphere, const AABB& aabb)
{
	Point closestPoint = ClosestPoint(aabb, sphere.position);
	return MagnitudeSq(sphere.position - closestPoint) < sphere.radius * sphere.radius;
}
bool SphereOBB(const Sphere& sphere, const OBB& obb)
{
	Point closestPoint = ClosestPoint(obb, sphere.position);
	return MagnitudeSq(sphere.position - closestPoint) < sphere.radius * sphere.radius;
}
bool SpherePlane(const Sphere& sphere, const Plane& plane)
{
	Point closestPoint = ClosestPoint(plane, sphere.position);
	return MagnitudeSq(sphere.position - closestPoint) < sphere.radius * sphere.radius;
}

Interval GetInterval(const AABB& aabb, const vec3& axis)
{
	vec3 min = GetMin(aabb);
	vec3 max = GetMax(aabb);

	vec3 vertex[8] = {
		vec3(min.x, max.y, max.z),
		vec3(min.x, max.y, min.z),
		vec3(min.x, min.y, max.z),
		vec3(min.x, min.y, min.z),
		vec3(max.x, max.y, max.z),
		vec3(max.x, max.y, min.z),
		vec3(max.x, min.y, max.z),
		vec3(max.x, min.y, min.z)
	};

	Interval result;
	result.min = result.max = Dot(axis, vertex[0]);
	for (int i = 1; i < 8; ++i)
	{
		float projection = Dot(axis, vertex[i]);
		if (projection < result.min)
			result.min = projection;
		if (projection > result.max)
			result.max = projection;
	}
	return result;
}
Interval GetInterval(const OBB& obb, const vec3& axis)
{
	vec3 vertex[8];
	vec3 C = obb.position; // OBB center
	vec3 E = obb.size; // OBB extents
	const float* o = obb.orientation.asArray;
	vec3 A[] = {
		vec3(o[0], o[1], o[2]),
		vec3(o[3], o[4], o[5]),
		vec3(o[6], o[7], o[8])
	};

	vertex[0] = C + (A[0] * E[0]) + (A[1] * E[1]) + (A[2] * E[2]);
	vertex[1] = C - (A[0] * E[0]) + (A[1] * E[1]) + (A[2] * E[2]);
	vertex[2] = C + (A[0] * E[0]) - (A[1] * E[1]) + (A[2] * E[2]);
	vertex[3] = C + (A[0] * E[0]) + (A[1] * E[1]) - (A[2] * E[2]);
	vertex[4] = C - (A[0] * E[0]) - (A[1] * E[1]) - (A[2] * E[2]);
	vertex[5] = C + (A[0] * E[0]) - (A[1] * E[1]) - (A[2] * E[2]);
	vertex[6] = C - (A[0] * E[0]) + (A[1] * E[1]) - (A[2] * E[2]);
	vertex[7] = C - (A[0] * E[0]) - (A[1] * E[1]) + (A[2] * E[2]);

	Interval result;
	result.min = result.max = Dot(axis, vertex[0]);
	for (int i = 1; i < 8; ++i)
	{
		float projection = Dot(axis, vertex[i]);
		if (projection < result.min)
			result.min = projection;
		if (projection > result.max)
			result.max = projection;
	}
	return result;
}
bool OverlapOnAxis(const AABB& aabb, const OBB& obb, const vec3& axis)
{
	Interval a = GetInterval(aabb, axis);
	Interval b = GetInterval(obb, axis);
	return OVERLAP(a.min, a.max, b.min, b.max);
}
bool OverlapOnAxis(const OBB& obb1, const OBB& obb2, const vec3& axis)
{
	Interval a = GetInterval(obb1, axis);
	Interval b = GetInterval(obb2, axis);
	return OVERLAP(a.min, a.max, b.min, b.max);
}

bool AABBAABB(const AABB& aabb1, const AABB& aabb2)
{
	Point aMin = GetMin(aabb1);
	Point aMax = GetMax(aabb1);
	Point bMin = GetMin(aabb2);
	Point bMax = GetMax(aabb2);

	return OVERLAP(aMin.x, aMax.x, bMin.x, bMax.x) &&
		OVERLAP(aMin.y, aMax.y, bMin.y, bMax.y) &&
		OVERLAP(aMin.z, aMax.z, bMin.z, bMax.z);
}
bool AABBOBB(const AABB& aabb, const OBB& obb)
{
	const float* o = obb.orientation.asArray;

	vec3 test[15] = {
		vec3(1, 0, 0),
		vec3(0, 1, 0),
		vec3(0, 0, 1),
		vec3(o[0], o[1], o[2]),
		vec3(o[3], o[4], o[5]),
		vec3(o[6], o[7], o[8])
	};

	for (int i = 0; i < 3; ++i)
	{
		test[6 + i * 3 + 0] = Cross(test[i], test[0]);
		test[6 + i * 3 + 1] = Cross(test[i], test[1]);
		test[6 + i * 3 + 2] = Cross(test[i], test[2]);
	}

	for (int i = 0; i < 15; ++i)
	{
		if (!OverlapOnAxis(aabb, obb, test[i]))
			return false;
	}
	return true;
}
bool AABBPlane(const AABB& aabb, const Plane& plane)
{
	float pLen = aabb.size.x * fabsf(plane.normal.x) +
				aabb.size.y * fabsf(plane.normal.y) +
				aabb.size.z * fabsf(plane.normal.z);
	return fabsf(Dot(plane.normal, aabb.position) - plane.distance) <= pLen;
}

bool OBBOBB(const OBB& obb1, const OBB& obb2)
{
	const float* o1 = obb1.orientation.asArray;
	const float* o2 = obb2.orientation.asArray;

	vec3 test[15] = {
		vec3(o1[0], o1[1], o1[2]),
		vec3(o1[3], o1[4], o1[5]),
		vec3(o1[6], o1[7], o1[8]),
		vec3(o2[0], o2[1], o2[2]),
		vec3(o2[3], o2[4], o2[5]),
		vec3(o2[6], o2[7], o2[8])
	};

	for (int i = 0; i < 3; ++i)
	{
		test[6 + i * 3 + 0] = Cross(test[i], test[0]);
		test[6 + i * 3 + 1] = Cross(test[i], test[1]);
		test[6 + i * 3 + 2] = Cross(test[i], test[2]);
	}

	for (int i = 0; i < 15; ++i)
	{
		if (!OverlapOnAxis(obb1, obb2, test[i]))
			return false;
	}
	return true;
}
bool OBBPlane(const OBB& obb, const Plane& plane)
{
	const float* o = obb.orientation.asArray;
	vec3 rot[] = {
		vec3(o[0], o[1], o[2]),
		vec3(o[3], o[4], o[5]),
		vec3(o[6], o[7], o[8])
	};
	float pLen = obb.size.x * fabsf(Dot(plane.normal, rot[0])) +
				obb.size.y * fabsf(Dot(plane.normal, rot[1])) +
				obb.size.z * fabsf(Dot(plane.normal, rot[2]));
	return fabsf(Dot(plane.normal, obb.position) - plane.distance) <= pLen;
}

bool PlanePlane(const Plane& plane1, const Plane& plane2)
{
	vec3 d = Cross(plane1.normal, plane2.normal);
	return CMP(Dot(d, d), 0.0f);
}

float Raycast(const Sphere& sphere, const Ray& ray)
{
	vec3 e = sphere.position - ray.origin;
	float rSq = sphere.radius * sphere.radius;
	float eSq = MagnitudeSq(e);
	float a = Dot(e, ray.direction);
	float bSq = eSq - (a * a);
	
	if (rSq - bSq < 0.0f)
		return -1;
	
	float f = sqrt(rSq - bSq);
	return eSq < rSq ? a + f : a - f; //inside the spehre : outside
}
float Raycast(const AABB& aabb, const Ray& ray)
{
	vec3 min = GetMin(aabb);
	vec3 max = GetMax(aabb);

	float idx = 1 / (CMP(ray.direction.x, 0.0f) ? 0.00001f : ray.direction.x);
	float idy = 1 / (CMP(ray.direction.y, 0.0f) ? 0.00001f : ray.direction.y);
	float idz = 1 / (CMP(ray.direction.z, 0.0f) ? 0.00001f : ray.direction.z);
	float t1 = (min.x - ray.origin.x) * idx;
	float t2 = (max.x - ray.origin.x) * idx;
	float t3 = (min.y - ray.origin.y) * idy;
	float t4 = (max.y - ray.origin.y) * idy;
	float t5 = (min.z - ray.origin.z) * idz;
	float t6 = (max.z - ray.origin.z) * idz;

	float tmin = fmaxf(
		fmaxf(
			fminf(t1, t2),
			fminf(t3, t4)
		),
		fminf(t5, t6)
	);
	float tmax = fminf(
		fminf(
			fmaxf(t1, t2),
			fmaxf(t3, t4)
		),
		fmaxf(t5, t6)
	);

	if (tmax < 0.0f)
		return -1;

	if (tmin > tmax)
		return -1;

	if (tmin < 0.0f)
		return tmax;

	return tmin;
}
float Raycast(const OBB& obb, const Ray& ray)
{
	const float* o = obb.orientation.asArray;
	const float* size = obb.size.asArray;
	vec3 X(o[0], o[1], o[2]);
	vec3 Y(o[3], o[4], o[5]);
	vec3 Z(o[6], o[7], o[8]);

	vec3 p = obb.position - ray.origin;

	vec3 f(
		Dot(X, ray.direction),
		Dot(Y, ray.direction),
		Dot(Z, ray.direction)
	);

	vec3 e(
		Dot(X, p),
		Dot(Y, p),
		Dot(Z, p)
	);

	float t[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	for (int i = 0; i < 3; ++i)
	{
		if (CMP(f[i], 0.0f))
		{
			if (-e[i] - size[i] > 0.0f || -e[i] + size[i] < 0.0f)
				return -1;
			f[i] = 0.00001f;
		}
		float inverse = 1.0f / f[i];
		t[i * 2 + 0] = (e[i] + size[i]) * inverse;
		t[i * 2 + 0] = (e[i] - size[i]) * inverse;
	}

	float tmin = fmaxf(
		fmaxf(
			fminf(t[1], t[2]),
			fminf(t[3], t[4])
		),
		fminf(t[5], t[6])
	);
	float tmax = fminf(
		fminf(
			fmaxf(t[1], t[2]),
			fmaxf(t[3], t[4])
		),
		fmaxf(t[5], t[6])
	);

	if (tmax < 0.0f)
		return -1;

	if (tmin > tmax)
		return -1;

	if (tmin < 0.0f)
		return tmax;

	return tmin;
}
float Raycast(const Plane& plane, const Ray& ray)
{
	float nd = Dot(ray.direction, plane.normal);
	if (nd >= 0.0f)
		return -1;
	float pn = Dot(ray.origin, plane.normal);
	float t = (plane.distance - pn) / nd;

	if (t >= 0.0f)
		return t;
	return -1;
}

bool Linetest(const Sphere& sphere, const Line& line)
{
	Point closest = ClosestPoint(line, sphere.position);
	return MagnitudeSq(sphere.position - closest) <= sphere.radius * sphere.radius;
}
bool Linetest(const AABB& aabb, const Line& line)
{
	Ray ray;
	ray.origin = line.start;
	ray.direction = Normalized(line.end - line.start);
	float t = Raycast(aabb, ray);
	return t >= 0.0f && t * t <= LenghtSq(line);
}
bool Linetest(const OBB& obb, const Line& line)
{
	Ray ray;
	ray.origin = line.start;
	ray.direction = Normalized(line.end - line.start);
	float t = Raycast(obb, ray);
	return t >= 0.0f && t * t <= LenghtSq(line);
}
bool Linetest(const Plane& plane, const Line& line)
{
	vec3 ab = line.end - line.start;

	float nA = Dot(plane.normal, line.start);
	float nAB = Dot(plane.normal, ab);

	if (CMP(nAB, 0.0f))
		return false;

	float t = (plane.distance - nA) / nAB;
	return t >= 0.0f && t <= 1.0f;
}

Plane FromTriangle(const Triangle& t)
{
	Plane result;
	result.normal = Normalized(Cross(t.b - t.a, t.c - t.a));
	result.distance = Dot(result.normal, t.a);
	return result;
}

bool PointInTriangle(const Point& p, const Triangle& t)
{
	vec3 a = t.a - p;
	vec3 b = t.b - p;
	vec3 c = t.c - p;
	vec3 normPBC = Cross(b, c);
	vec3 normPCA = Cross(c, a);
	vec3 normPAB = Cross(a, b);

	if (Dot(normPBC, normPCA) < 0.0f)
		return false;
	if (Dot(normPBC, normPAB) < 0.0f)
		return false;
	return true;
}
Point ClosestPoint(const Triangle& t, const Point& p)
{
	Plane plane = FromTriangle(t);
	Point closest = ClosestPoint(plane, p);
	if (PointInTriangle(closest, t))
		return closest;

	Point c1 = ClosestPoint(Line(t.a, t.b), p);
	Point c2 = ClosestPoint(Line(t.b, t.c), p);
	Point c3 = ClosestPoint(Line(t.c, t.a), p);

	float magSq1 = MagnitudeSq(p - c1);
	float magSq2 = MagnitudeSq(p - c2);
	float magSq3 = MagnitudeSq(p - c3);

	if (magSq1 < magSq2 && magSq1 < magSq3)
		return c1;
	if (magSq2 < magSq1 && magSq2 < magSq3)
		return c2;
	return c3;
}

Interval GetInterval(const Triangle& triangle, const vec3& axis)
{
	Interval result;
	result.min = result.max = Dot(axis, triangle.points[0]);

	for (int i = 1; i < 3; ++i)
	{
		float value = Dot(axis, triangle.points[i]);
		result.min = fminf(result.min, value);
		result.max = fmaxf(result.max, value);
	}

	return result;
}
bool OverlapOnAxis(const AABB& aabb, const Triangle& triangle, const vec3& axis)
{
	Interval a = GetInterval(aabb, axis);
	Interval b = GetInterval(triangle, axis);
	return OVERLAP(a.min, a.max, b.min, b.max);
}
bool OverlapOnAxis(const OBB& obb, const Triangle& triangle, const vec3& axis)
{
	Interval a = GetInterval(obb, axis);
	Interval b = GetInterval(triangle, axis);
	return OVERLAP(a.min, a.max, b.min, b.max);
}
bool OverlapOnAxis(const Triangle& t1, const Triangle& t2, const vec3& axis)
{
	Interval a = GetInterval(t1, axis);
	Interval b = GetInterval(t2, axis);
	return OVERLAP(a.min, a.max, b.min, b.max);
}

bool TriangleSphere(const Triangle& t, const Sphere& s)
{
	Point closest = ClosestPoint(t, s.position);
	return MagnitudeSq(closest - s.position) <= s.radius * s.radius;
}
bool TriangleAABB(const Triangle& t, const AABB& aabb)
{
	vec3 f0 = t.b - t.a;
	vec3 f1 = t.c - t.b;
	vec3 f2 = t.a - t.c;

	vec3 u0(1.0f, 0.0f, 0.0f);
	vec3 u1(0.0f, 1.0f, 0.0f);
	vec3 u2(0.0f, 0.0f, 1.0f);

	vec3 test[13] = {
		u0, u1, u2,
		Cross(f0, f1),
		Cross(u0, f0), Cross(u0, f1), Cross(u0, f2),
		Cross(u1, f0), Cross(u1, f1), Cross(u1, f2),
		Cross(u2, f0), Cross(u2, f1), Cross(u2, f2)
	};

	for (int i = 0; i < 13; ++i)
	{
		if (!OverlapOnAxis(aabb, t, test[i]))
			return false;
	}
	return true;
}
bool TriangleOBB(const Triangle& t, const OBB& obb)
{
	vec3 f0 = t.b - t.a;
	vec3 f1 = t.c - t.b;
	vec3 f2 = t.a - t.c;

	const float* orientation = obb.orientation.asArray;
	vec3 u0(orientation[0], orientation[1], orientation[2]);
	vec3 u1(orientation[3], orientation[4], orientation[5]);
	vec3 u2(orientation[6], orientation[7], orientation[8]);

	vec3 test[13] = {
		u0, u1, u2,
		Cross(f0, f1),
		Cross(u0, f0), Cross(u0, f1), Cross(u0, f2),
		Cross(u1, f0), Cross(u1, f1), Cross(u1, f2),
		Cross(u2, f0), Cross(u2, f1), Cross(u2, f2)
	};

	for (int i = 0; i < 13; ++i)
	{
		if (!OverlapOnAxis(obb, t, test[i]))
			return false;
	}
	return true;
}
bool TrianglePlane(const Triangle& t, const Plane& p)
{
	float side1 = PlaneEquation(t.a, p);
	float side2 = PlaneEquation(t.b, p);
	float side3 = PlaneEquation(t.b, p);

	if (CMP(side1, 0.0f) && CMP(side2, 0.0f) && CMP(side3, 0.0f))
		return true;

	if (side1 > 0.0f && side2 > 0.0f && side3 > 0.0f)
		return false;
	if (side1 < 0.0f && side2 < 0.0f && side3 < 0.0f)
		return false;
	return true;
}
bool TriangleTriangle(const Triangle& t1, const Triangle& t2)
{
	vec3 t1_f0 = t1.b - t1.a;
	vec3 t1_f1 = t1.c - t1.b;
	vec3 t1_f2 = t1.a - t1.c;

	vec3 t2_f0 = t2.b - t2.a;
	vec3 t2_f1 = t2.c - t2.b;
	vec3 t2_f2 = t2.a - t2.c;

	vec3 axisToTest[] = {
		Cross(t1_f0, t1_f1),
		Cross(t2_f0, t2_f1),
		Cross(t2_f0, t1_f0), Cross(t2_f0, t1_f1), Cross(t2_f0, t2_f2),
		Cross(t2_f1, t1_f0), Cross(t2_f1, t1_f1), Cross(t2_f1, t2_f2),
		Cross(t2_f2, t1_f0), Cross(t2_f2, t1_f1), Cross(t2_f2, t2_f2)
	};
	for (int i = 0; i < 11; ++i)
	{
		if (!OverlapOnAxis(t1, t2, axisToTest[i]))
			return false;
	}
	return true;
}

vec3 SatCrossEdge(const vec3& a, const vec3& b, const vec3& c, const vec3& d)
{
	vec3 ab = a - b;
	vec3 cd = c - d;
	vec3 result = Cross(ab, cd);

	if (!CMP(MagnitudeSq(result), 0.0f))
		return result; //Not paralel
	
	vec3 axis = Cross(ab, c - a);
	result = Cross(ab, axis);

	return result;
	//if (!CMP(MagnitudeSq(result), 0.0f))
	//	return result;

	//return vec3();
}
bool TriangleTriangleRobust(const Triangle& t1, const Triangle& t2)
{
	vec3 axisToTest[] = {
		SatCrossEdge(t1.a, t1.b, t1.b, t1.c), // T1 normal
		SatCrossEdge(t2.a, t2.b, t2.b, t2.c), // T2 normal

		SatCrossEdge(t2.a, t2.b, t1.a, t1.b),
		SatCrossEdge(t2.a, t2.b, t1.b, t1.c),
		SatCrossEdge(t2.a, t2.b, t1.c, t1.a),

		SatCrossEdge(t2.b, t2.c, t1.a, t1.b),
		SatCrossEdge(t2.b, t2.c, t1.b, t1.c),
		SatCrossEdge(t2.b, t2.c, t1.c, t1.a),

		SatCrossEdge(t2.c, t2.a, t1.a, t1.b),
		SatCrossEdge(t2.c, t2.a, t1.b, t1.c),
		SatCrossEdge(t2.c, t2.a, t1.c, t1.a)
	};

	for (int i = 0; i < 11; ++i)
		if (!OverlapOnAxis(t1, t2, axisToTest[i]))
			if (!CMP(MagnitudeSq(axisToTest[i]), 0.0f))
				return false;

	return true;
}

vec3 Barycentric(const Point& p, const Triangle& t)
{
	vec3 ap = p - t.a;
	vec3 bp = p - t.b;
	vec3 cp = p - t.c;

	vec3 ab = t.b - t.a;
	vec3 ac = t.c - t.a;
	vec3 bc = t.c - t.b;
	vec3 cb = t.b - t.c;
	vec3 ca = t.a - t.c;

	vec3 v = ab - Project(ab, cb);
	float a = 1.0f - (Dot(v, ap) / Dot(v, ab));

	v = bc - Project(bc, ac);
	float b = 1.0f - (Dot(v, bp) / Dot(v, bc));

	v = ca - Project(ca, ab);
	float c = 1.0f - (Dot(v, cp) / Dot(v, ca));

	return vec3(a, b, c);
}
float Raycast(const Triangle& triangle, const Ray& ray)
{
	Plane plane = FromTriangle(triangle);
	float t = Raycast(plane, ray);
	if (t < 0.0f)
		return t;

	Point result = ray.origin + ray.direction * t;

	vec3 barycentric = Barycentric(result, triangle);
	if (barycentric.x >= 0.0f && barycentric.x <= 1.0f &&
		barycentric.y >= 0.0f && barycentric.y <= 1.0f &&
		barycentric.z >= 0.0f && barycentric.z <= 1.0f)
		return t;
	return -1;
}
bool Linetest(const Triangle& triangle, const Line& line)
{
	Ray ray;
	ray.origin = line.start;
	ray.direction = Normalized(line.end - line.start);
	float t = Raycast(triangle, ray);
	return t >= 0.0f && t * t <= LenghtSq(line);
}

void AccelerateMesh(Mesh& mesh)
{
	if (mesh.accelerator != nullptr)
		return;
	vec3 min = mesh.vertices[0];
	vec3 max = mesh.vertices[0];
	for (int i = 1; i < mesh.numTriangles * 3; ++i)
	{
		min.x = fminf(mesh.vertices[i].x, min.x);
		min.y = fminf(mesh.vertices[i].y, min.y);
		min.z = fminf(mesh.vertices[i].z, min.z);
		max.x = fmaxf(mesh.vertices[i].x, max.x);
		max.y = fmaxf(mesh.vertices[i].y, max.y);
		max.z = fmaxf(mesh.vertices[i].z, max.z);
	}
	mesh.accelerator = new BVHNode();
	mesh.accelerator->bounds = FromMinMax(min, max);
	mesh.accelerator->triangles.resize(mesh.numTriangles);
	std::iota(mesh.accelerator->triangles.begin(), mesh.accelerator->triangles.end(), 0);

	SplitBVHNode(mesh.accelerator, mesh, 3);
}
void SplitBVHNode(BVHNode* node, const Mesh& model, int depth)
{
	if (depth-- == 0)
		return;
	if (node->children.empty())
	{
		if (!node->triangles.empty())
		{
			node->children.resize(8);
			vec3 c = node->bounds.position;
			vec3 e = node->bounds.size * 0.5f;

			node->children[0].bounds = AABB(c + vec3(-e.x, e.y, -e.z), e);
			node->children[1].bounds = AABB(c + vec3(e.x, e.y, -e.z), e);
			node->children[2].bounds = AABB(c + vec3(-e.x, e.y, e.z), e);
			node->children[3].bounds = AABB(c + vec3(e.x, e.y, e.z), e);
			node->children[4].bounds = AABB(c + vec3(-e.x, -e.y, -e.z), e);
			node->children[5].bounds = AABB(c + vec3(e.x, -e.y, -e.z), e);
			node->children[6].bounds = AABB(c + vec3(-e.x, -e.y, e.z), e);
			node->children[7].bounds = AABB(c + vec3(e.x, -e.y, e.z), e);
		}
	}

	if (node->children.size() > 0 && node->triangles.size() > 0)
	{
		for (auto it = node->children.begin(); it != node->children.end(); ++it)
		{
			(*it).triangles.clear();
			for (auto jt = node->triangles.begin(); jt != node->triangles.end(); ++jt)
			{
				Triangle t = model.triangles[(*jt)];
				if (TriangleAABB(t, (*it).bounds))
				{
					(*it).triangles.push_back((*jt));
				}
			}
			
			(*it).triangles.shrink_to_fit();
		}
	}

	node->triangles.clear();
	node->triangles.shrink_to_fit();

	for (auto it = node->children.begin(); it != node->children.end(); ++it)
	{
		SplitBVHNode(&(*it), model, depth);
	}
}
void FreeBVHNode(BVHNode* node)
{
	if (!node->children.empty())
	{
		for (auto it = node->children.begin(); it != node->children.end(); ++it)
		{
			FreeBVHNode(&(*it));
		}
		node->children.clear();
		node->children.shrink_to_fit();
	}
	if (!node->triangles.empty())
	{
		node->triangles.clear();
		node->triangles.shrink_to_fit();
	}
}

float MeshRay(const Mesh& mesh, const Ray& ray)
{
	if (mesh.accelerator == nullptr)
	{
		for (int i = 0; i < mesh.numTriangles; ++i)
		{
			float result = Raycast(mesh.triangles[i], ray);
			if (result >= 0.0f) return result;
		}
	}
	else
	{
		std::list<BVHNode*> toProcess;
		toProcess.push_front(mesh.accelerator);
		while (!toProcess.empty())
		{
			auto iterator = *(toProcess.begin());
			toProcess.erase(toProcess.begin());
			if (!iterator->triangles.empty())
			{
				for (auto it = iterator->triangles.begin(); it != iterator->triangles.end(); ++it)
				{
					float r = Raycast(mesh.triangles[(*it)], ray);
					if (r >= 0.0f) return r;
				}
			}

			if (!iterator->children.empty())
			{
				for (auto it = iterator->children.rbegin(); it != iterator->children.rend(); ++it)
				{
					if (Raycast((*it).bounds, ray) >= 0.0f)
						toProcess.push_front(&(*it));
				}
			}
		}
	}
	return -1;
}

bool MeshSphere(const Mesh& mesh, const Sphere& sphere)
{
	if (mesh.accelerator == nullptr)
	{
		for (int i = 0; i < mesh.numTriangles; ++i)
			if (TriangleSphere(mesh.triangles[i], sphere))
				return true;
	}
	else
	{
		std::list<BVHNode*> toProcess;
		toProcess.push_front(mesh.accelerator);
		while (!toProcess.empty())
		{
			BVHNode* iterator = *(toProcess.begin());
			toProcess.erase(toProcess.begin());

			if (!iterator->triangles.empty())
			{
				for (auto it = iterator->triangles.begin(); it != iterator->triangles.end(); ++it)
				{
					if (TriangleSphere(mesh.triangles[(*it)], sphere))
						return true;
				}
			}

			if (!iterator->children.empty())
			{
				for (auto it = iterator->children.rbegin(); it != iterator->children.rend(); ++it)
				{
					if (AABBSphere((*it).bounds, sphere))
						toProcess.push_front(&(*it));
				}
			}
		}
	}
	return false;
}
bool MeshAABB(const Mesh& mesh, const AABB& aabb)
{
	if (mesh.accelerator == nullptr)
	{
		for (int i = 0; i < mesh.numTriangles; ++i)
			if (TriangleAABB(mesh.triangles[i], aabb))
				return true;
	}
	else
	{
		std::list<BVHNode*> toProcess;
		toProcess.push_front(mesh.accelerator);
		while (!toProcess.empty())
		{
			BVHNode* iterator = *(toProcess.begin());
			toProcess.erase(toProcess.begin());

			if (!iterator->triangles.empty())
			{
				for (auto it = iterator->triangles.begin(); it != iterator->triangles.end(); ++it)
				{
					if (TriangleAABB(mesh.triangles[(*it)], aabb))
						return true;
				}
			}

			if (!iterator->children.empty())
			{
				for (auto it = iterator->children.rbegin(); it != iterator->children.rend(); ++it)
				{
					if (AABBAABB((*it).bounds, aabb))
						toProcess.push_front(&(*it));
				}
			}
		}
	}
	return false;
}
bool MeshOBB(const Mesh& mesh, const OBB& obb)
{
	if (mesh.accelerator == nullptr)
	{
		for (int i = 0; i < mesh.numTriangles; ++i)
			if (TriangleOBB(mesh.triangles[i], obb))
				return true;
	}
	else
	{
		std::list<BVHNode*> toProcess;
		toProcess.push_front(mesh.accelerator);
		while (!toProcess.empty())
		{
			BVHNode* iterator = *(toProcess.begin());
			toProcess.erase(toProcess.begin());

			if (!iterator->triangles.empty())
			{
				for (auto it = iterator->triangles.begin(); it != iterator->triangles.end(); ++it)
				{
					if (TriangleOBB(mesh.triangles[(*it)], obb))
						return true;
				}
			}

			if (!iterator->children.empty())
			{
				for (auto it = iterator->children.rbegin(); it != iterator->children.rend(); ++it)
				{
					if (AABBOBB((*it).bounds, obb))
						toProcess.push_front(&(*it));
				}
			}
		}
	}
	return false;
}
bool MeshPlane(const Mesh& mesh, const Plane& plane)
{
	if (mesh.accelerator == nullptr)
	{
		for (int i = 0; i < mesh.numTriangles; ++i)
			if (TrianglePlane(mesh.triangles[i], plane))
				return true;
	}
	else
	{
		std::list<BVHNode*> toProcess;
		toProcess.push_front(mesh.accelerator);
		while (!toProcess.empty())
		{
			BVHNode* iterator = *(toProcess.begin());
			toProcess.erase(toProcess.begin());

			if (!iterator->triangles.empty())
			{
				for (auto it = iterator->triangles.begin(); it != iterator->triangles.end(); ++it)
				{
					if (TrianglePlane(mesh.triangles[(*it)], plane))
						return true;
				}
			}

			if (!iterator->children.empty())
			{
				for (auto it = iterator->children.rbegin(); it != iterator->children.rend(); ++it)
				{
					if (AABBPlane((*it).bounds, plane))
						toProcess.push_front(&(*it));
				}
			}
		}
	}
	return false;
}
bool MeshTriangle(const Mesh& mesh, const Triangle& triangle)
{
	if (mesh.accelerator == nullptr)
	{
		for (int i = 0; i < mesh.numTriangles; ++i)
			if (TriangleTriangle(mesh.triangles[i], triangle))
				return true;
	}
	else
	{
		std::list<BVHNode*> toProcess;
		toProcess.push_front(mesh.accelerator);
		while (!toProcess.empty())
		{
			BVHNode* iterator = *(toProcess.begin());
			toProcess.erase(toProcess.begin());

			if (!iterator->triangles.empty())
			{
				for (auto it = iterator->triangles.begin(); it != iterator->triangles.end(); ++it)
				{
					if (TriangleTriangle(mesh.triangles[(*it)], triangle))
						return true;
				}
			}

			if (!iterator->children.empty())
			{
				for (auto it = iterator->children.rbegin(); it != iterator->children.rend(); ++it)
				{
					if (TriangleAABB(triangle, (*it).bounds))
						toProcess.push_front(&(*it));
				}
			}
		}
	}
	return false;
}