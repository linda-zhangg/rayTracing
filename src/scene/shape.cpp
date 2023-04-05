
// std
#include <algorithm>
#include <utility>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// project
#include "shape.hpp"

#include "texture.hpp"


using namespace glm;


RayIntersection AABB::intersect(const Ray &ray) {
	RayIntersection intersect;
	vec3 rel_origin = ray.origin - m_center;

	// start magic
	// x
	float rdx_inv = 1 / ray.direction.x;
	float tx1 = (-m_halfsize.x - rel_origin.x) * rdx_inv;
	float tx2 = (m_halfsize.x - rel_origin.x) * rdx_inv;

	float tmin = std::min(tx1, tx2);
	float tmax = std::max(tx1, tx2);

	// y
	float rdy_inv = 1 / ray.direction.y;
	float ty1 = (-m_halfsize.y - rel_origin.y) * rdy_inv;
	float ty2 = (m_halfsize.y - rel_origin.y) * rdy_inv;

	tmin = std::max(tmin, std::min(ty1, ty2));
	tmax = std::min(tmax, std::max(ty1, ty2));

	// z
	float rdz_inv = 1 / ray.direction.z;
	float tz1 = (-m_halfsize.z - rel_origin.z) * rdz_inv;
	float tz2 = (m_halfsize.z - rel_origin.z) * rdz_inv;

	tmin = std::max(tmin, std::min(tz1, tz2));
	tmax = std::min(tmax, std::max(tz1, tz2));

	if (tmax < tmin) return intersect;
	// end magic

	intersect.m_distance = tmin < 0 ? tmax : tmin;
	intersect.m_position = ray.origin + intersect.m_distance * ray.direction;
	intersect.m_valid = tmax >= 0;
	vec3 work_out_a_name_for_it_later = abs((intersect.m_position - m_center) / m_halfsize);
	float max_v = std::max(work_out_a_name_for_it_later[0], std::max(work_out_a_name_for_it_later[1], work_out_a_name_for_it_later[2]));
	intersect.m_normal = normalize(mix(intersect.m_position - m_center, vec3(0), lessThan(work_out_a_name_for_it_later, vec3(max_v))));
    
    intersect.m_uv_coord = (abs(intersect.m_normal.x) > 0) ?
            texture.sample(vec2(intersect.m_position.y, intersect.m_position.z)) :
            texture.sample(vec2(intersect.m_position.x, intersect.m_position.y + intersect.m_position.z));
    
	intersect.m_shape = this;

	return intersect;
}


RayIntersection Sphere::intersect(const Ray &ray) {
	RayIntersection intersect;
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Implement the intersection method for Sphere that returns
	// a RayIntersection object with valid == false if the 
	// ray doesn't intersect and true otherwise. If true, then
	// remember to fill out each field in the object correctly:
	// - m_valid : true if object is itersected
	// - m_distance : distance along the ray of the intersection
	// - m_position : position on the surface of the intersection
	// - m_normal : normal on the surface of the intersection
	// - m_shape : set to "this"
	// - m_uv_coord : texture coordinates (challenge only)
	//-------------------------------------------------------------
    
    float a = dot(ray.direction, ray.direction);
    float b = 2 * dot(ray.origin-m_center, ray.direction);
    float c = dot(ray.origin-m_center, ray.origin-m_center) - (m_radius*m_radius);
    
    float discriminant = (b*b) - 4*a*c;
    
    if(discriminant >= 0){
        float t0 = (-b - sqrt(discriminant)) / (2*a);
        float t1 = (-b + sqrt(discriminant)) / (2*a);
        
        if((t0>0 && t1>t0) || t0==t1){
            intersect.m_valid = true;
            intersect.m_distance = t0;
            intersect.m_position = ray.origin + t0 * ray.direction;
            intersect.m_normal = normalize(intersect.m_position - m_center);
            intersect.m_shape = this;
            //texture here - challenge only
            
            vec3 n = normalize(intersect.m_position - m_center);
            float u = atan2(n.x, n.z) / (2*M_PI) + 0.5;
            float v = n.y * 0.5f + 0.5f;
            intersect.m_uv_coord = texture.sample(vec2(u,v));
        }
    }
	return intersect;
}

