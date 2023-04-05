
// glm
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>

// std
#include <random>

// project
#include "scene.hpp"
#include "shape.hpp"
#include "light.hpp"
#include "material.hpp"
#include "path_tracer.hpp"


using namespace std;
using namespace glm;


vec3 SimplePathTracer::sampleRay(const Ray &ray, int) {
	// intersect ray with the scene
	RayIntersection intersect = m_scene->intersect(ray);

	// if ray hit something
	if (intersect.m_valid) {

		// simple grey shape shading
		float f = abs(dot(-ray.direction, intersect.m_normal));
		vec3 grey(0.5, 0.5, 0.5);
		return mix(grey / 2.0f, grey, f);
	}

	// no intersection - return background color
	return { 0.3f, 0.3f, 0.4f };
}



vec3 CorePathTracer::sampleRay(const Ray &ray, int) {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Implement a PathTracer that calculates the ambient, diffuse
	// and specular, for the given ray in the scene, using the 
	// Phong lighting model. Give special consideration to objects
	// that occluded from direct lighting (shadow rays). You do
	// not need to use the depth argument for this implementation.
	//-------------------------------------------------------------
    
    vec3 color = vec3(0, 0, 0);
    RayIntersection intersect = m_scene->intersect(ray);
    //vec3 specReflec = vec3(0);

    if(intersect.m_valid){
        for(unsigned long i = 0; i < m_scene->lights().size(); i++){
            std::shared_ptr<Light> light = m_scene->lights().at(i);
            vec3 diffuseElement = light->ambience() * intersect.m_material->diffuse();
            color += diffuseElement; //add color by diffuse element

            if(!light->occluded(m_scene, intersect.m_position)){ //if not occluded
                float angle = glm::max(0.0f,dot(-light->incidentDirection(intersect.m_position),intersect.m_normal));
                vec3 diffuseReflect = light->irradiance(intersect.m_position) * angle * intersect.m_material->diffuse();
                color += diffuseReflect; //add color by diffuse reflect
                
                vec3 reflect = glm::reflect(light->incidentDirection(intersect.m_position),intersect.m_normal);
                float angle2 = glm::max(0.0f,dot(normalize(reflect),normalize(-ray.direction)));
                float angle3 = pow(angle2,intersect.m_material->shininess());
                vec3 specReflect = light->irradiance(intersect.m_position) * angle3 * intersect.m_material->specular();
                //cout<<reflect.length()<<endl;
                color += specReflect; //add color by specular reflect
                
            }
        }
        return color;
    }
	return vec3(0.3f, 0.3f, 0.4f); //default background colour
}



vec3 CompletionPathTracer::sampleRay(const Ray &ray, int depth) {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Using the same requirements for the CorePathTracer add in 
	// a recursive element to calculate perfect specular reflection.
	// That is compute the reflection ray off your intersection and
	// sample a ray in that direction, using the result to additionally
	// light your object. To make this more realistic you may weight
	// the incoming light by the (1 - (1/shininess)).
	//-------------------------------------------------------------

    vec3 color = vec3(0, 0, 0);
    RayIntersection intersect = m_scene->intersect(ray);

    if(intersect.m_valid){
        for(unsigned long i = 0; i < m_scene->lights().size(); i++){
            std::shared_ptr<Light> light = m_scene->lights().at(i);
            vec3 diffuseElement = light->ambience() * intersect.m_material->diffuse();
            color += diffuseElement; //add color by diffuse element

            if(!light->occluded(m_scene, intersect.m_position)){ //if not occluded
                float angle = glm::max(0.0f,dot(-light->incidentDirection(intersect.m_position),intersect.m_normal));
                vec3 diffuseReflect = light->irradiance(intersect.m_position) * angle * intersect.m_material->diffuse();
                color += diffuseReflect; //add color by diffuse reflect

                vec3 reflect = glm::reflect(light->incidentDirection(intersect.m_position),intersect.m_normal);
                float angle2 = glm::max(0.0f,dot(normalize(reflect),normalize(-ray.direction)));
                angle2 = pow(angle2,intersect.m_material->shininess());
                vec3 specReflect = light->irradiance(intersect.m_position) * angle2 * intersect.m_material->specular();
                color += specReflect; //add color by specular reflect
            }
        }
        if(depth > 1){
            vec3 reflection = glm::reflect(ray.direction, intersect.m_normal);
            vec3 recColor = sampleRay(Ray(intersect.m_position, reflection), depth-1);
            color += recColor * intersect.m_material->specular() * (1 - (1/intersect.m_material->shininess()));
        }
        
        return color;
    }
    return vec3(0.3f, 0.3f, 0.4f); //default background colour
}



vec3 ChallengePathTracer::sampleRay(const Ray &ray, int depth) {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Implement a PathTracer that calculates the diffuse and 
	// specular, for the given ray in the scene, using the 
	// Phong lighting model. Give special consideration to objects
	// that occluded from direct lighting (shadow rays).
	// Implement support for textured materials (using a texture
	// for the diffuse portion of the material).
	//
	// EXTRA FOR EXPERTS :
	// Additionally implement indirect diffuse and specular instead
	// of using the ambient lighting term.
	// The diffuse is sampled from the surface hemisphere and the
	// specular is sampled from a cone of the phong lobe (which
	// gives a glossy look). For best results you need to normalize
	// the lighting (see http://www.thetenthplanet.de/archives/255)
	//-------------------------------------------------------------

    vec3 color = vec3(0, 0, 0);
    RayIntersection intersect = m_scene->intersect(ray);

    if(intersect.m_valid){
        for(unsigned long i = 0; i < m_scene->lights().size(); i++){
            std::shared_ptr<Light> light = m_scene->lights().at(i);
            vec3 diffuseElement = light->ambience() * intersect.m_material->diffuse();
            color += diffuseElement + vec3(intersect.m_uv_coord,0); //add color by diffuse element + texture

            if(!light->occluded(m_scene, intersect.m_position)){ //if not occluded
                float angle = glm::max(0.0f,dot(-light->incidentDirection(intersect.m_position),intersect.m_normal));
                vec3 diffuseReflect = light->irradiance(intersect.m_position) * angle * intersect.m_material->diffuse();
                color += diffuseReflect; //add color by diffuse reflect

                vec3 reflect = glm::reflect(light->incidentDirection(intersect.m_position),intersect.m_normal);
                float angle2 = glm::max(0.0f,dot(normalize(reflect),normalize(-ray.direction)));
                angle2 = pow(angle2,intersect.m_material->shininess());
                vec3 specReflect = light->irradiance(intersect.m_position) * angle2 * intersect.m_material->specular();
                color += specReflect; //add color by specular reflect
            }
        }
//        if(depth > 1){
//            vec3 reflection = glm::reflect(ray.direction, intersect.m_normal);
//            vec3 recColor = sampleRay(Ray(intersect.m_position, reflection), depth-1); //floating point error here?
//            color += recColor * intersect.m_material->specular() * (1 - (1/intersect.m_material->shininess()));
//        }
        
        return color;
    }
    return vec3(0.3f, 0.3f, 0.4f); //default background colour
}
