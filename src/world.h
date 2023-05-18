#pragma once
#include "camera.h"
#include "entity.h"
#include "game.h"

struct sRenderData {
	Texture* texture = nullptr;
	Shader* shader = nullptr;
	std::vector<Matrix44> models;
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
	
};

