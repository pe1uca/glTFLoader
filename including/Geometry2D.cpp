#include "Geometry2D.h"
#include "matrices.h"
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

#define OVERLAP(aMin, aMax, bMin, bMax) ((bMin <= aMax) && (aMin <= bMax))

float Lenght(const Line2D& line)
{
	return Magnitude(line.end - line.start);
}
float LenghtSq(const Line2D& line)
{
	return MagnitudeSq(line.end - line.start);
}

vec2 GetMin(const Rectangle2D& rect)
{
	vec2 p1 = rect.origin;
	vec2 p2 = rect.origin + rect.size;
	return vec2(fminf(p1.x, p2.x), fminf(p1.y, p2.y));
}
vec2 GetMax(const Rectangle2D& rect)
{
	vec2 p1 = rect.origin;
	vec2 p2 = rect.origin + rect.size;
	return vec2(fmaxf(p1.x, p2.x), fmaxf(p1.y, p2.y));
}
Rectangle2D FromMinMax(const vec2& min, const vec2& max)
{
	return Rectangle2D(min, max - min);
}

bool PointOnLine(const Point2D& point, const Line2D& line)
{
	// Find the slope
	float dy = (line.end.y - line.start.y);
	float dx = (line.end.x - line.start.x);
	float M = dy / dx;
	// Find the Y-intercept
	float B = line.start.y - M * line.start.x;
	// Check line equation
	return CMP(point.y, M * point.x + B);
}
bool PointInCircle(const Point2D& point, const Circle& c)
{
	Line2D line(point, c.position);
	return LenghtSq(line) < c.radius * c.radius;
}
bool PointInRectangle(const Point2D& point, const Rectangle2D& rectangle)
{
	vec2 min = GetMin(rectangle);
	vec2 max = GetMax(rectangle);
	return min.x <= point.x &&
		min.y <= point.y &&
		point.x <= max.x &&
		point.y <= max.y;
}
bool PointInOrientedRectangle(const Point2D& point, const OrientedRectangle& rectangle)
{
	vec2 rotVector = point - rectangle.position;
	float theta = -DEG2RAD(rectangle.rotation);
	float zRotation2x2[] = {
		cosf(theta), sinf(theta),
		-sinf(theta), cosf(theta)
	};
	Multiply(rotVector.asArray,
		vec2(rotVector.x, rotVector.y).asArray,
		1, 2, zRotation2x2, 2, 2);
	Rectangle2D localRectangle(Point2D(),
		rectangle.halfExtents * 2.0f);
	vec2 localPoint = rotVector + rectangle.halfExtents;
	return PointInRectangle(localPoint, localRectangle);
}

bool LineCircle(const Line2D& line, const Circle& circle)
{
	vec2 ab = line.end - line.start;
	float t = Dot(circle.position - line.start, ab) / Dot(ab, ab);
	if (t < 0.0f || t > 1.0f)
		return false;
	Point2D closestPoint = line.start + ab * t;
	Line2D circleToClosest(circle.position, closestPoint);
	return LenghtSq(circleToClosest) < circle.radius * circle.radius;
}
bool LineRectangle(const Line2D& l, const Rectangle2D& r)
{
	if (PointInRectangle(l.start, r) || PointInRectangle(l.end, r))
		return true;
	vec2 norm = Normalized(l.end - l.start);
	norm.x = (norm.x != 0) ? 1.0f / norm.x : 0;
	norm.y = (norm.y != 0) ? 1.0f / norm.y : 0;
	vec2 min = (GetMin(r) - l.start) * norm;
	vec2 max = (GetMax(r) - l.start) * norm;
	
	float tmin = fmaxf(
		fminf(min.x, max.x),
		fmin(min.y, max.y)
	);
	float tmax = fminf(
		fmaxf(min.x, max.x),
		fmaxf(min.y, max.y)
	);
	if (tmax < 0 || tmin > tmax)
		return false;
	float t = (tmin < 0.0f) ? tmax : tmin;
	return t > 0.0f && t * t < LenghtSq(l);
}
bool LineOrientedRectangle(const Line2D& line, const OrientedRectangle& rectangle)
{
	float theta = -DEG2RAD(rectangle.rotation);
	float zRotation2x2[]{
		cosf(theta), sinf(theta),
		-sinf(theta), cosf(theta)
	};
	Line2D localLine;

	vec2 rotVector = line.start - rectangle.position;
	Multiply(rotVector.asArray,
		vec2(rotVector.x, rotVector.y).asArray,
		1, 2, zRotation2x2, 2, 2);
	localLine.start = rotVector + rectangle.halfExtents;

	rotVector = line.end - rectangle.position;
	Multiply(rotVector.asArray,
		vec2(rotVector.x, rotVector.y).asArray,
		1, 2, zRotation2x2, 2, 2);
	localLine.end = rotVector + rectangle.halfExtents;

	Rectangle2D localRectangle(Point2D(),
		rectangle.halfExtents * 2.0f);
	return LineRectangle(localLine, localRectangle);
}

