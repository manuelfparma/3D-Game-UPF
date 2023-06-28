#pragma once
#include "entity.h"

class UI {
public:
	int width;
	int height;
	EntityPlayer* player;

	Shader* shader;
	Mesh* staminaContainer;

	Mesh* ability1;
	Mesh* ability2;
	Mesh* ability3;

	Mesh* collectible;

	Mesh* crosshair;

	UI(int width, int height, EntityPlayer* player);
	void createStaticMeshes();
	void onResize(int width, int height);
	void render();
	void renderStatic();
	void renderDynamic();
	void renderContainers();
	void renderCrosshair();
	void renderStaminaBar();
	void renderAbilities();
	void renderCollectible();

};
