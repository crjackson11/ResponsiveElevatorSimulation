#include "ECGraphicViewImp.h"
#include "ElevatorObserver.h"
#include "ECElevatorSim.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>



int main(int argcount, char* argv[])
{
    //Error handling (gives user proper way to input file)
    if (argcount != 2) {
        std::cout << "Arguments required: " << argv[0] << " <input-file>" << std::endl;
        return 1;
    }

    std::vector<ECElevatorSimRequest> requestList;
    int numOfLevels, lengthOfTime; //number of floors and the duration of the elevator's run

    std::ifstream fileStream(argv[1]);
    if (!fileStream.is_open()) {
        std::cout << "Couldn't open file: " << argv[1] << std::endl;
        return 1;
    }

    //read first line for floors + time to simulate
    std::string firstLine;
    while (std::getline(fileStream, firstLine)) {
        if (firstLine[0] == '#') continue;   //skip comments
        std::stringstream iss(firstLine);
        iss >> numOfLevels >> lengthOfTime;
        break;
    }

    //read passenger requests
    while (std::getline(fileStream, firstLine))
    {
        if (firstLine[0] == '#') continue;   //skip comments

        int passengerAppearanceTime, originalFloor, destinationFloor;
        std::stringstream iss(firstLine);
        if (iss >> passengerAppearanceTime >> originalFloor >> destinationFloor) {
            requestList.push_back(ECElevatorSimRequest(passengerAppearanceTime, originalFloor, destinationFloor));
        }
    }

    ECElevatorSim simulator(numOfLevels, requestList);

    //creates window to display UI
    const int widthWin = 500, heightWin = 800;
    ECGraphicViewImp view(widthWin, heightWin);

    ElevatorObserver obs(view, simulator, lengthOfTime);
    view.Attach(&obs);

    view.Show();

    return 0;
}