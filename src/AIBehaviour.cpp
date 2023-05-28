#include "AIBehaviour.h"
#include "game.h"
#include "world.h"
#include "stage.h"

void AIBehaviour::update(Matrix44* mModel, float seconds_elapsed) {
	World* world = Game::instance->stageManager->currentStage->world;
	Matrix44 player = world->player->model;

	switch (current_state) 
	{
	case SEARCH_STATE:
		// check if we can see the player
		if (world->checkLineOfSight(*mModel, player))
			current_state = FOUND_STATE;
		break;
	case FOUND_STATE:
		if (!world->checkLineOfSight(*mModel, player))
			current_state = SEARCH_STATE;
		break;
	}
}
