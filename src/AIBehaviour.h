#pragma once
#include "framework.h"
#include "extra/pathfinder/Dijkstra.h"

typedef enum {
	SEARCH_STATE,
	FOUND_STATE,
	DEAD_STATE
}
AI_ACTIONS;

class WayPoint : public DijkstraNode {
public:
	Vector3 position;
	void addLink(WayPoint* other, float distance);
};

class AIBehaviour {
public:
	AI_ACTIONS state = SEARCH_STATE;
	std::vector<WayPoint> waypoints;

	std::vector<WayPoint*> path;
	std::vector<WayPoint*>::iterator destination;

	bool isMoving = true;

	void update(float seconds_elapsed);

	AIBehaviour(Matrix44* mModel, int i);
private:
	Matrix44* mModel; // pointer to model matrix of enemy

	// constants
	float EPSILON = 1.f;

	bool checkLineOfSight();
	bool checkPointProximity(Vector3 point);
	void rotateEnemyToNewPoint(Vector3 point);
	bool parseEnemyPath(int i);
	void findClosestWayPoint();
};
