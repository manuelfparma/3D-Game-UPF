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
	EntityMesh(Mesh* mesh, Texture* texture, Shader* shader, std::vector<Matrix44> models);
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
	bool testCollision(std::vector<EntityCollider*> entity);


	//sphereRadius, etc
	// void get COllisions

	/*

	if ( entity->layer & SCENARIO ) {}

	*/

	using EntityMesh::EntityMesh;
	EntityCollider(bool isDynamic, int layer);
};

class EntityPlayer : public EntityCollider {
public:
	// Initial position
	Vector3 initial_pos = Vector3(0.f, 5.f, 0.f);

	// Rotation
	float yaw = 0.f;
	float pitch = 0.f;
	float roll = 0.f;

	// Movement
	const float speed = 1.0f;
	const float crouch_factor = 0.3f;
	const float jump_speed = 300.0f;
	const float gravity_speed = 2.5f;
	const float floor_friction = 0.001f;
	Vector3 velocity = Vector3(0, 0, 0);

	void update(float seconds_elapsed);
	EntityPlayer();
};