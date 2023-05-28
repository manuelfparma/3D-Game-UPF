#pragma once
#include "entity.h"

class UI {
public:
	int width;
	int height;
	EntityPlayer* player;


	EntityUI* stamina;


	std::vector<EntityUI*> UIBackground;
	std::vector<EntityUI*> UIElements;
	UI(int width, int height, EntityPlayer* player);
	void update();
	void render();

};
