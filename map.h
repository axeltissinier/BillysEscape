#ifndef MAP_H
#define MAP_H

#include "room.h"
#include "shop.h"
#include <vector>
using namespace std;

// Global variable: Size of the window
extern QSize windowSize;

enum Direction {N, E, S, W};

class Map{

private:
    Room* mapGrid[5][5] = {{nullptr}}; // entire map (5 by 5 size)
    // bit structures
    int currentx:4;
    int currenty:4;
    QGraphicsPixmapItem* heroImg;
    // The following functions are used in generateRandomMap to create a the map, they are not public to make sure only the map class can create rooms
    // reference are used to get the new room location & adding it to the tree without needing to return them
    bool tryGenerateNewRoom(int &x, int &y, DoorState doorType);
    bool tryConnectToAdjacentRoom(const int &x, const int &y);
    vector<int> getBranchableRooms(const vector<int> &treeX, const vector<int> &treeY);

public:
    Map(QGraphicsScene* scene);
    ~Map();
    int goToRoom(Direction direction); // return 0 if it managed to do so, 1 if no door, 2 if door is locked, 3 if door is locked boss door
    void generateRandomMap(QGraphicsScene* scene); // fill the mapGrid with a new map (called in constructor)
    void displayMap(); // sets all rooms to visible
    void deleteMap();
    // returns current room to be modified
    Room* getCurrentRoom();
    // called when window is resized
    void resize();
};


#endif // MAP_H
