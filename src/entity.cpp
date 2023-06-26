#include "entity.h"
#include "camera.h"
#include "input.h"
#include "world.h"
#include "game.h"
#include "stage.h"
#include <iostream>
#include "animation.h"

Matrix44 Entity::getGlobalMatrix()
{
	if (parent)
		return model * parent->getGlobalMatrix();

	return model;
}

void Entity::addChild(Entity* child)
{
	children.push_back(child);
	child->parent = this;
}

void Entity::removeChild(Entity* child)
{
	auto it = std::find(children.begin(), children.end(), child);
	if (it != children.end()) children.erase(it);
	child->parent = nullptr;
}


void Entity::render()
{
	// Render all children
	for (int i = 0; i < children.size(); i++) {
		children[i]->render();
	}
}

bool checkRender(Matrix44 model, Mesh *mesh) {
	Camera* camera = Camera::current;

	// Compute bounding sphere center 
	// in world coords
	Vector3 sphere_center = model * mesh->box.center;
	float sphere_radius = mesh->radius;

	// ignore objects whose bounding sphere 
	// is not inside the camera frustum
	if (camera->testSphereInFrustum(sphere_center, sphere_radius) == false)
		return false;

	return true;
}

void EntityMesh::render()
{
	// Get the last camera that was activated 
	Camera* camera = Camera::current;
	
	// Enable shader and pass uniforms 
	shader->enable();	
	shader->setUniform("u_color", color);
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	if (texture) shader->setTexture("u_texture", texture, 0);
	
	if (isInstanced) {
		// we will check which models to render
		std::vector<Matrix44> render_models;
		for (auto model : models)
			if (checkRender(model, mesh))
				render_models.push_back(model);
		mesh->renderInstanced(GL_TRIANGLES, render_models.data(), render_models.size());
	}
	// only render instance if inside fustrum
	else if (checkRender(model, mesh)){
		shader->setUniform("u_model", getGlobalMatrix());
		mesh->render(GL_TRIANGLES);
	}
	
	// Disable shader after finishing rendering
	shader->disable();	

	// Call children render
	Entity::render();
}


EntityMesh::EntityMesh(Mesh* mesh, Texture* texture, Shader* shader) : Entity() {
	this->mesh = mesh;
	this->texture = texture;
	this->shader = shader;
}

EntityMesh::EntityMesh(Mesh* mesh, Texture* texture, Shader* shader, std::vector<Matrix44> models) : Entity() {
	this->mesh = mesh;
	this->texture = texture;
	this->shader = shader;
	this->isInstanced = true;
	for (auto &model : models)
		this->models.push_back(model);
}


EntityCollider::EntityCollider(bool isDynamic, COLISSION_LAYER layer) {
	isDynamic = isDynamic;
	layer = layer;
}

EntityPlayer::EntityPlayer() : EntityCollider(true, PLAYER) {


	dashes = max_dashes;
	jumps = max_jumps;

	// set initial position
	model.setTranslation(initial_pos);

	// Model
	//mesh = Mesh::Get("data/models/ninja.obj");
	mesh = Mesh::Get("data/models/ninja_reuv.MESH");
	texture = Texture::Get("data/textures/ninja_texture.tga");
	//shader = Shader::Get("data/shaders/basic.vs", "data/shaders/material.fs");
	shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs");
}

EntityArmy::EntityArmy(Mesh* mesh, Texture* texture, Shader* shader, std::vector<Matrix44> models)
	: EntityCollider(mesh, texture, shader, models)
{
	isDynamic = true;
	layer = ENEMY;
	color = SEARCH_COLOR;
	size = models.size();
	for (int i = 0; i < size; ++i)
		stateMachines.push_back(AIBehaviour(&this->models[i], i));
	marked.resize(size, false);
}


void EntityPlayer::onTouchFloor() {
	dashes = max_dashes;
}


