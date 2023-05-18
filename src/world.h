#pragma once
#include "camera.h"
#include "entity.h"
#include "game.h"

struct sRenderData {
	Texture* texture = nullptr;
	Shader* shader = nullptr;
	std::vector<Matrix44> models;
};

struct sCollisionData {
	Vector3 colPoint;
	Vector3 colNormal;
};


class World
{
public:
	Camera* camera;
	bool freeCam = false;
	bool firstPerson = true;
	Entity* root;
	EntityPlayer* player;
	std::map<std::string, sRenderData> meshes_to_load;

	World(const char* sceneFilename);
	bool parseScene(const char* filename);
	void render();
	void update(double seconds_elapsed);
	void updateCamera(double seconds_elapsed);
	bool checkPlayerCollision(const Vector3& target, std::vector<sCollisionData>* collisions);
	bool checkLineOfSight(Matrix44& obs, Matrix44& target);
	
};
