#pragma once
#include "animation.h"
enum NinjaStates { NINJA_IDLE, NINJA_IDLE_CROUCH, NINJA_RUN, NINJA_CROUCH_MOVE, NINJA_BACKWARDS, NINJA_JUMP, NINJA_POINT };

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

