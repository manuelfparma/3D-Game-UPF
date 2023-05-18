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
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	if (texture) shader->setTexture("u_texture", texture, 0);
	
	if (isInstanced) {
		mesh->renderInstanced(GL_TRIANGLES, models.data(), models.size());
	}
	else {
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

EntityMesh::EntityMesh(Mesh* mesh, Texture* texture, Shader* shader, bool isInstanced, std::vector<Matrix44> models) : Entity() {
	this->mesh = mesh;
	this->texture = texture;
	this->shader = shader;
	this->isInstanced = isInstanced;
	this->models = models;
}


EntityCollider::EntityCollider(bool isDynamic, int layer ) {
	isDynamic = isDynamic;
	layer = layer;
}

EntityPlayer::EntityPlayer() : EntityCollider(true, CHARACTER) {

	// Rotation
	yaw = 0.0f;
	pitch = 0.0f;
	roll = 0.0f;

	// Movement
	speed = 10.0f;
	jump_speed = 300.0f;
	gravity_speed = 2.5f;
	velocity = Vector3(0,0,0);
	velocity_decrease_factor = 0.5;
	isOnFloor = true;

	// Model
	mesh = Mesh::Get("data/character.obj");
	shader = Shader::getDefaultShader("flat");
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

	float curSpeed = speed;
	//if (Input::isKeyPressed(SDL_SCANCODE_W)) 
	if (Input::isKeyPressed(SDL_SCANCODE_W)) move_dir = move_dir + move_front;
	if (Input::isKeyPressed(SDL_SCANCODE_A)) move_dir = move_dir + move_right;
	if (Input::isKeyPressed(SDL_SCANCODE_D)) move_dir = move_dir - move_right;
	if (Input::isKeyPressed(SDL_SCANCODE_S)) move_dir = move_dir - move_front;
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT))  curSpeed *= 0.3;

	if (!isOnFloor) {
		velocity.y -= 2.5;
	}
	else {
		if (Input::wasKeyPressed(SDL_SCANCODE_SPACE)) {
			velocity.y += jump_speed;
			isOnFloor = false;
		}
	}

	if (move_dir.length() > 0.01) move_dir.normalize();
	velocity += move_dir * curSpeed;
	
	position += velocity * seconds_elapsed;

	velocity.x *= 0.5;
	velocity.z *= 0.5;
	model.setTranslation(position);
	model.rotate(yaw, Vector3(0, 1, 0));
}


bool EntityCollider::testCollision(EntityCollider* other) {
	return false;
}

bool EntityCollider::testCollision(std::vector<EntityCollider*> vector) {
	return false;
}