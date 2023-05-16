#include "world.h"
#include "input.h"
#include "animation.h"

//some globals
Animation* anim = NULL;
float angle = 0;
float mouse_speed = 100.0f;
FBO* fbo = NULL;

World::World() {
    camera = new Camera();
    camera->lookAt(Vector3(0.f, 100.f, 100.f), Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
    camera->setPerspective(70.f, Game::instance->window_width / (float)Game::instance->window_height, 0.1f, 10000.f); //set the projection, we want to be perspective

    root = new Entity();
    player = new EntityPlayer();
}


void World::render() {

    Matrix44 mYaw;
    Matrix44 mPitch;

    mYaw.setRotation(player->yaw, Vector3(0, 1, 0));
    mPitch.setRotation(player->pitch, Vector3(-1, 0, 0));
    
    Vector3 front = (mPitch * mYaw).frontVector();
    Vector3 eye;
    Vector3 center;

    #define FIRST_PERSON false

    if (FIRST_PERSON) {
        eye = player->getGlobalMatrix() * Vector3(0.f, 2.f, 0.5f);
        center = eye + front;
    }
    else {
        eye = player->model.getTranslation() - front * 5.0f;
        center = player->getGlobalMatrix() * Vector3(0, 2.0f, 0.5f);
    }

    camera->lookAt(eye, center, Vector3(0, 1, 0));

    root->render();
}

void World::update(double seconds_elapsed) {
    // rotate ambulance
    angle = (float)seconds_elapsed * 10.0f;

    player->update(seconds_elapsed);

    //mouse input to rotate the cam
    /*if (Game::instance->mouse_locked ? true : (Input::mouse_state & SDL_BUTTON_LEFT)) //is left button pressed?
    {
        camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
        camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
    }
    */

    //to navigate with the mouse fixed in the middle
    if (Game::instance->mouse_locked)
        Input::centerMouse();
}


