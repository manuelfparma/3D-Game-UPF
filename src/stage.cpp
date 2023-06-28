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
    currentStage->onEnter(0);
}

Stage::Stage() {   
    camera2D = new Camera();
    camera2D->view_matrix = Matrix44();
    camera2D->setOrthographic(0, Game::instance->window_width, 0, Game::instance->window_height, -1, 1);
}

IntroStage::IntroStage() : Stage() {
    type = INTRO_STAGE;
}

PlayStage::PlayStage() {
    type = PLAY_STAGE;
    world = new World("data/myscene.scene");
}

OutroStage::OutroStage() {
    type = OUTRO_STAGE;
}


/* StageManager definitions */
void StageManager::changeStage(StageType newStage, int exitCode) {
    StageType currentStageType = this->currentStage->type;

    currentStage->onExit(exitCode);
    this->currentStage = stages[newStage];
    currentStage->onEnter(exitCode);
    SDL_ShowCursor(!Game::instance->mouse_locked);
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
    Game::instance->mouse_locked = false;

    background->createQuad(Game::instance->window_width / 2.f, Game::instance->window_height / 2.f, Game::instance->window_width, Game::instance->window_height, true);

    switch (enterCode) {
    default:
        break;
    }
}

void PlayStage::onEnter(int enterCode) {

    Game::instance->mouse_locked = true;

    switch (enterCode) {
    default:
        break;
    }
}

void OutroStage::onEnter(int enterCode) {
    Game::instance->mouse_locked = false;
    background->createQuad(Game::instance->window_width / 2.f, Game::instance->window_height / 2.f, Game::instance->window_width, Game::instance->window_height, true);

    switch (enterCode) {
    case 0:
        text = "You lose :(";
        break;
    case 1:
        text = "You win!";
        break;
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

    // Create into stage again
    Stage* newPlayStage = new PlayStage();
    Game::instance->stageManager->stages[PLAY_STAGE] = newPlayStage;

    switch (exitCode) {
    default:
        break;
    }
}

/* render functions */
void IntroStage::render() {

    Vector4 color = Vector4(1.0, 1.0, 1.0, 1.0);

    camera2D->enable();

    shader->enable();
    shader->setUniform("u_viewprojection", Camera::current->viewprojection_matrix);
    shader->setUniform("u_model", Matrix44());
    shader->setUniform("u_color", color);
    shader->setUniform("u_discard", true);
    shader->setUniform("u_discard_color", Vector3(0.0f, 0.0f, 0.0f));

    shader->setUniform("u_texture", Texture::Get("data/texture.tga"), 0);

    background->render(GL_TRIANGLES);
    drawText(Game::instance->window_width / 2, Game::instance->window_height / 2, "Press P to play", Vector3(1, 1, 1));

    shader->disable();
}

void PlayStage::render() {
    
    world->render();
}

void OutroStage::render() {

    Vector4 color = Vector4(1.0, 1.0, 1.0, 1.0);

    camera2D->enable();

    shader->enable();
    shader->setUniform("u_viewprojection", Camera::current->viewprojection_matrix);
    shader->setUniform("u_model", Matrix44());
    shader->setUniform("u_color", color);
    shader->setUniform("u_discard", true);
    shader->setUniform("u_discard_color", Vector3(0.0f, 0.0f, 0.0f));

    shader->setUniform("u_texture", Texture::Get("data/texture.tga"), 0);

    background->render(GL_TRIANGLES);

    shader->disable();

    drawText(Game::instance->window_width / 2, Game::instance->window_height / 2, text, Vector3(1, 1, 1));
    drawText(Game::instance->window_width / 2, Game::instance->window_height / 2 + 50, "Press R to restart", Vector3(1, 1, 1));

}

/* update functions */
void IntroStage::update(double seconds_elapsed) {

    if (Input::wasKeyPressed(SDL_SCANCODE_P)) {
        Game::instance->stageManager->changeStage(PLAY_STAGE, 0);
    }

};

void PlayStage::update(double seconds_elapsed) {
    world->update(seconds_elapsed);
};

void OutroStage::update(double seconds_elapsed) {

    if (Input::wasKeyPressed(SDL_SCANCODE_R)) {
        Game::instance->stageManager->changeStage(INTRO_STAGE, 0);
    }
}

void Stage::onResize(int width, int height) {
    camera2D->setOrthographic(0, width, 0, height, -1, 1);
}

void PlayStage::onResize(int width, int height) {
    world->onResize(width, height);
}