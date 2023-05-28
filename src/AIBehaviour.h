#pragma once
#include "framework.h"
#include "extra/pathfinder/Dijkstra.h"

typedef enum {
	SEARCH_STATE,
	FOUND_STATE
}
AI_ACTIONS;

class WayPoint : public DijkstraNode {
public:
	Vector3 position;
	void addLink(WayPoint* other, float distance);
};

class AIBehaviour {
public:
	AI_ACTIONS current_state = SEARCH_STATE;
	std::vector<WayPoint> waypoints;

	std::vector<WayPoint*> current_path;
	std::vector<WayPoint*>::iterator current_destination;

	void update(float seconds_elapsed);

	AIBehaviour(Matrix44* mModel);
private:
	Matrix44* mModel; // pointer to model matrix of enemy

	// constants
	float EPSILON = 1.f;

	bool checkWayPointProximity(WayPoint *point);
	void rotateEnemyToNewPoint(Vector3 point);
};
