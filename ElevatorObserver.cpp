#include "ElevatorObserver.h"
#include "ECElevatorSim.h"
#include <string>

namespace {
	const int ELEVATOR_X = 50;   //position of Elevator from left
	const int ELEVATOR_Y = 50;   //position of Elevator from top
	const int ELEVATOR_W = 200;  //width of Elevator shaft
	const int FLOOR_HEIGHT = 70;  //height of each floor
}


//Constructor (now updated with newly implemented variables for part 3)
ElevatorObserver::ElevatorObserver(ECGraphicViewImp& displayManager, ECElevatorSim& simulator, int timeToSim) : displayManager(displayManager),
	simulatorRef(simulator),
	simTimeLength(timeToSim),
	currTime(0),
	isPaused(false),
	floorCount(simulator.GetNumFloors()),  //dynamically takes in floor count
	currPos(0),
	positionOffset(0.0),
	headCount(0)
	{
		queue.resize(floorCount, std::make_pair(0, 0));
	}


//Updates the Window displaying the output
void ElevatorObserver::Update() {
	ECGVEventType currentEvent = displayManager.GetCurrEvent();

	static int frameCount = 0;
	if (currentEvent == ECGV_EV_TIMER && !isPaused && currTime < simTimeLength) {
		frameCount++;

		if (frameCount >= 85) {
			simulatorRef.Simulate(currTime);
			currPos = simulatorRef.GetCurrFloor();
			currTime++;
			frameCount = 0;
		}

		// Smooth movement 
		double targetPos = std::max(0, currPos - 1);
		if (abs(positionOffset - targetPos) > 0.02) {
			if (positionOffset < targetPos) {
				positionOffset += 0.02;
			}
			else if (positionOffset > targetPos) {
				positionOffset -= 0.02;
			}
		}

		// Update display
		RenderShaft();
		RenderElevator();
		RenderFloors();

		std::string displayTime = "Time: " + std::to_string(currTime);
		displayManager.DrawText(ELEVATOR_X + ELEVATOR_W + 140, 90, displayTime.c_str(), ECGV_BLACK);

		displayManager.SetRedraw(true);
	}

	ProcessInputs();
}

//method for drawing the passengers, whether waiting or in the elevator cabin
void ElevatorObserver::DrawPassenger(int x, int y, int destFloor) {
	// Head
	displayManager.DrawFilledCircle(x, y - 15, 7, ECGV_WHITE);
	displayManager.DrawCircle(x, y - 15, 7, 2, ECGV_BLACK);

	// Body
	displayManager.DrawFilledRectangle(
		x - 7,
		y - 8,
		x + 7,
		y + 20,
		ECGV_WHITE
	);
	displayManager.DrawRectangle(
		x - 7,
		y - 8,
		x + 7,
		y + 20,
		2,
		ECGV_BLACK
	);

	// draw the passengers destination floor on them
	displayManager.DrawText(x, y + 5, std::to_string(destFloor).c_str(), ECGV_BLACK);
}

void ElevatorObserver::RenderShaft() {
	//fill background color
	displayManager.DrawFilledRectangle(0, 0, 500, 800, ECGV_YELLOW);

	//draw white interior of shaft
	displayManager.DrawFilledRectangle(
		ELEVATOR_X,
		ELEVATOR_Y,
		ELEVATOR_X + ELEVATOR_W,
		ELEVATOR_Y + (floorCount * FLOOR_HEIGHT),
		ECGV_WHITE
	);

	//draw black outline
	displayManager.DrawRectangle(
		ELEVATOR_X,
		ELEVATOR_Y,
		ELEVATOR_X + ELEVATOR_W,
		ELEVATOR_Y + (floorCount * FLOOR_HEIGHT),
		3,
		ECGV_BLACK
	);
}

