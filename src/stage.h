#pragma once
#include "entity.h"
#include "camera.h"
#include "world.h"

enum StageType {
	INTRO_STAGE,
	PLAY_STAGE,
	OUTRO_STAGE,
	NO_STAGE
};

class Stage
{
	public:
		Stage();
		StageType type;
		Camera* camera2D;

		Shader* shader = Shader::Get("data/shaders/gui.vs", "data/shaders/hud.fs");
		Mesh* background = new Mesh();

		virtual void render() {};
		virtual void update(double seconds_elapsed) {};
		virtual void onResize(int width, int height);
		virtual void onEnter(int enterCode) {};
		virtual void onExit(int exitCode) {};
		bool addButton(float center_x, float center_y, float w, float h, const char* filename);
};

class StageManager {
public:
	std::vector<Stage*> stages;
	Stage* currentStage;
	StageManager();
	void changeStage(StageType newState, int exitCode);
	void render();
	void update(double seconds_elapsed);
};

class IntroStage : public Stage {
public:
	bool start = false;

	IntroStage();

	void createQuads(int width, int height);
	virtual void render() override;
	virtual void update(double seconds_elapsed) override;
	virtual void onEnter(int enterCode) override;
	virtual void onExit(int exitCode) override;
	virtual void onResize(int width, int height) override;
};

class PlayStage : public Stage {
public:
	World* world;
	PlayStage();
	virtual void render() override;
	virtual void update(double seconds_elapsed) override;
	virtual void onEnter(int enterCode) override;
	virtual void onExit(int exitCode) override;
	virtual void onResize(int width, int height) override;
};

class OutroStage : public Stage {
public:
	std::string text;
	OutroStage();
	virtual void render() override;
	virtual void update(double seconds_elapsed) override;
	virtual void onEnter(int enterCode) override;
	virtual void onExit(int exitCode) override;
};