void EntityPlayer::update(float seconds_elapsed){

	yaw -= Input::mouse_delta.x * seconds_elapsed * 10.f * DEG2RAD;
	pitch -= Input::mouse_delta.y * seconds_elapsed * 10.f * DEG2RAD;
	pitch = clamp(pitch, -M_PI * 0.3f, M_PI * 0.3f);

	Matrix44 mYaw;
	Matrix44 mPitch;

	mYaw.setRotation(yaw, Vector3(0, 1, 0));
	mPitch.setRotation(pitch, Vector3(-1, 0, 0));

	Vector3 front = (mPitch * mYaw).frontVector();
	Vector3 position = model.getTranslation();

	Vector3 move_dir(0.f, 0.f, 0.f);

	Vector3 move_right = mYaw.rightVector();
	Vector3 move_front = mYaw.frontVector();

	bool moving = false;

	float curSpeed = speed;

	int animation_state;

	if (Input::isKeyPressed(SDL_SCANCODE_W)) move_dir += move_front;
	if (Input::isKeyPressed(SDL_SCANCODE_A)) move_dir += move_right;
	if (Input::isKeyPressed(SDL_SCANCODE_D)) move_dir -= move_right;
	if (Input::isKeyPressed(SDL_SCANCODE_S)) move_dir -= move_front;
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) { 
		curSpeed *= crouch_factor; 

		if (move_dir.length() < 0.1) {
			animation_state = NINJA_IDLE_CROUCH;
		}
		else {
			animation_state = NINJA_CROUCH_MOVE;
		}
	}
	else {
		
			if (move_dir.length() < 0.1) {
				animation_state = NINJA_IDLE;
			}
			else {
				if (Input::isKeyPressed(SDL_SCANCODE_S)) {
					animation_state = NINJA_BACKWARDS;
				}
				else {
				animation_state = NINJA_RUN;
			}
			
		}
	}

	if (move_dir.x != 0 || move_dir.y != 0 || move_dir.z != 0)
		moving = true;

	if (move_dir.length() > 0.01) move_dir.normalize();
	velocity += move_dir * curSpeed;

	std::vector<sCollisionData> collisions;
	World* world = Game::instance->stageManager->currentStage->world;

	// we suppose the player is on the air
	bool onFloor = false;


	if (Input::wasKeyPressed(SDL_SCANCODE_Q)) {
		// dashing
		animation_state = NINJA_RUN;
		if (dashes){

			if (stamina >= dash_cost) {

				stamina -= dash_cost;
				velocity += move_front * dash_speed;
				dashes-=1;

			}
		}
	}

	if (Input::wasKeyPressed(SDL_SCANCODE_SPACE)) {
		// jumping
		animation_state = NINJA_JUMP;
		if (jumps) {
			if (stamina >= jump_cost) {

				stamina -= jump_cost;
				velocity.y = jump_speed;
				jumps -= 1;

			}
		}
	}


	// check collisions
	if (world->checkPlayerCollision(position + velocity * seconds_elapsed, &collisions)) {
		for (const sCollisionData& collision : collisions) {
			// check for floor collisions
			float up_factor = collision.colNormal.dot(Vector3(0, 1, 0));

			if (up_factor > 0.8) {
				
				onFloor = true;
				dashes = max_dashes;
				jumps = max_jumps;

				/* if (!isOnFloor) {
					onTouchFloor();
				}; */

				velocity.y = 0;

			}
			else {
				Vector3 push = velocity.dot(collision.colNormal) * collision.colNormal;
				velocity -= push;
			}
		}
	}

	//TODO: DEV
	if (Input::wasButtonPressed(SDL_SCANCODE_R)) {
		model.setTranslation(initial_pos);
	}


	if (!onFloor) {
		// gravitational pull
		animation_state = NINJA_FALLING;
		velocity.y -= gravity_speed * seconds_elapsed;
	}


	position += velocity * seconds_elapsed;
	// floor friction
	velocity.x *= pow(floor_friction, seconds_elapsed);
	velocity.z *= pow(floor_friction, seconds_elapsed);

	// stamina regen
	stamina = clamp(stamina + stamina_growth * seconds_elapsed, 0, 100);

	model.setTranslation(position);
	if (moving || world->firstPerson)
		lastYaw = yaw;
	model.rotate(lastYaw, Vector3(0, 1, 0));

	playerAnimation->goToState(animation_state, 0.0f);
	// isOnFloor = onFloor;

}

