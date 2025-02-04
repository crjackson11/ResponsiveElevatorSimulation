//
//  ECElevatorSim.cpp
//  
//
//  Created by Yufeng Wu on 6/27/23.
//  Elevator simulation


#include "ECElevatorSim.h"

using namespace std;

// UpwardStrategy Implementation
// Stop at the floor if there are passengers to unload or load
bool UpStrategy::stopAtFloor(int floor, const std::vector<ECElevatorSimRequest>& requests, int currTime) const {
    for (const auto& req : requests) {
        if (req.IsServiced()) continue;
        if (req.GetTime() > currTime) continue;

        // Unload passengers
        if (req.IsFloorRequestDone() && !req.IsServiced() && req.GetFloorDest() == floor) {
            return true;
        }

        // Load passengers
        if (!req.IsFloorRequestDone() && req.GetFloorSrc() == floor) {
            return true;
        }
    }
    return false;
}

// Check if there are more requests to service
bool UpStrategy::containsMoreRequests(int currFloor, const std::vector<ECElevatorSimRequest>& requests, int currTime) const {
    for (const auto& req : requests) {
        if (req.IsServiced()) continue;
        if (req.GetTime() > currTime) continue;

        // Check if there are requests to service above the current floor
        int targetFloor = req.GetRequestedFloor();
        if (targetFloor > currFloor) {
            return true;
        }
    }
    return false;
}

// Get the direction of the elevator (up)
EC_ELEVATOR_DIR UpStrategy::getDirection() const {
    return EC_ELEVATOR_UP;
}

// DownwardStrategy Implementation
bool DownStrategy::stopAtFloor(int floor, const std::vector<ECElevatorSimRequest>& requests, int currTime) const {
    for (const auto& req : requests) {
        if (req.IsServiced()) continue;
        if (req.GetTime() > currTime) continue;

        // Unload passengers
        if (req.IsFloorRequestDone() && !req.IsServiced() && req.GetFloorDest() == floor) {
            return true;
        }

        // Load passengers
        if (!req.IsFloorRequestDone() && req.GetFloorSrc() == floor) {
            return true;
        }
    }
    return false;
}

// Check if a floor contains more requests than the current floor
bool DownStrategy::containsMoreRequests(int currFloor, const std::vector<ECElevatorSimRequest>& requests, int currTime) const {
    for (const auto& req : requests) {
        if (req.IsServiced()) continue;
        if (req.GetTime() > currTime) continue;

        // Check if there are requests to service below the current floor
        int targetFloor = req.GetRequestedFloor();
        if (targetFloor < currFloor) {
            return true;
        }
    }
    return false;
}

// Get the direction of the elevator (down)
EC_ELEVATOR_DIR DownStrategy::getDirection() const {
    return EC_ELEVATOR_DOWN;
}

// ECElevatorSim Implementation
// Constructor
ECElevatorSim::ECElevatorSim(int numFloorsIn, std::vector<ECElevatorSimRequest>& listRequests)
    : numFloors(numFloorsIn), currFloor(1), currDir(EC_ELEVATOR_STOPPED), requests(listRequests) {
    upStrategy = new UpStrategy();
    downStrategy = new DownStrategy();
}

// Destructor
ECElevatorSim::~ECElevatorSim() {
    delete upStrategy;
    delete downStrategy;
}

// Simulate the elevator
void ECElevatorSim::Simulate(int lenSim) {
    if (handlePassengers(lenSim)) {
        return;
    }

    updateElevator(lenSim);
}

// HELPER METHODS FOR ECElevatorSim:Simulate
// handlePassengers helper method, handles the passengers
bool ECElevatorSim::handlePassengers(int time) {
    // Check if there's a passenger already loaded and a new request at the same floor
    for (const auto& req : requests) {
        if (!req.IsFloorRequestDone() && req.GetFloorSrc() == currFloor && req.GetTime() == time) {
            // Check if we already picked up someone at this floor
            for (const auto& otherReq : requests) {
                if (otherReq.IsFloorRequestDone() && !otherReq.IsServiced() &&
                    otherReq.GetFloorSrc() == currFloor && otherReq.GetTime() < time) {
                    return false;  // Don't allow new pickup, elevator should be moving
                }
            }
        }
    }

    // Normal passenger handling
    int passengersAtFloor = countPassengersAtFloor(time);
    if (passengersAtFloor == 0) return false;

    handleAllPassengersAtFloor(time);
    determineDirection(time);
    return true;
}

// countPassengersAtFloor helper method, counts the number of passengers at the current floor
int ECElevatorSim::countPassengersAtFloor(int time) {
    int count = 0;
    for (const auto& req : requests) {
        if (req.GetTime() > time) continue;

        // Check if there are passengers to unload or load at the current floor
        if ((req.IsFloorRequestDone() && !req.IsServiced() && currFloor == req.GetFloorDest()) ||
            (!req.IsFloorRequestDone() && currFloor == req.GetFloorSrc())) {
            count++; // Increment the count
        }
    }
    return count;
}

