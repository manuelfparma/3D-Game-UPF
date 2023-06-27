#include "entity.h"
#include "camera.h"
#include "input.h"
#include "world.h"
#include "game.h"
#include "stage.h"
#include <iostream>
#include "animation.h"
#include "EntityAnimation.h"

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
	shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs");

	walking_sound = Audio::Get("walk");
	slow_walking = Audio::Get("slow");
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
	marching_sound = Audio::Get("marching");
}


void EntityPlayer::onTouchFloor() {
	dashes = max_dashes;
}

void EntityPlayer::loseLife() {
	if (damage_cooldown > 0)
		return;
	lives--;
	damage_cooldown = DAMAGED_TIME;
	std::cout << "LIFE LOST! Remaining: " << lives << std::endl;
	Audio::Play("hurt");
}

enum Directions { FRONT_DIR, BACK_DIR, LEFT_DIR, RIGHT_DIR, IDLE_DIR };

int getDirection(const Vector3& move_dir, const Vector3& move_right, const Vector3& move_front) {
	float dotRight = move_right.dot(move_dir);
	float dotForward = move_front.dot(move_dir);

	if (dotRight > 0 && std::abs(dotRight) > std::abs(dotForward))
	{
		return RIGHT_DIR;
	}
	else if (dotRight < 0 && std::abs(dotRight) > std::abs(dotForward))
	{
		return LEFT_DIR;
	}
	else if (dotForward > 0 && std::abs(dotForward) >= std::abs(dotRight))
	{
		return FRONT_DIR;
	}
	else if (dotForward < 0 && std::abs(dotForward) >= std::abs(dotRight))
	{
		return BACK_DIR;
	}

	return IDLE_DIR;
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

	int animation_state = -1;

	if (Input::isKeyPressed(SDL_SCANCODE_W)) move_dir += move_front;
	if (Input::isKeyPressed(SDL_SCANCODE_A)) move_dir += move_right;
	if (Input::isKeyPressed(SDL_SCANCODE_D)) move_dir -= move_right;
	if (Input::isKeyPressed(SDL_SCANCODE_S)) move_dir -= move_front;

	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) { 
		if (!crouching && sound_playing) {
			walking_sound->pause();
			sound_playing = false;
		}

		curSpeed *= crouch_factor; 
		crouching = true;

	}
	else {
		if (crouching && sound_playing) {
			slow_walking->pause();
			sound_playing = false;
		}
		crouching = false;
	}

	if (move_dir.length() > 0) {
		moving = true;
	}

	if (move_dir.length() > 0.01) move_dir.normalize();
	velocity += move_dir * curSpeed;

	std::vector<sCollisionData> collisions;
	World* world = Game::instance->stageManager->currentStage->world;

	// we suppose the player is on the air
	bool onFloor = false;

	// dashing and jumping are not available in certain cases
	if (!landlocked) {

		if (Input::wasKeyPressed(SDL_SCANCODE_Q)) {
			// dashing
			if (dashes) {

				if (stamina >= dash_cost) {
					animation_state = NINJA_FRONT;
					stamina -= dash_cost;
					velocity += move_front * dash_speed;
					dashes -= 1;
					Audio::Play("dash");
				}
			}
		}

		if (Input::wasKeyPressed(SDL_SCANCODE_SPACE)) {
			// jumping
			if (jumps) {
				if (stamina >= jump_cost) {
					animation_state = NINJA_JUMP;
					stamina -= jump_cost;
					velocity.y = jump_speed;
					jumps -= 1;
					Audio::Play("jump");
				}
			}
		}
	}


	if (Input::wasKeyPressed(SDL_SCANCODE_F) && !invisible) {
		// invisibility
		if (stamina >= invisible_cost && invisible_time <= 0) {
			stamina -= invisible_cost;
			invisible = true;
			texture = Texture::getWhiteTexture();
			// activated time
			invisible_time = INVISIBLE_COOLDOWN;
			Audio::Play("sneak");
		}
	}
	else {
		if (invisible_time > 0)
			invisible_time -= seconds_elapsed;
		else if (invisible) {
			invisible = false;
			texture = Texture::Get("data/textures/ninja_texture.tga");
			// cooldown time
			invisible_time = INVISIBLE_COOLDOWN * 2;
		}
	}


	// check collisions
	if (world->checkPlayerCollision(position + velocity * seconds_elapsed, &collisions)) {
		for (const sCollisionData& collision : collisions) {
			// check for floor collisions
			float up_factor = collision.colNormal.dot(Vector3(0, 1, 0));

			if (up_factor > 0.8) {
				
				position.y = collision.colPoint.y - 0.05;

				onFloor = true;
				dashes = max_dashes;
				jumps = max_jumps;
				velocity.y = 0;

			}
			else {
				Vector3 push = velocity.dot(collision.colNormal) * collision.colNormal;
				velocity -= push;
			}
		}
	}


	if (!onFloor) {
		// gravitational pull
		velocity.y -= gravity_speed * seconds_elapsed;
	}

	Vector3 aux(move_dir.x, 0, move_dir.y);
	if (onFloor && aux.length() > 0) {
		if (!sound_playing) {
			if (crouching)
				slow_walking->play(1.0);
			else
				walking_sound->play(1.0);
			sound_playing = true;
		}
	}
	else if (sound_playing) {
		walking_sound->pause();
		slow_walking->pause();
		sound_playing = false;
	}

	position += velocity * seconds_elapsed;
	// floor friction
	velocity.x *= floor_friction;
	velocity.z *= floor_friction;

	// stamina regen
	stamina = clamp(stamina + stamina_growth * seconds_elapsed, 0, 100);

	model.setTranslation(position);
	if (moving || world->firstPerson)
		lastYaw = yaw;
	model.rotate(lastYaw, Vector3(0, 1, 0));

	// invincibility cooldown
	damage_cooldown = max(damage_cooldown - seconds_elapsed, 0.f);


	if (animation_state == -1) {
		if (!onFloor) {
			animation_state = NINJA_FALL;
		}
		else {
			int direction = getDirection(move_dir, move_right, move_front);

			switch (direction) {
			case IDLE_DIR:
				animation_state = (crouching) ? NINJA_IDLE_CROUCH: NINJA_IDLE;
				break;
			case FRONT_DIR:
				animation_state = (crouching) ? NINJA_FRONT_CROUCH : NINJA_FRONT;
				break;
			case BACK_DIR:
				animation_state = (crouching) ? NINJA_BACK_CROUCH : NINJA_BACK;
				break;
			case LEFT_DIR:
				animation_state = (crouching) ? NINJA_FRONT_CROUCH : NINJA_LEFT;
				break;
			case RIGHT_DIR:
				animation_state = (crouching) ? NINJA_FRONT_CROUCH : NINJA_RIGHT;
				break;
				
			}

		}
		playerAnimation->goToState(animation_state);
	}
	else {
		playerAnimation->goToState(animation_state);
	}

}

