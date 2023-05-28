#pragma once
#include "camera.h"
#include "entity.h"
#include "texture.h"
#include "game.h"
#include "ui.h"

struct sRenderData {
	Texture* texture = nullptr;
	Shader* shader = nullptr;
	std::vector<Matrix44> models;
};

struct sCollisionData {
	Vector3 colPoint;
	Vector3 colNormal;
};


class World
{
public:
	Camera* camera;
	Camera* camera2D;
	bool freeCam = false;
	bool firstPerson = true;
	Entity* root;
	EntityMesh* sky;
	EntityPlayer* player;
	EntityArmy* enemies;
	bool uiEnabled = true;
	UI* ui;
	std::map<std::string, sRenderData> meshes_to_load;

	// stats
	int ENEMY_COUNT = 4;
	float MAX_VIEW_DISTANCE = 50.f;

	World(const char* sceneFilename);
	bool parseScene(const char* filename);
	void renderSky();
	void render();
	void update(double seconds_elapsed);
	void updateCamera(double seconds_elapsed);
	bool checkPlayerCollision(Vector3 target, std::vector<sCollisionData>* collisions);
	void checkCameraCollision(Vector3& target);
	bool checkLineOfSight(Matrix44& obs, Matrix44& target);
	void onResize(int width, int height);

private:
	void createSkybox();
	void createEnemies();
	void collisionCalculations(Mesh* mesh, Matrix44 model, Vector3 target, std::vector<sCollisionData>* collisions);

};