void ElevatorObserver::RenderElevator() {
	//calculate right edge
	int rightBoundary = ELEVATOR_X + ELEVATOR_W;
	int bottomBoundary = ELEVATOR_Y + (floorCount * FLOOR_HEIGHT);

	// Reset queue states at start of each render
	for (int i = 0; i < floorCount; i++) {
		queue[i].first = 0;
		queue[i].second = 0;
	}

	//draw lines for each floor (horizontal)
	for (int i = 0; i <= floorCount; i++) {
		int currHeight = bottomBoundary - (i * FLOOR_HEIGHT);

		displayManager.DrawLine(
			ELEVATOR_X,        //from left edge
			currHeight,        //at current floor height
			rightBoundary,      //to right edge
			currHeight,         //same height
			2,                  //thickness of line
			ECGV_BLACK
		);

		if (i < floorCount) {
			int indicatorY = currHeight - FLOOR_HEIGHT / 2; //middle of floor

			// Set button states based on requests
			for (const auto& req : simulatorRef.GetRequests()) {
				if (!req.IsFloorRequestDone() && req.GetTime() <= currTime && (req.GetFloorSrc() == i + 1)) {
					if (req.GetFloorDest() > req.GetFloorSrc()) {
						queue[i].first = 1;  // Up button
					}
					else {
						queue[i].second = 1;  // Down button
					}
				}
			}

			// Up indicator
			if (queue[i].first) {
				displayManager.DrawFilledCircle(  //top circle (pressed)
					rightBoundary + 20,
					indicatorY - 10,
					8,
					ECGV_GREEN
				);
			}
			else {
				displayManager.DrawCircle(  //top circle
					rightBoundary + 20,
					indicatorY - 10,
					8,
					2,
					ECGV_BLACK
				);
			}

			// Down indicator
			if (queue[i].second) {
				displayManager.DrawFilledCircle(  //bottom circle (pressed)
					rightBoundary + 20,
					indicatorY + 10,
					8,
					ECGV_GREEN
				);
			}
			else {
				displayManager.DrawCircle(  //bottom circle
					rightBoundary + 20,
					indicatorY + 10,
					8,
					2,
					ECGV_BLACK
				);
			}

			// Draw all passengers in a line
			int totalPassengers = queue[i].first + queue[i].second;  // Total on this floor
			int drawnCount = 0;  // Keep track of passengers drawn

			// Draw all passengers with their destination floors
			for (const auto& request : simulatorRef.GetRequests()) {
				if (!request.IsFloorRequestDone() && request.GetFloorSrc() == i+1 && request.GetTime() <= currTime) {
					int floorY = bottomBoundary - ((request.GetFloorSrc() - 1) * FLOOR_HEIGHT) - FLOOR_HEIGHT / 2;
					DrawPassenger(rightBoundary + 45 + (drawnCount * 20),
						floorY, 
						request.GetFloorDest());
					drawnCount++;
				}
			}
		}
	}
}

void ElevatorObserver::RenderFloors() {
	//calculate bottom bound
	int bottomBoundary = ELEVATOR_Y + (floorCount * FLOOR_HEIGHT);

	//calculate Elevator position
	int position = bottomBoundary - (positionOffset * FLOOR_HEIGHT);

	//draw Elevator with some margins on the side
	int marginSize = 5;
	displayManager.DrawFilledRectangle(
		ELEVATOR_X + marginSize,
		position - FLOOR_HEIGHT + 5,
		ELEVATOR_X + ELEVATOR_W - marginSize,
		position,
		ECGV_CYAN
	);

	//draw passengers 
	int avaliableWidth = ELEVATOR_W - (marginSize * 2);
	int sectionWidth = avaliableWidth / 5;
	int drawnCount = 0;

	// Draw passengers in elevator with their destinations
	for (const auto& request : simulatorRef.GetRequests()) {
		if (request.IsFloorRequestDone() && !request.IsServiced()) {
			int passengerX = ELEVATOR_X + marginSize + (sectionWidth * drawnCount) + (sectionWidth / 2);
			int passengerY = position - FLOOR_HEIGHT / 2;
			DrawPassenger(passengerX, passengerY, request.GetFloorDest());
			drawnCount++;
		}
	}
}

//for processing key and mouse inputs
void ElevatorObserver::ProcessInputs() {
	ECGVEventType currentEvent = displayManager.GetCurrEvent();

	// Space bar for pause/resume
	if (currentEvent == ECGV_EV_KEY_DOWN_SPACE) {
		isPaused = !isPaused;
		std::cout << "Pause state: " << isPaused << std::endl;
	}
}

