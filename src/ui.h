#pragma once
#include "entity.h"

class UI {
public:
	int width;
	int height;
	EntityPlayer* player;

	Shader* shader;


	UI(int width, int height, EntityPlayer* player);
	void render();
	void renderStatic();
	void renderDynamic();
	void renderContainers();
	void renderCrosshair();
	void renderStaminaBar();
	void renderAbilities();
	void renderCollectible();

};
