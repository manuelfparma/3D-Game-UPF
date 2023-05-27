#include "entity.h"
#include "camera.h"
#include "input.h"
#include "world.h"
#include "game.h"
#include "stage.h"
#include <iostream>

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
		for (auto& model : models)
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
	this->models = models;
}


EntityCollider::EntityCollider(bool isDynamic, int layer ) {
	isDynamic = isDynamic;
	layer = layer;
}

EntityPlayer::EntityPlayer() : EntityCollider(true, CHARACTER) {
	// set initial position
	model.setTranslation(initial_pos);

	// Model
	mesh = Mesh::Get("data/models/ninja.obj");
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/material.fs");
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
	if (Input::isKeyPressed(SDL_SCANCODE_W)) move_dir += move_front;
	if (Input::isKeyPressed(SDL_SCANCODE_A)) move_dir += move_right;
	if (Input::isKeyPressed(SDL_SCANCODE_D)) move_dir -= move_right;
	if (Input::isKeyPressed(SDL_SCANCODE_S)) move_dir -= move_front;
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT))  curSpeed *= crouch_factor;

	if (move_dir.x != 0 || move_dir.y != 0 || move_dir.z != 0)
		moving = true;

	if (move_dir.length() > 0.01) move_dir.normalize();
	velocity += move_dir * curSpeed;

	std::vector<sCollisionData> collisions;
	World* world = Game::instance->stageManager->currentStage->world;

	// we suppose the player is on the air
	bool isOnFloor = false;

	// check collisions
	if (world->checkPlayerCollision(position + velocity * seconds_elapsed, &collisions)) {
		for (const sCollisionData& collision : collisions) {
			// check for floor collisions
			float up_factor = collision.colNormal.dot(Vector3(0, 1, 0));

			if (up_factor > 0.8) {
				isOnFloor = true;
				velocity.y = 0;
			}
			else {
				Vector3 push = velocity.dot(collision.colNormal) * collision.colNormal;
				velocity -= push;
			}
		}
	}

	if (!isOnFloor) {
		// gravitational pull
		velocity.y -= gravity_speed * seconds_elapsed;
	}else if (Input::wasKeyPressed(SDL_SCANCODE_SPACE)) {
		// jumping
		velocity.y = jump_speed;
	}

	position += velocity * seconds_elapsed;
	// floor friction
	velocity.x *= pow(floor_friction, seconds_elapsed);
	velocity.z *= pow(floor_friction, seconds_elapsed);

	model.setTranslation(position);
	if (moving || world->firstPerson)
		lastYaw = yaw;
	model.rotate(lastYaw, Vector3(0, 1, 0));
}


bool EntityCollider::testCollision(EntityCollider* other) {
	return false;
}

bool EntityCollider::testCollision(std::vector<EntityCollider*> vector) {
	return false;
}