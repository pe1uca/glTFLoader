#include "Box.h"

GLboolean Box::interect(Ray ray) const
{
	switch (ray.type)
	{
	case MMM:
		return !(ray.origin.x < bounds[0].x ||
				ray.origin.y < bounds[0].y ||
				ray.origin.z < bounds[0].z ||
				ray.sXY * bounds[0].x - bounds[1].y + ray.cXY > 0 ||
				ray.sYX * bounds[0].y - bounds[1].x + ray.cYX > 0 ||
				ray.sZY * bounds[0].z - bounds[1].y + ray.cZY > 0 ||
				ray.sYZ * bounds[0].y - bounds[1].z + ray.cYZ > 0 ||
				ray.sXZ * bounds[0].x - bounds[1].z + ray.cXZ > 0 ||
				ray.sZX * bounds[0].z - bounds[1].x + ray.cZX > 0);
	case MMP:
		return !(ray.origin.x < bounds[0].x ||
				ray.origin.y < bounds[0].y ||
				ray.origin.z > bounds[1].z ||
				ray.sXY * bounds[0].x - bounds[1].y + ray.cXY > 0 ||
				ray.sYX * bounds[0].y - bounds[1].x + ray.cYX > 0 ||
				ray.sZY * bounds[1].z - bounds[1].y + ray.cZY > 0 ||
				ray.sYZ * bounds[0].y - bounds[0].z + ray.cYZ < 0 ||
				ray.sXZ * bounds[0].x - bounds[0].z + ray.cXZ < 0 ||
				ray.sZX * bounds[1].z - bounds[1].x + ray.cZX > 0);
	case MPM:
		return !(ray.origin.x < bounds[0].x ||
				ray.origin.y > bounds[1].y ||
				ray.origin.z < bounds[0].z ||
				ray.sXY * bounds[0].x - bounds[0].y + ray.cXY < 0 ||
				ray.sYX * bounds[1].y - bounds[1].x + ray.cYX > 0 ||
				ray.sZY * bounds[0].z - bounds[0].y + ray.cZY < 0 ||
				ray.sYZ * bounds[1].y - bounds[1].z + ray.cYZ > 0 ||
				ray.sXZ * bounds[0].x - bounds[1].z + ray.cXZ > 0 ||
				ray.sZX * bounds[0].z - bounds[1].x + ray.cZX > 0);
	case MPP:
		return !(ray.origin.x < bounds[0].x ||
				ray.origin.y > bounds[1].y ||
				ray.origin.z > bounds[1].z ||
				ray.sXY * bounds[0].x - bounds[0].y + ray.cXY < 0 ||
				ray.sYX * bounds[1].y - bounds[1].x + ray.cYX > 0 ||
				ray.sZY * bounds[1].z - bounds[0].y + ray.cZY < 0 ||
				ray.sYZ * bounds[1].y - bounds[0].z + ray.cYZ < 0 ||
				ray.sXZ * bounds[0].x - bounds[0].z + ray.cXZ < 0 ||
				ray.sZX * bounds[1].z - bounds[1].x + ray.cZX > 0);
	case PMM:
		return !(ray.origin.x > bounds[1].x ||
				ray.origin.y < bounds[0].y ||
				ray.origin.z < bounds[0].z ||
				ray.sXY * bounds[1].x - bounds[1].y + ray.cXY > 0 ||
				ray.sYX * bounds[0].y - bounds[0].x + ray.cYX < 0 ||
				ray.sZY * bounds[0].z - bounds[1].y + ray.cZY > 0 ||
				ray.sYZ * bounds[0].y - bounds[1].z + ray.cYZ > 0 ||
				ray.sXZ * bounds[1].x - bounds[1].z + ray.cXZ > 0 ||
				ray.sZX * bounds[0].z - bounds[0].x + ray.cZX < 0);
	case PMP:
		return !(ray.origin.x > bounds[1].x ||
				ray.origin.y < bounds[0].y ||
				ray.origin.z > bounds[1].z ||
				ray.sXY * bounds[1].x - bounds[1].y + ray.cXY > 0 ||
				ray.sYX * bounds[0].y - bounds[0].x + ray.cYX < 0 ||
				ray.sZY * bounds[1].z - bounds[1].y + ray.cZY > 0 ||
				ray.sYZ * bounds[0].y - bounds[0].z + ray.cYZ > 0 ||
				ray.sXZ * bounds[1].x - bounds[0].z + ray.cXZ > 0 ||
				ray.sZX * bounds[1].z - bounds[0].x + ray.cZX > 0);
	case PPM:
		return !(ray.origin.x > bounds[1].x ||
				ray.origin.y > bounds[1].y ||
				ray.origin.z < bounds[0].z ||
				ray.sXY * bounds[1].x - bounds[0].y + ray.cXY < 0 ||
				ray.sYX * bounds[1].y - bounds[0].x + ray.cYX < 0 ||
				ray.sZY * bounds[0].z - bounds[0].y + ray.cZY < 0 ||
				ray.sYZ * bounds[1].y - bounds[1].z + ray.cYZ > 0 ||
				ray.sXZ * bounds[1].x - bounds[1].z + ray.cXZ > 0 ||
				ray.sZX * bounds[0].z - bounds[0].x + ray.cZX < 0);
	case PPP:
		return !(ray.origin.x > bounds[1].x ||
				ray.origin.y > bounds[1].y ||
				ray.origin.z > bounds[1].z ||
				ray.sXY * bounds[1].x - bounds[1].y + ray.cXY < 0 ||
				ray.sYX * bounds[1].y - bounds[1].x + ray.cYX < 0 ||
				ray.sZY * bounds[1].z - bounds[1].y + ray.cZY < 0 ||
				ray.sYZ * bounds[1].y - bounds[1].z + ray.cYZ < 0 ||
				ray.sXZ * bounds[1].x - bounds[1].z + ray.cXZ < 0 ||
				ray.sZX * bounds[1].z - bounds[1].x + ray.cZX < 0);
	case MOO:
		return !(ray.origin.x < bounds[0].x ||
				ray.origin.y < bounds[0].y ||
				ray.origin.y > bounds[1].y ||
				ray.origin.z < bounds[0].z ||
				ray.origin.z > bounds[1].z);
	case POO:
		return !(ray.origin.x > bounds[1].x ||
				ray.origin.y < bounds[0].y ||
				ray.origin.y > bounds[1].y ||
				ray.origin.z < bounds[0].z ||
				ray.origin.z > bounds[1].z);
	case OMO:
		return !(ray.origin.y < bounds[0].y ||
				ray.origin.x < bounds[0].x ||
				ray.origin.x > bounds[1].x ||
				ray.origin.z < bounds[0].z ||
				ray.origin.z > bounds[1].z);
	case OPO:
		return !(ray.origin.y > bounds[1].y ||
				ray.origin.x < bounds[0].x ||
				ray.origin.x > bounds[1].x ||
				ray.origin.z < bounds[0].z ||
				ray.origin.z > bounds[1].z);
	case OOM:
		return !(ray.origin.z < bounds[0].z ||
				ray.origin.x < bounds[0].x ||
				ray.origin.x > bounds[1].x ||
				ray.origin.y < bounds[0].y ||
				ray.origin.y > bounds[1].y);
	case OOP:
		return !(ray.origin.z > bounds[1].z ||
				ray.origin.x < bounds[0].x ||
				ray.origin.x > bounds[1].x ||
				ray.origin.y < bounds[0].y ||
				ray.origin.y > bounds[1].y);
	case MMO:
		return !(ray.origin.z < bounds[0].z ||
				ray.origin.z > bounds[1].z ||
				ray.origin.x < bounds[0].x ||
				ray.origin.y < bounds[0].y ||
				ray.sXY * bounds[0].x - bounds[1].y + ray.cXY > 0 ||
				ray.cYX * bounds[0].y - bounds[1].x + ray.cYX > 0);
	case MPO:
		return !(ray.origin.z < bounds[0].z ||
				ray.origin.z > bounds[1].z ||
				ray.origin.x < bounds[0].x ||
				ray.origin.y > bounds[1].y ||
				ray.sXY * bounds[0].x - bounds[0].y + ray.cXY < 0 ||
				ray.cYX * bounds[1].y - bounds[1].x + ray.cYX > 0);
	case PMO:
		return !(ray.origin.z < bounds[0].z ||
				ray.origin.z > bounds[1].z ||
				ray.origin.x > bounds[1].x ||
				ray.origin.y < bounds[0].y ||
				ray.sXY * bounds[1].x - bounds[1].y + ray.cXY > 0 ||
				ray.cYX * bounds[0].y - bounds[0].x + ray.cYX < 0);
	case PPO:
		return !(ray.origin.z < bounds[0].z ||
				ray.origin.z > bounds[1].z ||
				ray.origin.x > bounds[1].x ||
				ray.origin.y < bounds[0].y ||
				ray.sXY * bounds[1].x - bounds[0].y + ray.cXY < 0 ||
				ray.cYX * bounds[1].y - bounds[0].x + ray.cYX < 0);
	case MOM:
		return !(ray.origin.y < bounds[0].y ||
				ray.origin.y > bounds[1].y ||
				ray.origin.x < bounds[0].x ||
				ray.origin.z < bounds[0].z ||
				ray.sXZ * bounds[0].x - bounds[1].z + ray.cXZ > 0 ||
				ray.cZX * bounds[0].z - bounds[1].x + ray.cZX > 0);
	case MOP:
		return !(ray.origin.y < bounds[0].y ||
				ray.origin.y > bounds[1].y ||
				ray.origin.x < bounds[0].x ||
				ray.origin.z > bounds[1].z ||
				ray.sXZ * bounds[0].x - bounds[0].z + ray.cXZ < 0 ||
				ray.cZX * bounds[1].z - bounds[1].x + ray.cZX > 0);
	case POM:
		return !(ray.origin.y < bounds[0].y ||
				ray.origin.y > bounds[1].y ||
				ray.origin.x > bounds[1].x ||
				ray.origin.z < bounds[0].z ||
				ray.sXZ * bounds[1].x - bounds[1].z + ray.cXZ > 0 ||
				ray.cZX * bounds[0].z - bounds[0].x + ray.cZX < 0);
	case POP:
		return !(ray.origin.y < bounds[0].y ||
				ray.origin.y > bounds[1].y ||
				ray.origin.x > bounds[1].x ||
				ray.origin.z > bounds[1].z ||
				ray.sXZ * bounds[1].x - bounds[0].z + ray.cXZ < 0 ||
				ray.cZX * bounds[1].z - bounds[0].x + ray.cZX < 0);
	case OMM:
		return !(ray.origin.x < bounds[0].x ||
				ray.origin.x > bounds[1].x ||
				ray.origin.y < bounds[0].y ||
				ray.origin.z < bounds[0].z ||
				ray.sZY * bounds[0].z - bounds[1].y + ray.cZY > 0 ||
				ray.cYZ * bounds[0].y - bounds[1].z + ray.cYZ > 0);
	case OMP:
		return !(ray.origin.x < bounds[0].x ||
				ray.origin.x > bounds[1].x ||
				ray.origin.y < bounds[0].y ||
				ray.origin.z > bounds[1].z ||
				ray.sZY * bounds[1].z - bounds[1].y + ray.cZY > 0 ||
				ray.cYZ * bounds[0].y - bounds[0].z + ray.cYZ < 0);
	case OPM:
		return !(ray.origin.x < bounds[0].x ||
				ray.origin.x > bounds[1].x ||
				ray.origin.y > bounds[1].y ||
				ray.origin.z < bounds[0].z ||
				ray.sZY * bounds[0].z - bounds[1].y + ray.cZY > 0 ||
				ray.cYZ * bounds[0].y - bounds[1].z + ray.cYZ > 0);
	case OPP:
		return !(ray.origin.x < bounds[0].x ||
				ray.origin.x > bounds[1].x ||
				ray.origin.y > bounds[1].y ||
				ray.origin.z > bounds[1].z ||
				ray.sZY * bounds[1].z - bounds[0].y + ray.cZY < 0 ||
				ray.cYZ * bounds[1].y - bounds[0].z + ray.cYZ < 0);
	default:
		return GL_FALSE;
		break;
	}

	return GL_FALSE;
}