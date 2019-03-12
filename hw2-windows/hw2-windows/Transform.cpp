// Transform.cpp: implementation of the Transform class.

// Note: when you construct a matrix using mat4() or mat3(), it will be COLUMN-MAJOR
// Keep this in mind in readfile.cpp and display.cpp
// See FAQ for more details or if you're having problems.

#include "Transform.h"

// Helper rotation function.  Please implement this.  
mat3 Transform::rotate(const float degrees, const vec3& axis) 
{
	float radians = degrees * glm::pi<float>() / 180.0f;  //convert to radians
	mat3 left_mat = mat3(
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	);
	left_mat = glm::transpose(left_mat);
	mat3 middle_mat = mat3(
		axis.x*axis.x, axis.x*axis.y, axis.x*axis.z,
		axis.x*axis.y, axis.y*axis.y, axis.y*axis.z,
		axis.x*axis.z, axis.y*axis.z, axis.z*axis.z
	);
	middle_mat = glm::transpose(middle_mat);

	mat3 right_mat = mat3(
		0, -axis.z, axis.y,
		axis.z, 0, -axis.x,
		-axis.y, axis.x, 0
	);
	right_mat = glm::transpose(right_mat);

	mat3 rotate_mat = glm::cos(radians)*left_mat + (1 - glm::cos(radians))*middle_mat + glm::sin(radians)*right_mat;
	return rotate_mat;
}

void Transform::left(float degrees, vec3& eye, vec3& up) 
{
	eye = Transform::rotate(degrees, glm::normalize(up)) * eye;
}

void Transform::up(float degrees, vec3& eye, vec3& up) 
{
	vec3 rotate_axis = glm::cross(glm::normalize(up), glm::normalize(eye));
	eye = Transform::rotate(-degrees, rotate_axis) * eye;
	up = Transform::rotate(-degrees, rotate_axis) * up;
}

mat4 Transform::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) 
{
	vec3 w = glm::normalize(eye);
	vec3 u = glm::normalize(glm::cross(up, eye));
	vec3 v = glm::cross(w, u);

	mat4 viewing_mat = mat4(
		u.x, u.y, u.z, -u.x*eye.x - u.y*eye.y - u.z*eye.z,
		v.x, v.y, v.z, -v.x*eye.x - v.y*eye.y - v.z*eye.z,
		w.x, w.y, w.z, -w.x*eye.x - w.y*eye.y - w.z*eye.z,
		0, 0, 0, 1
	);
	viewing_mat = glm::transpose(viewing_mat);
	return viewing_mat;
}

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
{
	float theta = fovy * glm::pi<float>() / 2.0f / 180.0f;
	float d = 1.0f / glm::tan(theta);
	float a = -(zFar + zNear) / (zFar - zNear);
	float b = -2.0f *zFar * zNear / (zFar - zNear);
	mat4 perspective_mat = mat4(
		d / aspect, 0, 0, 0,
		0, d, 0, 0,
		0, 0, a, b,
		0, 0, -1, 0	
	);
	perspective_mat = glm::transpose(perspective_mat);
    return perspective_mat;
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) 
{
	mat4 scale_mat = mat4(
		sx, 0, 0, 0,
		0, sy, 0, 0,
		0, 0, sz, 0,
		0, 0, 0, 1
	);
	scale_mat = glm::transpose(scale_mat);
	return scale_mat;
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) 
{
	mat4 translate_mat = mat4(
		1, 0, 0, tx,
		0, 1, 0, ty,
		0, 0, 1, tz,
		0, 0, 0, 1
	);
	translate_mat = glm::transpose(translate_mat);
	return translate_mat;
}

// To normalize the up direction and construct a coordinate frame.  
// As discussed in the lecture.  May be relevant to create a properly 
// orthogonal and normalized up. 
// This function is provided as a helper, in case you want to use it. 
// Using this function (in readfile.cpp or display.cpp) is optional.  

vec3 Transform::upvector(const vec3 &up, const vec3 & zvec) 
{
    vec3 x = glm::cross(up,zvec); 
    vec3 y = glm::cross(zvec,x); 
    vec3 ret = glm::normalize(y); 
    return ret; 
}


Transform::Transform()
{

}

Transform::~Transform()
{

}
