#ifndef QUATERNION
#define QUATERNION

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
using namespace glm;

quat rotationBetweenVectors(vec3 start, vec3 dest);
quat lookAt(vec3 direction, vec3 up);
quat rotateTowards(quat q1, quat q2, float max_angle);

#endif