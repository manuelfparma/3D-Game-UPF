#include "world.h"
#include "input.h"
#include "animation.h"
#include "entity.h"
#include "utils.h"
#include <fstream>
#include <map>

//some globals
Animation* anim = NULL;
float angle = 0;
float mouse_speed = 100.0f;
FBO* fbo = NULL;

World::World(const char* sceneFilename) {
    camera = new Camera();
    camera->lookAt(Vector3(0.f, 100.f, 100.f), Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
    camera->setPerspective(70.f, Game::instance->window_width / (float)Game::instance->window_height, 0.1f, 10000.f); //set the projection, we want to be perspective

	// initialize root and player
    root = new Entity();
    player = new EntityPlayer();

	createSkybox();
	createEnemies();

	// open scene
	parseScene(sceneFilename);
}

void World::createSkybox() {
	std::vector<std::string> faces = {
		"data/textures/skybox/right.png",
		"data/textures/skybox/left.png",
		"data/textures/skybox/bottom.png",
		"data/textures/skybox/top.png",
		"data/textures/skybox/back.png",
		"data/textures/skybox/front.png",
	};

	Texture* skybox = new Texture();
	skybox->loadCubemap("sky", faces);

	sky = new EntityMesh();
	sky->mesh = Mesh::Get("data/meshes/box.ASE");
	sky->shader = Shader::Get("data/shaders/cube.vs", "data/shaders/cubemap.fs");
	sky->texture = skybox;
}

void World::createEnemies() {
	std::vector<Matrix44> models;
	std::vector<float> yaws;
	Matrix44 pos;

	for (int i = 0; i < ENEMY_COUNT; ++i) {
		pos.setTranslation(Vector3(0.f, 0.2f, 0.f));
		//pos.rotate(DEG2RAD * (360 / ENEMY_COUNT) * (i % ENEMY_COUNT), Vector3(0, 1, 0));
		models.push_back(pos);
	}

	enemies = new EntityArmy(
		Mesh::Get("data/models/samurai.obj"),
		nullptr,
		Shader::Get("data/shaders/instanced.vs", "data/shaders/material.fs"),
		models
	);
}

void World::renderSky() {
	
	sky->model.setTranslation(camera->eye);
	glDisable(GL_DEPTH_TEST);
	sky->render();
	glEnable(GL_DEPTH_TEST);
}


void World::render() {

	renderSky();

	if (!freeCam) {

		Matrix44 mYaw;
		Matrix44 mPitch;

		mYaw.setRotation(player->yaw, Vector3(0, 1, 0));
		mPitch.setRotation(player->pitch, Vector3(-1, 0, 0));

		Vector3 front = (mPitch * mYaw).frontVector();
		Vector3 eye;
		Vector3 center;

		if (firstPerson) {
			eye = player->getGlobalMatrix() * Vector3(0.f, player->model_height, 0.5f);
			center = eye + front;
		}
		else {
			eye = player->model.getTranslation() - front * 10.0f;
			center = player->getGlobalMatrix() * Vector3(0, player->model_height, 0.5f);
		}

		checkCameraCollision(eye);

		camera->lookAt(eye, center, Vector3(0, 1, 0));
	}

    root->render(); 

	if (freeCam || !firstPerson)
		player->render();

	enemies->render();
}

void World::update(double seconds_elapsed) {
	// pause the game if free camera is activated
	if (!freeCam) {
		player->update(seconds_elapsed);
		enemies->update(seconds_elapsed);
	}
	else {
		updateCamera(seconds_elapsed);
	}

    //to navigate with the mouse fixed in the middle
    if (Game::instance->mouse_locked)
        Input::centerMouse();

	if (Input::wasKeyPressed(SDL_SCANCODE_TAB)) {
		Game::instance->mouse_locked = !Game::instance->mouse_locked;
		SDL_ShowCursor(!Game::instance->mouse_locked);
		freeCam = !freeCam;
	}

	if (Input::wasKeyPressed(SDL_SCANCODE_C)) {
		firstPerson = !firstPerson;
	}
}

void World::updateCamera(double seconds_elapsed) {

	float speed = 10.f * seconds_elapsed;
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
	if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f, -1.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);

	if (Input::mouse_state & SDL_BUTTON_LEFT) //is left button pressed?
	{
		camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
		camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
	}
}

void World::collisionCalculations(Mesh* mesh, Matrix44 model, Vector3 target, std::vector<sCollisionData>* collisions) {
	Vector3 col_point;
	Vector3 col_normal;

	// as the position of the player is on its feet, we add a height
	float max_dist = player->model_height / 2;
	target.y += max_dist;

	// wall colision
	if (!mesh->testSphereCollision(model, target, max_dist, col_point, col_normal)) return;
	// add colision to list
	col_normal.normalize();
	collisions->push_back({ col_point, col_normal });

	// floor collision
	if (!mesh->testRayCollision(model, target, Vector3(0, -1, 0), col_point, col_normal, max_dist)) return;
	// add colision to list
	col_normal.normalize();
	collisions->push_back({ col_point, col_normal });
}

