#pragma once
#include "camera.h"
#include "entity.h"
#include "texture.h"
#include "game.h"
#include "ui.h"
#include "sound.h"

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
	bool firstPerson = false;
	Entity* root;
	EntityMesh* sky;
	EntityPlayer* player;
	EntityArmy* enemies;
	EntityCollider* collectible;
	bool uiEnabled = true;
	UI* ui;
	std::map<std::string, sRenderData> meshes_to_load;
	HCHANNEL bgMusic;

	// stats
	int ENEMY_COUNT = 5;
	float MAX_VIEW_DISTANCE = 50.f;

	World(const char* sceneFilename);
	bool parseScene(const char* filename);
	void renderSky();
	void render();
	void update(double seconds_elapsed);
	void updateCamera(double seconds_elapsed);
	bool checkPlayerCollision(Vector3 target, std::vector<sCollisionData>* collisions);
	void checkCameraCollision(Vector3& target);
	bool testCollisionAgainstWorld(Vector3 rayOrigin, Vector3 direction, float distance);
	bool checkCollectiblePickup();
	void checkEnemyMarking();
	void onResize(int width, int height);
private:
	void createSkybox();
	void createEnemies();
	void collisionCalculations(Mesh* mesh, Matrix44 model, Vector3 target, std::vector<sCollisionData>* collisions);

};

