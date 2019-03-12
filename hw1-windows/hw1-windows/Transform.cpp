// Transform.cpp: implementation of the Transform class.

#include "Transform.h"

//Please implement the following functions:

// Helper rotation function.  
mat3 Transform::rotate(const float degrees, const vec3& axis) {
  // YOUR CODE FOR HW1 HERE
  float radians = degrees * pi / 180;  //convert to radians
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

  // You will change this return call
  //return mat3();
}

// Transforms the camera left around the "crystal ball" interface
void Transform::left(float degrees, vec3& eye, vec3& up) {
  // YOUR CODE FOR HW1 HERE
  eye = Transform::rotate(degrees, glm::normalize(up)) * eye;
}

// Transforms the camera up around the "crystal ball" interface
void Transform::up(float degrees, vec3& eye, vec3& up) {
  // YOUR CODE FOR HW1 HERE 
  vec3 rotate_axis = glm::cross(glm::normalize(up), glm::normalize(eye));
  eye = Transform::rotate(-degrees, rotate_axis) * eye;
  up = Transform::rotate(-degrees, rotate_axis) * up;
}

// Your implementation of the glm::lookAt matrix
mat4 Transform::lookAt(vec3 eye, vec3 up) {
  // YOUR CODE FOR HW1 HERE
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

Transform::Transform()
{

}

Transform::~Transform()
{

}