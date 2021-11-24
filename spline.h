//----------------------------------------------------------------------------------------
/**
 * \file    spline.h
 * \author  veverlu4
 * \date    2020
 * \brief   Managing catmull rom spline
 */
 //----------------------------------------------------------------------------------------
#ifndef __SPLINE_H
#define __SPLINE_H

#include "pgr.h" // glm

bool isVectorNull(const glm::vec3& vect);

glm::vec3 evaluateCurveSegment(const glm::vec3& p0,const glm::vec3& p1,const glm::vec3& p2,const glm::vec3& p3,const float t);
glm::vec3 evaluateCurveSegment_1stDerivative(const glm::vec3& p0,const glm::vec3& p1,const glm::vec3& p2,const glm::vec3& p3,const float t);
glm::vec3 evaluateClosedCurve(const glm::vec3 points[], const size_t count, const float t);
glm::vec3 evaluateClosedCurve_1stDerivative(const glm::vec3 points[], const size_t count, const float t);



//**************************************************************************************************
/// Align (rotate and move) current coordinate system to given parameters.
/**
 This function works similarly to \ref gluLookAt, however it is used for object transform
 rather than for view transform. Therefore, this function does not compute the computed matrix inverse.
 The current coordinate system is moved in such a way that the origin is moved
 to the \a position. Object's local front (-Z) direction is rotated to the \a front and
 object's local up (+Y) direction is rotated so that angle between its local up direction and
 \a up vector is minimum.

 \param[in]  position           Position of the origin.
 \param[in]  front              Front direction.
 \param[in]  up                 Up vector.
 */
glm::mat4 alignObject(const glm::vec3& position, const glm::vec3& front, const glm::vec3& up);


extern glm::vec3 curveData[];
extern const size_t  curveSize;


//**************************************************************************************************
/// Cyclic clamping of a value.
/**
 Makes sure that \a value is not outside the interval [\a minBound, \a maxBound].
 If \a value is outside the interval it is treated as a periodic value with the period equal to the size
 of the interval. A necessary number of periods are added/subtracted to fit the value to the interval.

 \param[in]  value              Value to be clamped.
 \param[in]  minBound           Minimum bound of value.
 \param[in]  maxBound           Maximum bound of value.
 \return                        Value within range [minBound, maxBound].
 \pre                           \a minBound is not greater that \maxBound.
*/
template <typename T>
T cyclic_clamp(const T value, const T minBound, const T maxBound) {

	T amp = maxBound - minBound;
	T val = fmod(value - minBound, amp);

	if (val < T(0))
		val += amp;

	return val + minBound;
}


#endif // __SPLINE_H
