#pragma once
#include "entity.h"
#include "camera.h"

enum StageType {
	INTRO_STAGE,
	PLAY_STAGE,
	OUTRO_STAGE,
	NO_STAGE
};

enum StageExitCode {
	EXIT_NONE = 0
};

struct UpdateStage {
	StageExitCode exitCode;
	StageType nextStage;
};

class Stage
{
	public:
		Stage();
		Camera* camera;
		StageType type;
		Entity* root;

		virtual void render() {};
		virtual UpdateStage	update(double seconds_elapsed) { return { EXIT_NONE, NO_STAGE }; };
		virtual void onEnter(StageExitCode enterCode) {};
		virtual void onExit(StageExitCode exitCode) {};
};

class StageManager {
public:
	std::vector<Stage*> stages;
	Stage* currentStage;
	StageManager();
	void changeStage(Stage* newState, StageExitCode exitCode);
	void render();
	void update(double seconds_elapsed);
};

class IntroStage : public Stage {
public:
	Entity* ambulance;

	IntroStage();
	virtual void render() override;
	virtual UpdateStage update(double seconds_elapsed) override;
	virtual void onEnter(StageExitCode enterCode) override;
	virtual void onExit(StageExitCode exitCode) override;
};

class PlayStage : public Stage {
public:
	PlayStage();
	virtual void render() override;
	virtual UpdateStage update(double seconds_elapsed) override;
	virtual void onEnter(StageExitCode enterCode) override;
	virtual void onExit(StageExitCode exitCode) override;
};

class OutroStage : public Stage {
public:
	OutroStage();
	virtual void render() override;
	virtual UpdateStage update(double seconds_elapsed) override;
	virtual void onEnter(StageExitCode enterCode) override;
	virtual void onExit(StageExitCode exitCode) override;
};