RayIntersection Plane::intersect(const Ray &ray) {
    RayIntersection intersect;
    
    //calc if there is an intersection on the plane
    float t = (dot((m_translation - ray.origin), m_normal)) / dot(normalize(ray.direction), m_normal);
    
    //intersection if ray is pointing in correct direction and is not parallel to the plane
    if(t >= 0 && dot(normalize(ray.direction), m_normal) != 0){
        intersect.m_valid = true;
        intersect.m_distance = t;
        intersect.m_position = ray.origin + t * ray.direction;
        
        //check which side of the plane the ray intersects it
        if(dot(normalize(ray.direction), m_normal) > 0) intersect.m_normal = -m_normal;
        else intersect.m_normal = m_normal;
        
        //texture
        vec3 e1 = normalize(cross(m_normal, vec3(1, 0, 0)));
        //If normal and (1,0,0) are parallel, change e1
        if (e1 == vec3(0, 0, 0)) {
            e1 = normalize(cross(m_normal, vec3(0, 0, 1)));
        }
        vec3 e2 = normalize(cross(m_normal, e1));
        intersect.m_uv_coord = texture.sample(vec2(dot(e1, intersect.m_position),dot(e2, intersect.m_position)));
        
        intersect.m_shape = this;
    }
               
    return intersect;
}

RayIntersection Disk::intersect(const Ray &ray) {
    RayIntersection intersect;
    
    //calc if there is an intersection on the plane
    float t = (dot((m_translation - ray.origin), m_normal)) / dot(normalize(ray.direction), m_normal);

    //plane intersection conditions and if its inside disk
    if(t >= 0 && dot(normalize(ray.direction), m_normal) != 0){
        //inside disk
        vec3 p = ray.origin + t * normalize(ray.direction);
        vec3 v = p - m_translation;
        if(sqrt(dot(v, v)) <= m_radius){
            intersect.m_valid = true;
            intersect.m_distance = t;
            intersect.m_position = ray.origin + t * ray.direction;
            
            //check which side of the disk the ray intersects it
            if(dot(normalize(ray.direction), m_normal) > 0) intersect.m_normal = -m_normal;
            else intersect.m_normal = m_normal;
            
            vec3 e1 = normalize(cross(m_normal, vec3(1, 0, 0)));
            //If normal and (1,0,0) are parallel, change e1
            if (e1 == vec3(0, 0, 0)) {
                e1 = normalize(cross(m_normal, vec3(0, 0, 1)));
            }
            vec3 e2 = normalize(cross(m_normal, e1));
            intersect.m_uv_coord = texture.sample(vec2(dot(e1, intersect.m_position),dot(e2, intersect.m_position)));
            
            intersect.m_shape = this;
        }
        
    }
    return intersect;
}

RayIntersection Triangle::intersect(const Ray &ray) { //used help from scratchPixel
    RayIntersection intersect;
    
    // compute plane's normal
    vec3 v0v1 = m_vertex1 - m_vertex0;
    vec3 v0v2 = m_vertex2 - m_vertex0;
    // no need to normalize
    vec3 N = cross(v0v1,(v0v2));  //N
 
    // Step 1: finding P
 
    // check if ray and plane are parallel.
    float NdotRayDirection = dot(N, normalize(ray.direction));
    if (abs(NdotRayDirection) < 0.0001)  //almost 0
        return intersect;  //they are parallel so they don't intersect!
 
    // compute d parameter using equation 2
    float d = dot(-N,m_vertex0);
 
    // compute t (equation 3)
    float t = -(dot(N, ray.origin) + d) / NdotRayDirection;
 
    // check if the triangle is in behind the ray
    if (t < 0) return intersect;  //the triangle is behind
 
    // compute the intersection point using equation 1
    vec3 P = ray.origin + t * normalize(ray.direction);
 
    // Step 2: inside-outside test
    vec3 C;  //vector perpendicular to triangle's plane
 
    // edge 0
    vec3 edge0 = m_vertex1 - m_vertex0;
    vec vp0 = P - m_vertex0;
    C = cross(edge0,vp0);
    if (dot(N, C) < 0) return intersect;  //P is on the right side
 
    // edge 1
    vec3 edge1 = m_vertex2 - m_vertex1;
    vec3 vp1 = P - m_vertex1;
    C = cross(edge1, vp1);
    if (dot(N, C) < 0)  return intersect;  //P is on the right side
 
    // edge 2
    vec3 edge2 = m_vertex0 - m_vertex2;
    vec3 vp2 = P - m_vertex2;
    C = cross(edge2, vp2);
    if (dot(N, C) < 0) return intersect;  //P is on the right side;
    
    intersect.m_valid = true;
    intersect.m_distance = t;
    intersect.m_position = ray.origin + t * ray.direction;

    //check which side of the triangle the ray intersects it
    if(dot(normalize(ray.direction), N) > 0) intersect.m_normal = -N;
    else intersect.m_normal = N;
    
//    vec3 U =m_vertex1 - m_vertex0;
//    vec3 V =m_vertex2 - m_vertex0;
    
    //intersect.m_uv_coord = texture.sample(vec2(m_vertex1 - m_vertex0, m_vertex2 - m_vertex0));
    intersect.m_shape = this;
    
    return intersect;
}


