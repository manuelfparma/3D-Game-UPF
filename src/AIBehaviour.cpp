#include "AIBehaviour.h"
#include "game.h"
#include "world.h"
#include "stage.h"
#include "extra/pathfinder/PathFinder.h"

void WayPoint::addLink(WayPoint* other, float distance) {
	this->addChild(other, distance);
	other->setParent(this);
}

AIBehaviour::AIBehaviour(Matrix44* mModel) {
	this->mModel = mModel;

	std::vector<Vector3> points;
	points.push_back(Vector3(15, 0, 0));
	points.push_back(Vector3(15, 0, 15));
	points.push_back(Vector3(0, 0, 15));

	waypoints.resize(points.size());

	// points is a vector with the path of the enemy
	// we create linear path from this list
	for (int i = 0; i < points.size() - 1; ++i) {
		waypoints[i].position = points[i];
		waypoints[i + 1].position = points[i + 1];
		waypoints[i].addLink(
			&waypoints[i + 1],
			(points[i] - points[i + 1]).length()
		);
	}

	// create pathfinder and algorithm
	PathFinder<WayPoint> p;
	p.setStart(waypoints.front());
	p.setGoal(waypoints.back());

	// check if a path is found
	if (p.findPath<Dijkstra>(current_path)) {
		current_destination = current_path.begin();
		rotateEnemyToNewPoint((*current_destination)->position);
	}
}

void AIBehaviour::update(float seconds_elapsed) {
	World* world = Game::instance->stageManager->currentStage->world;
	Matrix44 player = world->player->model;

	switch (current_state) 
	{
	case SEARCH_STATE:
		// check if we can see the player
		if (world->checkLineOfSight(*mModel, player))
			current_state = FOUND_STATE;
		// check current path movement and orientation
		else if (checkWayPointProximity(*current_destination)) {
			++current_destination;
			if (current_destination == current_path.end()) {
				// path has finished
				current_destination = current_path.begin();
			}
			rotateEnemyToNewPoint((*current_destination)->position);
		}
		break;

	case FOUND_STATE:
		if (!world->checkLineOfSight(*mModel, player))
			current_state = SEARCH_STATE;
		break;
	}
}

bool AIBehaviour::checkWayPointProximity(WayPoint* point) {
	Vector3 currentPos = mModel->getTranslation();
	Vector3 nextPos = (*current_destination)->position;
	
	return (currentPos - nextPos).length() < EPSILON;
}

void AIBehaviour::rotateEnemyToNewPoint(Vector3 point) {
	float yaw = mModel->getYawRotationToAimTo(point);
	mModel->rotate(yaw, Vector3(0, 1, 0));
}
