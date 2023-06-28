#include "ui.h"
#include "utils.h"
#include <iomanip>
#include "camera.h"

#include "input.h"

UI::UI(int window_width, int window_height, EntityPlayer* UIplayer) {
	width = window_width;
	height = window_height;
	player = UIplayer;
	shader = Shader::Get("data/shaders/gui.vs", "data/shaders/hud.fs");

	Texture::Get("data/ui/stamina.png");
	Texture::Get("data/ui/dash.png");
	Texture::Get("data/ui/jump.png");
	Texture::Get("data/ui/smoke-bomb.png");
	Texture::Get("data/ui/artifact.png");
	Texture::Get("data/ui/crosshair.png");

}

void UI::render() {
	
	Vector4 color = Vector4(1.f, 1.f, 1.f, 1.f);
	glDisable(GL_DEPTH_TEST);

	shader->enable();
	shader->setUniform("u_viewprojection", Camera::current->viewprojection_matrix);
	shader->setUniform("u_model", Matrix44());
	shader->setUniform("u_color", color);
	shader->setUniform("u_discard", true);
	shader->setUniform("u_discard_color", Vector3(0.0f, 0.0f, 0.0f));

	renderStatic();
	renderDynamic();

	shader->disable();

	glDisable(GL_DEPTH_TEST);

};

void UI::renderStatic() {
	
	renderContainers();
	renderAbilities();
	renderCrosshair();

}

void UI::renderDynamic() {
	renderStaminaBar();
	renderArtifact();
}


void UI::renderContainers() {

	Mesh* staminaContainer = new Mesh();
	float startX = 0.2 * width;
	float startY = (1 - 0.07) * height;
	float quadWidth = 0.2 * width;
	float quadHeight = 0.05 * height;
	float stamina = player->stamina / 100.f;

	float offset = (quadWidth * (1.f - stamina) * 0.5f);


	staminaContainer->createQuad(startX, startY, quadWidth, quadHeight, true);
	shader->setUniform("u_texture", Texture::getColorTexture(Vector3(200, 200, 200)), 0);
	staminaContainer->render(GL_TRIANGLES);
}

void UI::renderCrosshair() {

	Mesh* crosshair = new Mesh();
	crosshair->createQuad(0.5 * width, 0.5 * height, height * 0.03, width * 0.03, true);
	shader->setUniform("u_texture", Texture::Get("data/ui/crosshair.png"), 0);
	crosshair->render(GL_TRIANGLES);

}

void UI::renderAbilities() {

	Mesh* ability = new Mesh();

	float startX = 0.1 * width;
	float startY = 0.15 * height;
	float abilitySize = height * 0.1;

	ability->createQuad(startX * 1, startY, abilitySize, abilitySize, true);
	shader->setUniform("u_texture", Texture::Get("data/ui/jump.png"), 0);
	shader->setUniform("u_grayscale", player->jumps <= 0 || player->stamina < player->jump_cost);
	ability->render(GL_TRIANGLES);

	ability->createQuad(startX * 2, startY, abilitySize, abilitySize, true);
	shader->setUniform("u_grayscale", !player->dashes > 0 || player->stamina < player->dash_cost);
	shader->setUniform("u_texture", Texture::Get("data/ui/dash.png"), 0);
	ability->render(GL_TRIANGLES);

	ability->createQuad(startX * 3, startY, abilitySize, abilitySize, true);
	shader->setUniform("u_grayscale", player->invisible || player->stamina < player->invisible_cost);
	shader->setUniform("u_texture", Texture::Get("data/ui/smoke_bomb.png"), 0);
	ability->render(GL_TRIANGLES);


	shader->setUniform("u_grayscale", false);

	

}

void UI::renderStaminaBar() {

	
	Mesh* staminaMesh = new Mesh();

	float startX = 0.2 * width;
	float startY = (1 - 0.07) * height;
	float quadWidth = 0.2 * width;
	float quadHeight = 0.05 * height;
	float stamina = player->stamina / 100.f;
	float offset = (quadWidth * (1.f - stamina) * 0.5f);



	staminaMesh->createQuad(startX - offset, startY, quadWidth * stamina, quadHeight, true);
	shader->setUniform("u_texture", Texture::Get("data/ui/stamina.png"), 0);
	staminaMesh->render(GL_TRIANGLES);

}

void UI::renderArtifact() {
	Mesh* artifact = new Mesh();

	artifact->createQuad(0.8 * width, (0.15) * height, height * 0.2, width * 0.2, true);
	shader->setUniform("u_texture", Texture::Get("data/ui/artifact.png"), 0);
	artifact->render(GL_TRIANGLES);
}
