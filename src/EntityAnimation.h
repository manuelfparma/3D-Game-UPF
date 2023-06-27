#pragma once
#include "animation.h"
enum AnimatedEntities {NINJA_ANIMATIONS, ENEMY_ANIMATIONS};
enum NinjaStates { NINJA_IDLE, NINJA_IDLE_CROUCH, NINJA_LEFT, NINJA_RIGHT, NINJA_FRONT, NINJA_FRONT_CROUCH, NINJA_BACK, NINJA_BACK_CROUCH, NINJA_JUMP, NINJA_FALL };
enum EnemyStates { ENEMY_PATROL, ENEMY_CHASE, ENEMY_ALERT};

class EntityAnimation
{
public:
	EntityAnimation(AnimatedEntities type);
	void update(float seconds_elapsed);
	void addAnimationState(const char* path, int state);
	void goToState(int state, float time = 0.f, int type=NINJA_ANIMATIONS);

	Skeleton& getCurrentSkeleton();
private:
	std::map<int, Animation*> animation_states;
	Skeleton blended_skeleton;
	int current_state = -1;
	int target_state = -1;

	float transition_counter = 0.f;
	float transition_time = 0.f;
};