bool CircleCircle(const Circle& c1, const Circle& c2)
{
	Line2D line(c1.position, c2.position);
	float raddiSum = c1.radius + c2.radius;
	return LenghtSq(line) <= raddiSum * raddiSum;
}
bool CircleRectangle(const Circle& circle, const Rectangle2D& rectangle)
{
	vec2 min = GetMin(rectangle);
	vec2 max = GetMax(rectangle);

	Point2D closestPoint = circle.position;
	CLAMP(closestPoint.x, min.x, max.x);
	CLAMP(closestPoint.y, min.y, max.y);

	Line2D line(circle.position, closestPoint);
	return LenghtSq(line) <= circle.radius * circle.radius;
}
bool CircleOrientedRectangle(const Circle& circle, const OrientedRectangle& rectangle)
{
	vec2 r = circle.position - rectangle.position;
	float theta = -DEG2RAD(rectangle.rotation);
	float zRotation2x2[] = {
		cosf(theta), sinf(theta),
		-sinf(theta), cosf(theta)
	};
	Multiply(r.asArray, vec2(r.x, r.y).asArray,
		1, 2, zRotation2x2, 2, 2);
	Circle lCircle(r + rectangle.halfExtents, circle.radius);
	Rectangle2D lRect(Point2D(), rectangle.halfExtents * 2.0f);
	return CircleRectangle(lCircle, lRect);
}
bool RectangleRectangle(const Rectangle2D& rect1, const Rectangle2D& rect2)
{
	vec2 aMin = GetMin(rect1);
	vec2 aMax = GetMax(rect1);

	vec2 bMin = GetMin(rect2);
	vec2 bMax = GetMax(rect2);

	return OVERLAP(aMin.x, aMax.x, bMin.x, bMax.x) &&
		OVERLAP(aMin.y, aMax.y, bMin.y, bMax.y);
}

Interval2D GetInterval(const Rectangle2D& rect, const vec2& axis)
{
	Interval2D result;

	vec2 min = GetMin(rect);
	vec2 max = GetMax(rect);

	vec2 verts[] = {
		vec2(min.x, min.y), vec2(min.x, max.y),
		vec2(max.x, max.y), vec2(max.x, min.y)
	};
	
	result.min = result.max = Dot(axis, verts[0]);
	for (int i = 1; i < 4; ++i)
	{
		float projection = Dot(axis, verts[i]);
		if (projection < result.min)
			result.min = projection;
		if (projection > result.max)
			result.max = projection;
	}
	return result;
}
bool OverlapOnAxis(const Rectangle2D& rect1, const Rectangle2D& rect2, const vec2& axis)
{
	Interval2D a = GetInterval(rect1, axis);
	Interval2D b = GetInterval(rect2, axis);
	return OVERLAP(a.min, a.max, b.min, b.max);
}
bool RectangleRectangleSAT(const Rectangle2D& rect1, const Rectangle2D& rect2)
{
	vec2 axisToTest[] = { vec2(1, 0), vec2(0, 1) };
	for (int i = 0; i < 2; ++i)
	{
		if (!OverlapOnAxis(rect1, rect2, axisToTest[i]))
			return false;
	}
	return true;
}

