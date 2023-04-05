
#include <iostream>

// glm
#include <glm/gtc/matrix_transform.hpp>

// project
#include "camera.hpp"
#include "opengl.hpp"


using namespace std;
using namespace glm;


void Camera::setPositionOrientation(const vec3 &pos, float yaw, float pitch) {
	m_position = pos;
	m_yaw = yaw;
	m_pitch = pitch;

	// update rotation matrix (based on yaw and pitch)
	m_rotation = rotate(mat4(1), m_yaw, vec3(0, 1, 0)) * rotate(mat4(1), m_pitch, vec3(1, 0, 0));
}


Ray Camera::generateRay(const vec2 &pixel) {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Generate a ray in the scene using the camera position,
	// rotation field of view on the y axis (fovy) and the image
	// size. The pixel is given in image coordinates [0, imagesize]
	// This COULD be done by first creating the ray in ViewSpace
	// then transforming it by the position and rotation to get
	// it into worldspace.
	//-------------------------------------------------------------
    
    Ray ray;
    
    float imageAspectRatio = m_image_size.x / m_image_size.y;
    
    float px = (2 * ((pixel.x + 0.5) / m_image_size.x) - 1) * imageAspectRatio * tan(m_fovy / 2);
    float py = (2 * ((pixel.y + 0.5) / m_image_size.y) - 1) * tan(m_fovy / 2);
    vec3 pCameraSpace = vec3(px, py, -1);
    
    vec3 rayDirection = normalize(pCameraSpace - m_position);
    rayDirection = vec4(rayDirection, 0) * m_rotation;
    
    ray.origin = m_position;
    ray.direction = rayDirection;
    
	return ray;
}
