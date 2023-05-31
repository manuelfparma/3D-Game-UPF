#include "EntityAnimation.h"
#include "game.h"

void EntityAnimation::addAnimationState(const char* path, int state) {
	animation_states[state] = Animation::Get(path);
}

void EntityAnimation::goToState(int state, float time) {
	if (time == 0.0f) {
		current_state = state;
		return;
	}

	if (target_state == state) {
		return;
	}

	transition_counter = 0.0f;
	transition_time = time;
	target_state = state;
}

void EntityAnimation::update(float seconds_elapsed) {
	animation_states[current_state]->assignTime(Game::instance->time);

	if (target_state != -1) {
		animation_states[target_state]->assignTime(Game::instance->time);
		transition_counter += seconds_elapsed;

		if (transition_counter >= transition_time) {
			current_state = target_state;
			target_state = -1;
			return;
		}

		blendSkeleton(
			&animation_states[current_state]->skeleton,
			&animation_states[target_state]->skeleton,
			transition_counter/transition_time,
			&blended_skeleton
		);
	}
}


Skeleton& EntityAnimation::getCurrentSkeleton()
{
	if (target_state != -1) {
		return blended_skeleton;
	}

	return animation_states[current_state]->skeleton;
}