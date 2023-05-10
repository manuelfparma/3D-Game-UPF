#include "entity.h"
#include "camera.h"

void Entity::render()
{
	// Render all children
	for (int i = 0; i < children.size(); i++)
		children[i]->render();
}

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

void EntityMesh::render()
{
	// Get the last camera that was activated 
	Camera* camera = Camera::current;

	// Enable shader and pass uniforms 
	shader->enable();
	shader->setUniform("u_model", model);
	shader->setUniform("u_viewproj", camera->viewprojection_matrix);
	shader->setTexture("u_texture", texture, 0);	//TODO: que slot?

	// Render the mesh using the shader
	mesh->render(GL_TRIANGLES);

	// Disable shader after finishing rendering
	shader->disable();

	// Call children render
	Entity::render();
}
