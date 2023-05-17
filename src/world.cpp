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

    root = new Entity();
    player = new EntityPlayer();
	parseScene(sceneFilename);
}


void World::render() {

    Matrix44 mYaw;
    Matrix44 mPitch;

    mYaw.setRotation(player->yaw, Vector3(0, 1, 0));
    mPitch.setRotation(player->pitch, Vector3(-1, 0, 0));
    
    Vector3 front = (mPitch * mYaw).frontVector();
    Vector3 eye;
    Vector3 center;

    #define FIRST_PERSON false

    if (FIRST_PERSON) {
        eye = player->getGlobalMatrix() * Vector3(0.f, 2.f, 0.5f);
        center = eye + front;
    }
    else {
        eye = player->model.getTranslation() - front * 5.0f;
        center = player->getGlobalMatrix() * Vector3(0, 2.0f, 0.5f);
    }

    camera->lookAt(eye, center, Vector3(0, 1, 0));

    root->render();
}

void World::update(double seconds_elapsed) {
    // rotate ambulance
    angle = (float)seconds_elapsed * 10.0f;

    player->update(seconds_elapsed);

    //mouse input to rotate the cam
    /*if (Game::instance->mouse_locked ? true : (Input::mouse_state & SDL_BUTTON_LEFT)) //is left button pressed?
    {
        camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
        camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
    }
    */

    //to navigate with the mouse fixed in the middle
    if (Game::instance->mouse_locked)
        Input::centerMouse();
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

			InstancedEntityMesh* new_entity = new InstancedEntityMesh(Mesh::Get(mesh_name.c_str()), defaultTexture, instancedShader);
			// Add all instances
			new_entity->models = render_data.models;
			// Add entity to scene root
			root->addChild((Entity *)new_entity);
		}
		// Create normal entity
		else {
			EntityMesh* new_entity = new EntityMesh(Mesh::Get(mesh_name.c_str()), defaultTexture, singleShader);
			new_entity->model = render_data.models[0];
			// Add entity to scene root
			root->addChild(new_entity);
		}
	}
	

	std::cout << "Scene [OK]" << " Meshes added: " << mesh_count << std::endl;
	return true;
}