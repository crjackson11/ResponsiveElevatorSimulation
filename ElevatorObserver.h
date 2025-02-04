#pragma once
#ifndef ElevatorObserver_h
#define ElevatorObserver_h

#include "ECObserver.h"
#include "ECGraphicViewImp.h"
#include "ECElevatorSim.h"
#include <vector>
#include <utility>

class ElevatorObserver : public ECObserver {
public:
	ElevatorObserver(ECGraphicViewImp& displayManager, ECElevatorSim& simulator, int timeToSim);  //includes simulator and simulation time now
	virtual void Update();

private:
	ECGraphicViewImp& displayManager;
	ECElevatorSim& simulatorRef;  //reference to the simulator

	//New part 3 variables
	int simTimeLength; //total simulation time
	int currTime;  //current simulation time
	bool isPaused;  //used to pause/resume the simulation

	int floorCount;  //number of floors in the Elevator display
	int currPos;     //current position of the Elevator
	double positionOffset;   //used for fluid movement
	int headCount;   //number of passengers in the Elevator
	std::vector<std::pair<int, int>> queue;  //the passengers waiting at each floor (up and down)

	//helper functions for drawing display
	void RenderShaft();   //draws the shaft of the Elevator
	void RenderElevator();  //draws the Elevator itself (moving square)
	void RenderFloors();   //draws the floor lines to distinguish between floors
	void ProcessInputs();  //handles the input of the user
	void DrawPassenger(int x, int y, int destFloor); // draws the passenger
};

#endif