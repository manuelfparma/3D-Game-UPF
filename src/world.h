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
	Entity* root;
	EntityPlayer* player;
	std::map<std::string, sRenderData> meshes_to_load;

	World(const char* sceneFilename);
	void render();
	void update(double seconds_elapsed);
	bool parseScene(const char* filename);
	
};

