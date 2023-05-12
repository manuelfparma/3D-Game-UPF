#include "stage.h"
#include "game.h"
#include "animation.h"
#include "input.h"


//some globals
Animation* anim = NULL;
float angle = 0;
float mouse_speed = 100.0f;
FBO* fbo = NULL;

/* Constructors */
StageManager::StageManager() {
	stages.reserve(3);
	stages.push_back(new IntroStage());
	stages.push_back(new PlayStage());
	stages.push_back(new OutroStage());
    currentStage = stages[INTRO_STAGE];
}

Stage::Stage() {
    //create our camera
    camera = new Camera();
    camera->lookAt(Vector3(0.f, 100.f, 100.f), Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
    camera->setPerspective(70.f, Game::instance->window_width / (float)Game::instance->window_height, 0.1f, 10000.f); //set the projection, we want to be perspective
}

IntroStage::IntroStage() : Stage() {
    type = INTRO_STAGE;
    EntityMesh* ambulance_meshed = new EntityMesh();
    ambulance_meshed->mesh = Mesh::Get("data/ambulance.obj");
    ambulance_meshed->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/color.fs");
    ambulance = (Entity*)ambulance_meshed;

    // root->addChild(ambulance);
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
    ambulance->render();
}

void PlayStage::render() {
    root->render();
}

void OutroStage::render() {
    root->render();
}

/* update functions */
UpdateStage IntroStage::update(double seconds_elapsed) {

    float speed = seconds_elapsed * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant
    
    // rotate ambulance
    angle = (float)seconds_elapsed * 10.0f;


    ambulance->model.rotate(angle * DEG2RAD, Vector3(0, 1, 0));

    //mouse input to rotate the cam
    if ((Input::mouse_state & SDL_BUTTON_LEFT) || Game::instance->mouse_locked) //is left button pressed?
    {
        camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
        camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
    }

    //async input to move the camera around
    if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
    if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
    if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f, -1.0f) * speed);
    if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
    if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);

    //to navigate with the mouse fixed in the middle
    if (Game::instance->mouse_locked)
        Input::centerMouse();


    return { EXIT_NONE, NO_STAGE };
};

UpdateStage PlayStage::update(double seconds_elapsed) {
    return { EXIT_NONE, NO_STAGE };
};

UpdateStage OutroStage::update(double seconds_elapsed) {
    return { EXIT_NONE, NO_STAGE };
}