void EntityPlayer::render() {


	float elapsed_seconds = Game::instance->elapsed_time - game_time;
	game_time = Game::instance->elapsed_time;

	playerAnimation->update(elapsed_seconds);

	shader->enable();
	shader->setUniform("u_color", color);
	shader->setUniform("u_texture", texture, 0);
	shader->setUniform("u_viewprojection", Camera::current->viewprojection_matrix);
	shader->setUniform("u_model", getGlobalMatrix());
	mesh->renderAnimated(GL_TRIANGLES, &playerAnimation->getCurrentSkeleton());
	shader->disable();
	//EntityMesh::render();
}

void EntityArmy::update(float seconds_elapsed) {
	bool playerSeen = false;

	for (int i = 0; i < models.size(); ++i){
		// get model matrix of current enemy
		Matrix44* mModel = &models[i];
		// update state machine of current enemy
		stateMachines[i].update(seconds_elapsed);
		// check if player was found
		if (stateMachines[i].state == FOUND_STATE) {
			playerSeen = true;
		}
		// move enemy (state machine updates orientation)
		if (stateMachines[i].isMoving)
			mModel->translate(0, 0, moveSpeed * seconds_elapsed);
	}

	if (playerSeen) {
		seenCooldown -= seconds_elapsed;
		if (seenCooldown < 0)
			onAlert = true;
	}
	else {
		seenCooldown = ATTENTION_TIME;
		onAlert = false;
	}
}

void EntityArmy::render() {
	// Get the last camera that was activated 
	Camera* camera = Camera::current;

	// Enable shader and pass uniforms 
	shader->enable();
	shader->setUniform("u_color", Vector4(1.f, 1.f, 1.f, 1.f));
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	if (texture) shader->setTexture("u_texture", texture, 0);

	// First we will check which enemies are marked and which aren't
	std::vector<Matrix44> normal_render;
	std::vector<Matrix44> marked_render;

	for (int i = 0; i < models.size(); i++) {
		// if enemy is outside fulstrum, don't render
		if (!checkRender(models[i], mesh))
			continue;
		// if enemy is marked, we have to render them without z-buffer
		if (marked[i]) {
			marked_render.push_back(models[i]);
		}
		else {
			normal_render.push_back(models[i]);
		}
	}

	// first we render normal enemies
	mesh->renderInstanced(GL_TRIANGLES, normal_render.data(), normal_render.size());

	// now we render marked enemies after clearing z-buffer
	color = onAlert ? FOUND_COLOR : SEARCH_COLOR;
	shader->setUniform("u_color", color);
	if (texture) shader->setTexture("u_texture", texture, 0);

	glClear(GL_DEPTH_BUFFER_BIT);
	mesh->renderInstanced(GL_TRIANGLES, marked_render.data(), marked_render.size());

	// Disable shader after finishing rendering
	shader->disable();
}


EntityUI::EntityUI(Mesh* UImesh, Shader* UIshader, Texture* UItexture, Vector4 UIcolor) {
	mesh = UImesh;
	shader = UIshader;
	texture = UItexture;
	color = UIcolor;
}

void EntityUI::render() {

	Camera* camera = Camera::current;

	shader->enable();
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_model", model);
	shader->setUniform("u_color", color );
	shader->setUniform("u_texture", texture, 0);

	mesh->render(GL_TRIANGLES);

	shader->disable();
}


void EntityUI::update(float seconds_elapsed) {
}