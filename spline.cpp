//----------------------------------------------------------------------------------------
/**
 * \file    spline.cpp
 * \author  veverlu4
 * \date    2020
 * \brief   Managing catmull rom spline
 */
 //----------------------------------------------------------------------------------------
#include "spline.h"


bool isVectorNull(const glm::vec3& vect) {

	return !vect.x && !vect.y && !vect.z;
}

glm::mat4 alignObject(const glm::vec3& position, const glm::vec3& front, const glm::vec3& up) {

	glm::vec3 z = -glm::normalize(front);

	if (isVectorNull(z))
		z = glm::vec3(0.0, 0.0, 1.0);

	glm::vec3 x = glm::normalize(glm::cross(up, z));

	if (isVectorNull(x))
		x = glm::vec3(1.0, 0.0, 0.0);

	glm::vec3 y = glm::cross(z, x);
	//mat4 matrix = mat4(1.0f);
	glm::mat4 matrix = glm::mat4(
		x.x, x.y, x.z, 0.0,
		y.x, y.y, y.z, 0.0,
		z.x, z.y, z.z, 0.0,
		position.x, position.y, position.z, 1.0
	);

	return matrix;
}

const size_t  curveSize = 13;

glm::vec3 curveData[] = {
	glm::vec3(-2.00,-0.03, 0.0),
	glm::vec3(1.35,0.05,0.0),
	glm::vec3(5.93,-1.08,0.0),
	glm::vec3(2.69,-2.69,0.0),
	glm::vec3(1.0,-3.41,0.0),
	glm::vec3(-0.2,-2.0,0.0),
	glm::vec3(1.12,-1.5,0.0),
	glm::vec3(1.31,-2.0,0.0),
	glm::vec3(0.0,-4.33,0.0),
	glm::vec3(-0.80,-7.48,0.0),
	glm::vec3(0.88,-9.40,0.0),
	glm::vec3(3.87,-8.67,0.0),
	glm::vec3(2.72,-3.97,0.0),

};

glm::vec3 evaluateCurveSegment(
	const glm::vec3& p0,
	const glm::vec3& p1,
	const glm::vec3& p2,
	const glm::vec3& p3,
	const float t
) {
	glm::vec3 firstSeg = p0 * (t*((2-t)*t-1));
	glm::vec3 secSeg = p1 * (t * t * (3 * t - 5) + 2);
	glm::vec3 thirdSeg = p2 * (t * ((4 - 3 * t) * t + 1));
	glm::vec3 fourthSeg = p3 * ((t-1)*t*t);

	glm::vec3 result = firstSeg + secSeg+thirdSeg+fourthSeg;
	result *= 0.5;
	return result;
}

glm::vec3 evaluateCurveSegment_1stDerivative(
	const glm::vec3& p0,
	const glm::vec3& p1,
	const glm::vec3& p2,
	const glm::vec3& p3,
	const float t
) {
	glm::vec3 firstSeg = p0 * (-3.0f * t*t + 4.0f * t - 1.0f);
	glm::vec3 secSeg = p1 * (9.0f * t*t - 10.0f * t);
	glm::vec3 thirdSeg = p2 * (-9.0f * t*t + 8.0f * t + 1.0f);
	glm::vec3 fourthSeg = p3 * (3.0f * t*t - 2.0f * t);
	
	return 0.5f*(firstSeg+secSeg+thirdSeg+fourthSeg);
}

glm::vec3 evaluateClosedCurve(
	const glm::vec3 points[],
	const size_t    count,
	const float     t
) {
	glm::vec3 result(0.0, 0.0, 0.0);

	// based on the value of parameter t first find corresponding segment and its control points => i
	// and then call evaluateCurveSegment function with proper parameters to get a point on a closed curve

	float param = cyclic_clamp(t, 0.0f, float(count));
	size_t index = size_t(param);

	result = evaluateCurveSegment(
		points[(index - 1 + count) % count],
		points[(index) % count],
		points[(index + 1) % count],
		points[(index + 2) % count],
		param - floor(param)
	);

	return result;
}

glm::vec3 evaluateClosedCurve_1stDerivative(
	const glm::vec3 points[],
	const size_t    count,
	const float     t
) {
	glm::vec3 result(1.0, 0.0, 0.0);

	float param = cyclic_clamp(t, 0.0f, float(count));
	size_t index = size_t(param);

	result = evaluateCurveSegment_1stDerivative(
		points[(index - 1 + count) % count],
		points[(index) % count],
		points[(index + 1) % count],
		points[(index + 2) % count],
		param - floor(param)
	);

	return result;
}