void EntityPlayer::render() {

	playerAnimation->update(Game::instance->elapsed_time);

	if (invisible) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		color = Vector4(1, 1, 1, 0.5);
	}
	else
		color = Vector4(1, 1, 1, 1);
	
	shader->enable();
	shader->setUniform("u_color", color);
	shader->setUniform("u_texture", texture, 0);
	shader->setUniform("u_viewprojection", Camera::current->viewprojection_matrix);
	shader->setUniform("u_model", getGlobalMatrix());
	mesh->renderAnimated(GL_TRIANGLES, &playerAnimation->getCurrentSkeleton());
	shader->disable();

	glDisable(GL_BLEND);
}

void EntityArmy::update(float seconds_elapsed) {
	bool playerSeen = false;
	int animation_state;
	float closest_to_player = 99999999.f;
	Vector3 player = Game::getPlayerPosition();

	for (int i = 0; i < models.size(); ++i){
		// get model matrix of current enemy
		Matrix44* mModel = &models[i];
		// update state machine of current enemy
		stateMachines[i].update(seconds_elapsed);
		// check if player was found
		if (stateMachines[i].state == FOUND_STATE) {
			playerSeen = true;
			animation_state = ENEMY_CHASE;
		}
		// move enemy (state machine updates orientation)
		if (stateMachines[i].isMoving && !(playerSeen && !onAlert)) {
			mModel->translate(0, 0, moveSpeed * seconds_elapsed);

			float dist = (mModel->getTranslation() - player).length();

			if (dist < closest_to_player) {
				closest_to_player = dist;
			}

		}
	}

	if (!(playerSeen && !onAlert)) {
		// we colud not make 3D audio work
		marching_sound->play(clamp(1 - (closest_to_player / 50), 0.0, 0.8));
	}
	else {
		marching_sound->pause();
	}

	if (playerSeen) {
		seenCooldown -= seconds_elapsed;
		if (seenCooldown < 0)
			onAlert = true;
		else animation_state = ENEMY_ALERT;
	}
	else {
		seenCooldown = ATTENTION_TIME;
		onAlert = false;
		animation_state = ENEMY_PATROL;
	}

	armyAnimation->goToState(animation_state, 0.0F, ENEMY_ANIMATIONS);
}

void EntityArmy::render() {
	// Get the last camera that was activated 
	Camera* camera = Camera::current;

	armyAnimation->update(Game::instance->elapsed_time);

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
	mesh->renderInstancedAnimated(GL_TRIANGLES, normal_render.data(), normal_render.size(), &armyAnimation->getCurrentSkeleton());

	// now we render marked enemies after clearing z-buffer
	color = onAlert ? FOUND_COLOR : SEARCH_COLOR;
	shader->setUniform("u_color", color);
	if (texture) shader->setTexture("u_texture", texture, 0);

	glClear(GL_DEPTH_BUFFER_BIT);
	mesh->renderInstancedAnimated(GL_TRIANGLES, marked_render.data(), marked_render.size(), &armyAnimation->getCurrentSkeleton());

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