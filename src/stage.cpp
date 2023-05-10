#include "stage.h"
#include "game.h"

/* Constructors */
StageManager::StageManager() {
	stages.reserve(3);
	stages.push_back(new IntroStage());
	stages.push_back(new PlayStage());
	stages.push_back(new OutroStage());
    currentStage = stages[INTRO_STAGE];
}

IntroStage::IntroStage() {
    type = INTRO_STAGE;
}

PlayStage::PlayStage() {
    type = PLAY_STAGE;
}

OutroStage::OutroStage() {
    type = OUTRO_STAGE;
}


/* StageManager definitions */
void StageManager::changeStage(Stage* newStage, StageExitCode exitCode) {
    StageType currentStageType = this->currentStage->type;
    StageType nextStageType = newStage->type;

    currentStage->onExit(exitCode);
    this->currentStage = newStage;
    currentStage->onEnter(exitCode);
}

void StageManager::render() {
    currentStage->render();
}

void StageManager::update(double seconds_elapsed) {
    UpdateStage updateExit = currentStage->update(seconds_elapsed);

    if (updateExit.exitCode != EXIT_NONE ) changeStage(stages[updateExit.nextStage], updateExit.exitCode);
};


/* onEnter functions */
void IntroStage::onEnter(StageExitCode enterCode) {
    switch (enterCode) {
    default:
        break;
    }
}

void PlayStage::onEnter(StageExitCode enterCode) {


    switch (enterCode) {
    default:
        break;
    }
}

void OutroStage::onEnter(StageExitCode enterCode) {

    switch (enterCode) {
    default:
        break;
    }
}


/* onLeave functions */
void IntroStage::onExit(StageExitCode exitCode) {

    switch (exitCode) {
    default:
        break;
    }
}

void PlayStage::onExit(StageExitCode exitCode) {

    switch (exitCode) {
    default:
        break;
    }
}

void OutroStage::onExit(StageExitCode exitCode) {

    switch (exitCode) {
    default:
        break;
    }
}

/* render functions */
void IntroStage::render() {
}

void PlayStage::render() {
}

void OutroStage::render() {
}

/* update functions */
UpdateStage IntroStage::update(double seconds_elapsed) {
    return { EXIT_NONE, NO_STAGE };
};

UpdateStage PlayStage::update(double seconds_elapsed) {
    return { EXIT_NONE, NO_STAGE };
};

UpdateStage OutroStage::update(double seconds_elapsed) {
    return { EXIT_NONE, NO_STAGE };
}