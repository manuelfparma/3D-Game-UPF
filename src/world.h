#pragma once
#include "camera.h"
#include "entity.h"
#include "game.h"

class World
{
public:
	Camera* camera;
	Entity* root;
	EntityPlayer* player;

	World();
	virtual void render();
	virtual void update(double seconds_elapsed);
	
};

