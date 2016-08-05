#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

#include "quaternion.hpp"

// Returns a quaternion such that q*start = dest
quat rotationBetweenVectors(vec3 start, vec3 dest) {
	start = normalize(start);
	dest = normalize(dest);

	float cos_theta = dot(start, dest);
	vec3 rotation_axis;

	if (cos_theta < -1 + 0.001f) {
		rotation_axis = cross(vec3(0.0f, 0.0f, 1.0f), start);
		if (length2(rotation_axis) < 0.01 )
			rotation_axis = cross(vec3(1.0f, 0.0f, 0.0f), start);

		rotation_axis = normalize(rotation_axis);
		return angleAxis(180.0f, rotation_axis);
	}

	rotation_axis = cross(start, dest);

	float s = sqrt( (1 + cos_theta) * 2 );
	float invs = 1 / s;

	return quat(
	           s * 0.5f,
	           rotation_axis.x * invs,
	           rotation_axis.y * invs,
	           rotation_axis.z * invs
	       );
}


quat lookAt(vec3 direction, vec3 up) {
	if (length2(direction) < 0.0001f )
		return quat();

	vec3 right = cross(direction, up);
	up = cross(right, direction);

	quat rot1 = rotationBetweenVectors(vec3(0.0f, 0.0f, 1.0f), direction);
	vec3 new_up = rot1 * vec3(0.0f, 1.0f, 0.0f);
	quat rot2 = rotationBetweenVectors(new_up, up);

	return rot2 * rot1;
}

quat rotateTowards(quat q1, quat q2, float max_angle) {
	if ( max_angle < 0.001f ) {
		return q1;
	}

	float cos_theta = dot(q1, q2);

	if (cos_theta > 0.9999f) {
		return q2;
	}

	if (cos_theta < 0) {
		q1 = q1 * -1.0f;
		cos_theta *= -1.0f;
	}

	float angle = acos(cos_theta);

	if (angle < max_angle) {
		return q2;
	}

	float t = max_angle / angle;
	angle = max_angle;
	quat res = (sin((1.0f - t) * angle) * q1 + sin(t * angle) * q2) / sin(angle);
	res = normalize(res);

	return res;
}