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

		virtual void render() {};
		virtual void update(double seconds_elapsed) {};
		virtual void onResize(int width, int height);
		virtual void onEnter(int enterCode) {};
		virtual void onExit(int exitCode) {};
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
	Shader* shader = Shader::Get("data/shaders/gui.vs", "data/shaders/hud.fs");
	Mesh* background = new Mesh();
	IntroStage();
	virtual void render() override;
	virtual void update(double seconds_elapsed) override;
	virtual void onEnter(int enterCode) override;
	virtual void onExit(int exitCode) override;
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
	Shader* shader = Shader::Get("data/shaders/gui.vs", "data/shaders/hud.fs");
	Mesh* background = new Mesh();
	OutroStage();
	virtual void render() override;
	virtual void update(double seconds_elapsed) override;
	virtual void onEnter(int enterCode) override;
	virtual void onExit(int exitCode) override;
};