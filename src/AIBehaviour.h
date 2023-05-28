#pragma once
#include "framework.h"

typedef enum {
	SEARCH_STATE,
	FOUND_STATE
}
AI_ACTIONS;

class AIBehaviour
{
public:
	AI_ACTIONS current_state = SEARCH_STATE;

	void update(Matrix44* mModel, float seconds_elapsed);

	AIBehaviour() {};
};
