#pragma once
#include <string>
#include "framework.h"
#include "mesh.h"
#include "texture.h"
#include "shader.h"

class Entity
{
public:
	Entity() {};

	// render functions
	std::string name;
	Matrix44 model;

	virtual void render();
	virtual void update(double elapsed_time) {};

	// world coordinates transform
	Matrix44 getGlobalMatrix();

	// tree functions
	Entity* parent = nullptr;
	std::vector<Entity*> children;

	void addChild(Entity* child);
	void removeChild(Entity* child);
};

class EntityMesh : public Entity 
{
public:
	Mesh* mesh = nullptr;
	Texture* texture = nullptr;
	Shader* shader = nullptr;
	Vector4 color = Vector4(1, 1, 1, 1);

	void render();
};
