
#pragma once

// glm
#include <glm/glm.hpp>

// project
#include "ray.hpp"
#include "scene.hpp"
#include "texture.hpp"

class Shape {
public:
    Texture texture = Texture("checkerboard.jpg"); //set working directory to textures folder
	virtual RayIntersection intersect(const Ray &ray) = 0;
};


class AABB : public Shape {
private:
	glm::vec3 m_center;
	glm::vec3 m_halfsize;
public:
	AABB(const glm::vec3 &c, float hs) : m_center(c), m_halfsize(hs) { }
	AABB(const glm::vec3 &c, const glm::vec3 &hs) : m_center(c), m_halfsize(hs) { }
	virtual RayIntersection intersect(const Ray &ray) override;
};


class Sphere : public Shape {
private:
	glm::vec3 m_center;
	float m_radius;
public:
	Sphere(const glm::vec3 &c, float radius) : m_center(c), m_radius(radius) { }
	virtual RayIntersection intersect(const Ray &ray) override;
};

//-------------------------------------------------------------
// [Assignment 4] :
// Implement the following additional Shapes :
// - Plane
// - Disk
// - Triangle
// Follow the pattern shown by AABB and Sphere for implementing
// a class that subclasses Shape making sure that you implement
// the intersect method for each new Shape.
//-------------------------------------------------------------

class Plane : public Shape{
private:
    glm::vec3 m_translation;
    glm::vec3 m_normal;
public:
    Plane(const glm::vec3 &t, const glm::vec3 &n) : m_translation(t), m_normal(n) { }
    virtual RayIntersection intersect(const Ray &ray) override;
};

class Disk : public Shape{
private:
    glm::vec3 m_translation;
    glm::vec3 m_normal;
    float m_radius;
public:
    Disk(const glm::vec3 &t, const glm::vec3 &n, float radius) : m_translation(t), m_normal(n), m_radius(radius) { }
    virtual RayIntersection intersect(const Ray &ray) override;
};

class Triangle : public Shape{
private:
    glm::vec3 m_vertex0;
    glm::vec3 m_vertex1;
    glm::vec3 m_vertex2;
public:
    Triangle(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2) :
    m_vertex0(v0), m_vertex1(v1), m_vertex2(v2) { }
    virtual RayIntersection intersect(const Ray &ray) override;
};
