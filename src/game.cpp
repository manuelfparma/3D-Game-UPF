#include "game.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"
#include "entity.h"
#include "stage.h"
#include "sound.h"

#include <cmath>


Game* Game::instance = NULL;

Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	mouse_locked = false;
	draw_gpu_stats = false;


	loadingScreen(window_width, window_height);

	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer

	// initialize stage and audio manager
	Audio::Init();	// this first because stages used sounds
	stageManager = new StageManager();

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
}

void Game::loadingScreen(int window_width, int window_height) {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Vector4 color = Vector4(1.0, 1.0, 1.0, 1.0);

	Shader* shader = Shader::Get("data/shaders/gui.vs", "data/shaders/hud.fs");
	Mesh* background = new Mesh();
	background->createQuad(window_width / 2.f, window_height / 2.f, window_width, window_height, true);

	Camera* camera2D = new Camera();
	camera2D->view_matrix = Matrix44();
	camera2D->setOrthographic(0, window_width, 0, window_height, -1, 1);
	camera2D->enable();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	shader->enable();
	shader->setUniform("u_viewprojection", Camera::current->viewprojection_matrix);
	shader->setUniform("u_model", Matrix44());
	shader->setUniform("u_color", color);
	shader->setUniform("u_discard", true);
	shader->setUniform("u_discard_color", Vector3(0.0f, 0.0f, 0.0f));

	shader->setUniform("u_texture", Texture::Get("data/ui/ninja_bg.png"), 0);
	background->render(GL_TRIANGLES);

	float width = window_width,
		height = window_height;

	Mesh* loading = new Mesh();
	loading->createQuad(width * 0.1, height * 0.1, 620 / 5, 235 / 5, true);
	shader->setUniform("u_texture", Texture::Get("data/ui/loading.png"), 1);
	loading->render(GL_TRIANGLES);

	shader->disable();
	SDL_GL_SwapWindow(this->window);
}

//what to do when the image has to be draw
void Game::render(void)
{
	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set the camera as default
	
	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
   
	//create model matrix for cube
	//Matrix44 m;
	//m.rotate(angle*DEG2RAD, Vector3(0, 1, 0));

	stageManager->render();

	//render the FPS, Draw Calls, etc
	if (draw_gpu_stats) {

		drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);
	}

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	if (return_to_start) {
		return_to_start = false;
		stageManager->changeStage(INTRO_STAGE, 0);
	}
	stageManager->update(seconds_elapsed);

}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: return_to_start = true; break;
		case SDLK_F1: Shader::ReloadAll(); break; 
		case SDLK_F3: draw_gpu_stats = !draw_gpu_stats; break;
	}
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {
		mouse_clicked = true;
		// we have to flip the value in Y as the quads start the zero on the other side
		last_click = Vector2(event.x, window_height - event.y);
	}
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
}

void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );

	stageManager->currentStage->onResize(width, height);
	window_width = width;
	window_height = height;
}

Vector3 Game::getPlayerPosition() {
	PlayStage* current_stage = dynamic_cast<PlayStage*>(instance->stageManager->currentStage);

	if (current_stage != nullptr) {
		return current_stage->world->player->model.getTranslation();
	}
	else {
		return Vector3(0.f, 0.f, 0.f);
	}
}
