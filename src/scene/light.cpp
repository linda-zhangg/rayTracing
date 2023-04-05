
// glm
#include <glm/gtc/constants.hpp>

// project
#include "light.hpp"

using namespace glm;


bool DirectionalLight::occluded(Scene *scene, const vec3 &point) const {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Determine whether the given point is being occluded from
	// this directional light by an object in the scene.
	// Remember that directional lights are "infinitely" far away
	// so any object in the way would cause an occlusion.
	//-------------------------------------------------------------

    Ray ray = Ray(point, -m_direction);
    return scene->intersect(ray).m_valid;
}


vec3 DirectionalLight::incidentDirection(const vec3 &) const {
	return m_direction;
}


vec3 DirectionalLight::irradiance(const vec3 &) const {
	return m_irradiance;
}


bool PointLight::occluded(Scene *scene, const vec3 &point) const {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Determine whether the given point is being occluded from
	// this point light by an object in the scene.
	// Remember that point lights are somewhere in the scene and
	// an occulsion has to occur somewhere between the light and 
	// the given point.
	//-------------------------------------------------------------
//    vec3 dir = -incidentDirection(point);
//    float len = length(dir);
//    if(len > 0) dir = normalize(dir);
//    Ray ray = Ray(point, dir);
//    RayIntersection intersect = scene->intersect(ray);
//    return intersect.m_valid && intersect.m_distance < len;
    if(length(point - m_position) > 0){
        Ray ray = Ray(point, normalize(m_position - point));
        if(scene->intersect(ray).m_valid && scene->intersect(ray).m_distance < distance(m_position, point)){
            return true;
        }
    }
    return false;
}


vec3 PointLight::incidentDirection(const vec3 &point) const {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Return the direction of the incoming light (light to point)
	//-------------------------------------------------------------
    if(length(point - m_position) > 0){
        return normalize(point - m_position);
    }
    return vec3(0);
    //return point - m_position;
}


vec3 PointLight::irradiance(const vec3 &point) const {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Return the total irradiance on the given point.
	// This requires you to convert the flux of the light into
	// irradiance by dividing it by the surface of the sphere
	// it illuminates. Remember that the surface area increases
	// as the sphere gets bigger, ie. the point is further away.
	//-------------------------------------------------------------

    if(length(m_position - point) > 0){
        return m_flux / (float)(4.0f * M_PI * length(m_position - point) * length(m_position - point));
    }
	return m_flux;
}
