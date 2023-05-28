#pragma once
#include <string>
#include "framework.h"
#include "mesh.h"
#include "texture.h"
#include "shader.h"
#include "AIBehaviour.h"

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

typedef enum {
	NONE = 0,
	PLAYER = 1 << 0,
	WALL = 1 << 1,
	FLOOR = 1 << 2,
	ENEMY = 1 << 3,
	SCENARIO = WALL | FLOOR,
	CHARACTER = PLAYER | ENEMY,
	ALL = CHARACTER | SCENARIO
} COLISSION_LAYER;

class EntityCollider : public EntityMesh {
public:
	bool isDynamic = false;
	COLISSION_LAYER layer = NONE;

	using EntityMesh::EntityMesh;
	EntityCollider(bool isDynamic, COLISSION_LAYER layer);
};

class EntityPlayer : public EntityCollider {
public:
	// Initial position
	Vector3 initial_pos = Vector3(-7.f, 35.f, -120.f);

	// Rotation
	float yaw = 0.f;
	float pitch = 0.f;
	float roll = 0.f;
	float lastYaw = 0.f;

	// Movement
	const float speed = 1.0f;
	const float crouch_factor = 0.3f;
	const float jump_speed = 100.0f;
	const int max_dashes = 1;
	const float dash_speed = 200.0f;
	const float gravity_speed = 150.f;
	const float floor_friction = 0.001f;
	
	
	// bool isOnFloor = false;
	int dashes = 1;
	Vector3 velocity = Vector3(0, 0, 0);

	// Model
	float model_height = 5.f;

	void onTouchFloor();
	void update(float seconds_elapsed);
	EntityPlayer();
};

class EntityArmy : public EntityCollider {
public:
	std::vector<AIBehaviour> stateMachines;
	bool onAlert = false;

	void update(float seconds_elapsed);
	void render() override;

	EntityArmy(Mesh* mesh, Texture* texture, Shader* shader, std::vector<Matrix44> models);
private:
	// constants
	Vector4 SEARCH_COLOR = Vector4(0.f, 0.f, 0.5f, 1.f);
	Vector4 FOUND_COLOR = Vector4(0.5f, 0.f, 0.f, 1.f);
	float moveSpeed = 10.f;
};
