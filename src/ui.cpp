#include "ui.h"
#include "utils.h"
#include <iomanip>

UI::UI(int window_width, int window_height, EntityPlayer* UIplayer) {
	width = window_width;
	height = window_height;
	player = UIplayer;

	
	/*	
	Mesh* staminaContainerMesh = new Mesh();
	staminaContainerMesh-> createQuad(0.02 * window_width, 0.05 * window_height, 0.2 * window_width, 0.05 * window_height, true);
	EntityUI* staminaContainer = new EntityUI(staminaContainerMesh, Shader::Get("data/shaders/gui.vs", "data/shaders/texture.fs"), Texture::getWhiteTexture(), Vector4(0,0,0,1));
	UIBackground.push_back(staminaContainer);
	

	Mesh* staminaMesh = new Mesh();
	staminaMesh->createQuad(0.02 * window_width, 0.05 * window_height, 0.2 * window_width, 0.05 * window_height, true);
	stamina = new EntityUI(staminaMesh, Shader::Get("data/shaders/gui.vs", "data/shaders/texture.fs"), Texture::getBlackTexture(), Vector4(255, 170, 0, 1));
	UIElements.push_back(stamina);

	*/

	
}


void UI::update() {

	stamina->mesh->createQuad(0.2 * width, 0.05 * height, 0.2 * width * player->stamina / 100.f, 0.05 * height, false);

};


void UI::render() {


	/*
	update();

	for (auto e : UIBackground) {
		e->render();
	}

	for (auto e : UIElements) {
		e->render();
	}
	*/
	
	std::stringstream stream;
	stream << std::fixed << std::setprecision(2) << player->stamina;
	drawText(2, 50, "Stamina: " + stream.str(), Vector3(1, 1, 1), 2);
};