bool World::checkPlayerCollision(Vector3 target, std::vector<sCollisionData>* collisions) {
	for (auto& e : root->children){
		EntityCollider* ec = dynamic_cast<EntityCollider*>(e);

		if (!ec || ec->isDynamic) continue;

		if (ec->isInstanced) {
			for (auto& model : ec->models) {
				collisionCalculations(ec->mesh, model, target, collisions);
			}
		}
		else {
			collisionCalculations(ec->mesh, ec->model, target, collisions);
		}	
	}

	return !collisions->empty();
}

void World::checkCameraCollision(Vector3& target) {
	Vector3 origin = player->getGlobalMatrix() * Vector3(0.f, player->model_height, 0);
	Vector3 toTarget = target - origin;
	Vector3 collision;
	float distance = toTarget.length();
	
	Vector3 direction =  toTarget.length() > 0.01 ? toTarget.normalize() : toTarget;

	for (auto& e : root->children) {
		EntityCollider* ec = dynamic_cast<EntityCollider*>(e);

		if (!ec) continue;

		if (ec->isInstanced) {
			for (auto& model : ec->models) {
				if (ec->mesh->testRayCollision(model, origin, direction, collision, Vector3(), distance)) {
					target = collision;
					distance = (target - origin).length();
				}
			}
		}
		else {
			if (ec->mesh->testRayCollision(ec->model, origin, normalize(toTarget), collision, Vector3(), distance)) {
				target = collision;
				distance = (target - origin).length();
			};
		}
	}
}

bool World::checkLineOfSight(Matrix44& obs, Vector3 target) {
	Vector3 front = obs.frontVector();
	Vector3 toTarget = target - obs.getTranslation();

	float distance = toTarget.length();

	if (distance > MAX_VIEW_DISTANCE)
		return false;

	Vector3 rayOrigin = obs.getTranslation();
	rayOrigin.y += player->model_height;
	Vector3 direction = normalize(toTarget);

	if (direction.dot(front) > 0.5)
	{
		for (auto e : root->children)
		{
			EntityCollider* ec = dynamic_cast<EntityCollider*>(e);

			if (!ec || ec->isDynamic) continue;

			if (ec->isInstanced) {
				// instanced entity
				for (auto model : ec->models)
					if (ec->mesh->testRayCollision(model, rayOrigin, direction, Vector3(), Vector3(), distance))
						return false;
			}
			// not instanced entity
			else if (ec->mesh->testRayCollision(ec->model, rayOrigin, direction, Vector3(), Vector3(), distance))
				return false;
		}

		return true;
	}

	return false;
}

bool World::parseScene(const char* filename)
{
	// You could fill the map manually to add shader and texture for each mesh
	// If the mesh is not in the map, you can use the MTL file to render its colors
	// meshes_to_load["meshes/example.obj"] = { Texture::Get("texture.tga"), Shader::Get("shader.vs", "shader.fs") };

	std::cout << " + Scene loading: " << filename << "..." << std::endl;

	std::ifstream file(filename);

	if (!file.good()) {
		std::cerr << "Scene [ERROR]" << " File not found!" << std::endl;
		return false;
	}

	std::string scene_info, mesh_name, model_data;
	file >> scene_info; file >> scene_info;
	int mesh_count = 0;

	// Read file line by line and store mesh path and model info in separated variables
	while (file >> mesh_name >> model_data)
	{
		// Get all 16 matrix floats
		std::vector<std::string> tokens = tokenize(model_data, ",");

		// Fill matrix converting chars to floats
		Matrix44 model;
		for (int t = 0; t < tokens.size(); ++t) {
			model.m[t] = (float)atof(tokens[t].c_str());
		}

		// Add model to mesh list (might be instanced!)
		sRenderData& render_data = meshes_to_load[mesh_name];
		render_data.models.push_back(model);
		mesh_count++;
	}

	// TODO: Shaders and texture should be set up at a .scene level
	Texture* defaultTexture = Texture::Get("data/texture.tga");
	Shader* instancedShader = Shader::Get("data/shaders/instanced.vs", "data/shaders/texture.fs");
	Shader* singleShader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");


	// Iterate through meshes loaded and create corresponding entities
	for (auto data : meshes_to_load) {

		mesh_name = "data/" + data.first;
		sRenderData& render_data = data.second;

		// No transforms, anything to do here
		if (render_data.models.empty())
			continue;

		// Create instanced entity
		if (render_data.models.size() > 1) {

			EntityCollider* new_entity = new EntityCollider(Mesh::Get(mesh_name.c_str()), defaultTexture, instancedShader, render_data.models);
			// Add entity to scene root
			root->addChild(new_entity);
		}
		// Create normal entity
		else {
			EntityCollider* new_entity = new EntityCollider(Mesh::Get(mesh_name.c_str()), defaultTexture, singleShader);
			new_entity->model = render_data.models[0];
			// Add entity to scene root
			root->addChild(new_entity);
		}
	}
	

	std::cout << "Scene [OK]" << " Meshes added: " << mesh_count << std::endl;
	return true;
}