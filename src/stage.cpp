#include "stage.h"
#include "game.h"
#include "animation.h"
#include "input.h"



/* Constructors */
StageManager::StageManager() {
	stages.push_back(new IntroStage());
	stages.push_back(new PlayStage());
	stages.push_back(new OutroStage());
    currentStage = stages[INTRO_STAGE];
}

Stage::Stage() {
}

IntroStage::IntroStage() : Stage() {
    type = INTRO_STAGE;
    world = new World("data/myscene.scene");

    /*
    EntityMesh* ambulance_meshed = new EntityMesh();
    ambulance_meshed->mesh = Mesh::Get("data/ambulance.obj");
    ambulance_meshed->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/color.fs");
    ambulance = (Entity*)ambulance_meshed;

    world->root->addChild(ambulance);
    */
}

PlayStage::PlayStage() {
    type = PLAY_STAGE;
}

OutroStage::OutroStage() {
    type = OUTRO_STAGE;
}


/* StageManager definitions */
void StageManager::changeStage(Stage* newStage, int exitCode) {
    StageType currentStageType = this->currentStage->type;
    StageType nextStageType = newStage->type;

    currentStage->onExit(exitCode);
    this->currentStage = newStage;
    currentStage->onEnter(exitCode);
}

    
// changeStage(stages[NEW_STAGE], exitCode);

void StageManager::render() {
    currentStage->render();
}

void StageManager::update(double seconds_elapsed) {
    currentStage->update(seconds_elapsed);
};



/* onEnter functions */
void IntroStage::onEnter(int enterCode) {
    switch (enterCode) {
    default:
        break;
    }
}

void PlayStage::onEnter(int enterCode) {


    switch (enterCode) {
    default:
        break;
    }
}

void OutroStage::onEnter(int enterCode) {

    switch (enterCode) {
    default:
        break;
    }
}


/* onLeave functions */
void IntroStage::onExit(int exitCode) {

    switch (exitCode) {
    default:
        break;
    }
}

void PlayStage::onExit(int exitCode) {

    switch (exitCode) {
    default:
        break;
    }
}

void OutroStage::onExit(int exitCode) {

    switch (exitCode) {
    default:
        break;
    }
}

/* render functions */
void IntroStage::render() {
    world->render();
}

void PlayStage::render() {
    world->render();
}

void OutroStage::render() {
    world->render();
}

/* update functions */
void IntroStage::update(double seconds_elapsed) {

    world->update(seconds_elapsed);

};

void PlayStage::update(double seconds_elapsed) {
};

void OutroStage::update(double seconds_elapsed) {
}