// handleAllPassengersAtFloor helper method, handles all passengers at the current floor
void ECElevatorSim::handleAllPassengersAtFloor(int time) {
    //Go through the requests to see who needs to be serviced
    for (auto& req : requests) {
        if (req.GetTime() > time) continue;

        //Servicing the passenger
        if (req.IsFloorRequestDone() && !req.IsServiced() && currFloor == req.GetFloorDest()) {
            req.SetArriveTime(time);
            req.SetServiced(true);
        }

        //Finish the floor request
        if (!req.IsFloorRequestDone() && currFloor == req.GetFloorSrc()) {
            req.SetFloorRequestDone(true);
        }
    }
}

//determineDirection helper method, determines the direction of the elevator
void ECElevatorSim::determineDirection(int currTime) {
    if (shouldContinueCurrentDirection(currTime)) {
        return;
    }

    setNewDirection(currTime);
}

// HELPER METHODS FOR determineDirection:
//shouldContinueCurrentDirection helper method, checks if the elevator should continue in the current direction
bool ECElevatorSim::shouldContinueCurrentDirection(int currTime) {
    if (currDir == EC_ELEVATOR_STOPPED) {
        return false;
    }

    ElevatorStrategy* strategy = (currDir == EC_ELEVATOR_UP) ? upStrategy : downStrategy;
    return strategy->containsMoreRequests(currFloor, requests, currTime);
}

// Helper method for determineDirection:
// setNewDirection helper method, sets the new direction of the elevator
void ECElevatorSim::setNewDirection(int currTime) {
    int earliestTime = INT_MAX;
    int targetFloor = -1;

    // Find the earliest request that still needs service
    for (const auto& req : requests) {
        // Check if the request is not serviced and the time has passed
        if (!req.IsServiced() && req.GetTime() <= currTime) {
            // Check if the request is the earliest
            if (req.GetTime() < earliestTime) {
                earliestTime = req.GetTime();
                targetFloor = !req.IsFloorRequestDone() ? req.GetFloorSrc() : req.GetFloorDest();
            }
        }
    }

    // Set the direction based on the target floor (if there is one)
    if (targetFloor != -1) {
        if (targetFloor > currFloor) {
            currDir = EC_ELEVATOR_UP;
        }
        else if (targetFloor < currFloor) {
            currDir = EC_ELEVATOR_DOWN;
        }
        else {
            currDir = EC_ELEVATOR_STOPPED;
        }
    }
    else {
        currDir = EC_ELEVATOR_STOPPED;
    }
}

// hasRequests helper method, checks if there are requests
bool ECElevatorSim::hasRequests(int currTime) const {
    for (const auto& req : requests) {
        if (!req.IsServiced() && req.GetTime() <= currTime) {
            return true;
        }
    }
    return false;
}

// Update the elevator
void ECElevatorSim::updateElevator(int time) {
    handleStop(time);
    moveElevator();
    determineDirection(time);
}

// HELPER METHODS FOR updateElevator:
// moveElavator helper method, moves the elevator
void ECElevatorSim::moveElevator() {
    if (currDir == EC_ELEVATOR_UP && currFloor < numFloors) {
        currFloor++;
    }
    else if (currDir == EC_ELEVATOR_DOWN && currFloor > 1) {
        currFloor--;
    }
}

// Helper method for updateElevator:
// handleStop helper method, handles the elevator stop
void ECElevatorSim::handleStop(int currTime) {
    bool handledPassenger = false;
    handleUnloading(currTime, handledPassenger);
    handleLoading(currTime, handledPassenger);
    determineDirection(currTime);
}

//Helper methods for handleStop:
//hasExistingPickup helper method, checks if there is an existing pickup
bool ECElevatorSim::hasExistingPickup(int floor, int currTime) {
    for (const auto& req : requests) {
        if (req.IsFloorRequestDone() && !req.IsServiced() &&
            req.GetFloorSrc() == floor && req.GetTime() < currTime) {
            return true;
        }
    }
    return false;
}

//Helper methods for handleStop:
//handleUnloading helper method, unloads the passenger
void ECElevatorSim::handleUnloading(int currTime, bool& handledPassenger) {
    for (auto& req : requests) {
        if (req.IsFloorRequestDone() && !req.IsServiced() && req.GetFloorDest() == currFloor) {
            req.SetServiced(true);
            req.SetArriveTime(currTime);
            handledPassenger = true;
        }
    }
}

//Helper methods for handleStop:
//handleLoading helper method, loads the passenger
void ECElevatorSim::handleLoading(int currTime, bool handledPassenger) {
    if (handledPassenger) return;

    for (auto& req : requests) {
        if (!req.IsFloorRequestDone() && !req.IsServiced() &&
            req.GetFloorSrc() == currFloor && req.GetTime() <= currTime) {
            if (!hasExistingPickup(currFloor, currTime)) {
                req.SetFloorRequestDone(true);
                break;
            }
        }
    }
}