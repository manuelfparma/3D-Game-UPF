#include "AIBehaviour.h"
#include "game.h"
#include "world.h"
#include "stage.h"
#include "extra/pathfinder/PathFinder.h"
#include <fstream>

void WayPoint::addLink(WayPoint* other, float distance) {
	this->addChild(other, distance);
	other->setParent(this);
}

AIBehaviour::AIBehaviour(Matrix44* mModel, int i) {
	this->mModel = mModel;

	// parse enemy path file with waypoints info
	if (!parseEnemyPath(i)) {
		state = DEAD_STATE;
		return;
	}

	/* Code for applying Dijkstra to path
	// create pathfinder and algorithm
	PathFinder<WayPoint> p;
	p.setStart(waypoints.front());
	p.setGoal(waypoints.back());

	// check if a path is found
	if (p.findPath<Dijkstra>(current_path)) {
		current_destination = current_path.begin();
		rotateEnemyToNewPoint((*current_destination)->position);
	}
	*/

	// for now, paths are cyclical
	for (auto &point : waypoints) {
		path.push_back(&point);
	}

	destination = path.begin();
	Vector3 initial_pos = (*destination)->position;
	initial_pos.y = 0;
	mModel->setTranslation(initial_pos);
	rotateEnemyToNewPoint((*destination)->position);
}

bool AIBehaviour::parseEnemyPath(int i) {
	std::vector<Vector3> points;

	char filename[50];
	sprintf(filename, "data/paths/enemy%d.txt", i);

	std::ifstream file(filename);

	if (!file.good()) {
		std::cerr << "Enemy [ERROR]" << filename << " not found!" << std::endl;
		return false;
	}

	std::string posX, posY, posZ;

	while (file >> posX >> posY >> posZ) {
		points.push_back(Vector3(
			(float) atof(posX.c_str()),
			(float) atof(posY.c_str()),
			(float) atof(posZ.c_str())
		));
	}

	if (points.empty()) return false;

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

	return true;
}

void AIBehaviour::update(float seconds_elapsed) {
	World* world = Game::instance->stageManager->currentStage->world;
	Vector3 player = world->player->model.getTranslation();

	isMoving = true;

	switch (state) 
	{
	case SEARCH_STATE:
		// check if we can see the player
		if (world->checkLineOfSight(*mModel, player))
			state = FOUND_STATE;
		// check current path movement and orientation
		else if (checkPointProximity((*destination)->position)) {
			++destination;
			if (destination == path.end()) {
				// path has finished
				destination = path.begin();
			}
			rotateEnemyToNewPoint((*destination)->position);
		}
		break;

	case FOUND_STATE:
		rotateEnemyToNewPoint(player);

		if (world->checkLineOfSight(*mModel, player)) {
			// if we are close to the player, stop moving
			isMoving = !checkPointProximity(player);
		}
		else {
			// else, return to search state
			state = SEARCH_STATE;
			// find closest point in path and go there
			findClosestWayPoint();
			rotateEnemyToNewPoint((*destination)->position);
		}
		break;

	case DEAD_STATE:
		isMoving = false;
		break;
	}
}

bool AIBehaviour::checkPointProximity(Vector3 point) {
	return (mModel->getTranslation() - point).length() < EPSILON;
}

void AIBehaviour::rotateEnemyToNewPoint(Vector3 point) {
	float yaw = mModel->getYawRotationToAimTo(point);
	mModel->rotate(yaw, Vector3(0, 1, 0));
}

void AIBehaviour::findClosestWayPoint() {
	Vector3 currentPos = mModel->getTranslation();

	auto it = path.begin();
	double min = 999999.0;

	while (it != path.end()) {
		double dist = ((*it)->position - currentPos).length();
		if (dist < min) {
			min = dist;
			destination = it;
		}
		++it;
	}
}