Interval2D GetInterval(const OrientedRectangle& rect, const vec2& axis)
{
	Rectangle2D lr = Rectangle2D(
		Point2D(rect.position - rect.halfExtents),
		rect.halfExtents * 2.0f
	);
	vec2 min = GetMin(lr);
	vec2 max = GetMax(lr);
	vec2 verts[] = {
		min, max,
		vec2(min.x, max.y), vec2(max.x, min.y)
	};

	float theta = DEG2RAD(rect.rotation);
	float zRot[] = {
		cosf(theta), sinf(theta),
		-sinf(theta), cosf(theta)
	};
	for (int i = 0; i < 4; ++i)
	{
		vec2 r = verts[i] - rect.position;
		Multiply(r.asArray, vec2(r.x, r.y).asArray,
			1, 2, zRot, 2, 2);
		verts[i] = r + rect.position;
	}
	Interval2D res;
	res.min = res.max = Dot(axis, verts[0]);
	for (int i = 1; i < 4; ++i)
	{
		float proj = Dot(axis, verts[i]);
		if (proj < res.min)
			res.min = proj;
		if (proj > res.max)
			res.max = proj;
	}
	return res;
}
bool OverlapOnAxis(const Rectangle2D& rect1, const OrientedRectangle& rect2, const vec2& axis)
{
	Interval2D a = GetInterval(rect1, axis);
	Interval2D b = GetInterval(rect2, axis);
	return OVERLAP(a.min, a.max, b.min, b.max);
}
bool RectangleOrientedRectangle(const Rectangle2D& rect1, const OrientedRectangle& rect2)
{
	vec2 axisToTest[]{
		vec2(1, 0), vec2(0, 1),
		vec2(), vec2()
	};

	float theta = DEG2RAD(rect2.rotation);
	float zRot[] = {
		cosf(theta), sinf(theta),
		-sinf(theta), cosf(theta)
	};

	vec2 axis = Normalized(vec2(rect2.halfExtents.x, 0));
	Multiply(axisToTest[2].asArray,
		axis.asArray, 1, 2, zRot, 2, 2);
	axis = Normalized(vec2(0, rect2.halfExtents.y));
	Multiply(axisToTest[3].asArray,
		axis.asArray, 1, 2, zRot, 2, 2);
	for (int i = 0; i < 4; ++i)
	{
		if (!OverlapOnAxis(rect1, rect2, axisToTest[i]))
			return false;
	}
	return true;
}

bool OrientedRectangleOrientedRectangle(const OrientedRectangle& r1, const OrientedRectangle& r2)
{
	Rectangle2D local1(Point2D(), r1.halfExtents* 2.0f);
	vec2 r = r2.position - r1.position;
	OrientedRectangle local2(r2.position, r2.halfExtents, r2.rotation);
	local2.rotation = r2.rotation - r1.rotation;

	float theta = -DEG2RAD(r1.rotation);
	float zRot[] = {
		cosf(theta), sinf(theta),
		-sinf(theta), cosf(theta)
	};
	Multiply(r.asArray, vec2(r.x, r.y).asArray, 1, 2, zRot, 2, 2);
	local2.position = r + r1.halfExtents;
	return RectangleOrientedRectangle(local1, local2);
}

Circle ContainingCircle(std::vector<Point2D> points)
{
	Point2D center;
	for (auto it = points.begin(); it != points.end(); ++it)
	{
		center = center + *it;
	}
	center = center * (1.0f / (float)points.size());
	Circle result(center, 1.0f);
	result.radius = MagnitudeSq(center - points[0]);
	for (auto it = points.begin(); it != points.end(); ++it)
	{
		float distance = MagnitudeSq(center - *it);
		if (distance > result.radius)
			result.radius = distance;
	}
	result.radius = sqrtf(result.radius);
	return result;
}

Rectangle2D ContainingRectangle(std::vector<Point2D> points)
{
	vec2 min = points[0];
	vec2 max = points[0];
	for (auto it = points.begin(); it != points.end(); ++it)
	{
		min.x = (*it).x < min.x ? (*it).x : min.x;
		min.y = (*it).y < min.y ? (*it).y : min.y;
		max.x = (*it).x > max.x ? (*it).x : max.x;
		max.x = (*it).x > max.x ? (*it).x : max.x;
	}
	return FromMinMax(min, max);
}

bool PointInShape(const BoundingShape& shape, const Point2D& point)
{
	for (auto it = shape.circles.begin(); it != shape.circles.end(); ++it)
	{
		if (PointInCircle(point, *it))
			return true;
	}
	for (auto it = shape.rectangles.begin(); it != shape.rectangles.end(); ++it)
	{
		if (PointInRectangle(point, *it))
			return true;
	}
	return false;
}