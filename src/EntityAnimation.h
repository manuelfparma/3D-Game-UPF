#pragma once
#include "animation.h"
enum PlayerStates { PLAYER_IDLE, PLAYER_WALK, PLAYER_RUN, PLAYER_JUMP, PLAYER_CROUCH, PLAYER_DASH, PLAYER_DANCE };

class EntityAnimation
{
public:
	EntityAnimation();
	void update(float seconds_elapsed);
	void addAnimationState(const char* path, int state);
	void goToState(int state, float time = 0.f);

	Skeleton& getCurrentSkeleton();
private:
	std::map<int, Animation*> animation_states;
	Skeleton blended_skeleton;
	int current_state = -1;
	int target_state = -1;

	float transition_counter = 0.f;
	float transition_time = 0.f;
};

