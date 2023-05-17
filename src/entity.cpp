#include "entity.h"
#include "camera.h"
#include "input.h"

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

void EntityMesh::render()
{
	// Get the last camera that was activated 
	Camera* camera = Camera::current;
	// Enable shader and pass uniforms 
	shader->enable();
	shader->setUniform("u_color", color);
	shader->setUniform("u_model", getGlobalMatrix());
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	if (texture) shader->setTexture("u_texture", texture, 0);

	// Render the mesh using the shader
	mesh->render(GL_TRIANGLES);

	// Disable shader after finishing rendering
	shader->disable();

	// Call children render
	Entity::render();
}

void InstancedEntityMesh::render()
{
	// Get the last camera that was activated 
	Camera* camera = Camera::current;
	// Enable shader and pass uniforms
	
	shader->enable();
	shader->setUniform("u_color", color);
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	if (texture) shader->setTexture("u_texture", texture, 0);

	// Render the mesh using the shader
	mesh->renderInstanced(GL_TRIANGLES, models.data(), models.size());

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


EntityCollider::EntityCollider(bool isDynamic, int layer ) {
	isDynamic = isDynamic;
	layer = layer;
}

EntityPlayer::EntityPlayer() : EntityCollider(true, CHARACTER) {
	float yaw = 0.0f;
	float pitch = 0.0f;
	float roll = 0.0f;

	float speed = 10.0f;
	float jump_speed = 30.0f;

	float gravity_speed = 2.5f;

	Vector3 velocity;
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
	Vector3 eye;
	Vector3 center;
	Vector3 position = model.getTranslation();

	Vector3 move_dir;

	Vector3 move_right = mYaw.rightVector();
	Vector3 move_front = mYaw.frontVector();

	if (Input::isKeyPressed(SDL_SCANCODE_W)) move_dir = move_dir + move_front;
	if (Input::isKeyPressed(SDL_SCANCODE_D)) move_dir = move_dir + move_right;
	if (Input::isKeyPressed(SDL_SCANCODE_A)) move_dir = move_dir - move_right;
	if (Input::isKeyPressed(SDL_SCANCODE_S)) move_dir = move_dir - move_front;


	// if (move_dir.length()) move_dir.normalize();
	velocity = velocity + move_dir * speed;
	
	position = position + velocity * seconds_elapsed;

	/*
	velocity.x *= 0.5;
	velocity.z *= 0.5;
	*/
	model.setTranslation(position);
	model.rotate(yaw, Vector3(0, 1, 0));
}
