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

	bool isInstanced = false;
	std::vector<Matrix44> models;

	void render() override;

	EntityMesh() {};
	EntityMesh(Mesh* mesh, Texture* texture, Shader* shader);
	EntityMesh(Mesh* mesh, Texture* texture, Shader* shader, bool isInstanced, std::vector<Matrix44> models);
};

enum {
	NONE = 0,
	CHARACTER,
	WALL,
	FLOOR,
	SCENARIO = WALL | FLOOR,
	ALL = CHARACTER | SCENARIO
};

class EntityCollider : public EntityMesh {
public:
	bool isDynamic = false;
	int layer = NONE;
	bool testCollision(EntityCollider* entity);

	//sphereRadius, etc
	// void get COllisions

	EntityCollider(bool isDynamic, int layer);
};

class EntityPlayer : public EntityCollider {
public:
	float yaw;
	float pitch;
	float roll;
	
	float speed;
	float jumpSpeed;
	bool isOnFloor;

	Vector3 velocity;

	EntityPlayer();
	void update(float seconds_elapsed);
};


/*

if ( entity->layer & SCENARIO ) {}

*/