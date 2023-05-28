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
		World* world;

		virtual void render() {};
		virtual void update(double seconds_elapsed) {};
		virtual void onEnter(int enterCode) {};
		virtual void onExit(int exitCode) {};
};

class StageManager {
public:
	std::vector<Stage*> stages;
	Stage* currentStage;
	StageManager();
	void changeStage(Stage* newState, int exitCode);
	void render();
	void update(double seconds_elapsed);
};

class IntroStage : public Stage {
public:
	IntroStage();
	virtual void render() override;
	virtual void update(double seconds_elapsed) override;
	virtual void onEnter(int enterCode) override;
	virtual void onExit(int exitCode) override;
};

class PlayStage : public Stage {
public:
	PlayStage();
	virtual void render() override;
	virtual void update(double seconds_elapsed) override;
	virtual void onEnter(int enterCode) override;
	virtual void onExit(int exitCode) override;
};

class OutroStage : public Stage {
public:
	OutroStage();
	virtual void render() override;
	virtual void update(double seconds_elapsed) override;
	virtual void onEnter(int enterCode) override;
	virtual void onExit(int exitCode) override;
};