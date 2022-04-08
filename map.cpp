#include "map.h"
#include "room.h"
#include "potion.h"
#include <QGraphicsScene>
#include <QRandomGenerator>
#include <vector>
#include <QDebug>

// operator += overloading to concatenate vectors (of int) for room generation
// safer to use because vectors are pointer and we can't return a new copy without memory corruption
template <typename T>
std::vector<T>& operator += (std::vector<T>& vector1, const std::vector<T>& vector2) {
    vector1.insert(vector1.end(), vector2.begin(), vector2.end());
    return vector1;
}


Map::Map(QGraphicsScene* scene) {
    qInfo() << "generating map";
    generateRandomMap(scene);
    //displayMap();
    // add a special icon for first room exit: wordle padlock ----------------------------------------------------------------------
    qInfo() << "end of map generation, showing hero";
    heroImg = new QGraphicsPixmapItem(QPixmap(":/ressources/items/hero.png").scaled(20,20));
    heroImg->setPos(mapGrid[currentx][currenty]->pos()+QPointF(-10,-10));
    scene->addItem(heroImg);
}

Map::~Map() {
    for (int i=0; i<5; i++) {
        for (int j = 0; j<5; j++) {
            if (mapGrid[i][j]!=nullptr) delete mapGrid[i][j];
        }
    }
    delete heroImg;
}


void Map::generateRandomMap(QGraphicsScene* scene)  { // fill the mapGrid

    int windowx = windowSize.width();
    // Room is a friend of map, and this property will be used extensively here

    // reseting the mapgrid
    for (int i = 0; i<5; i++) {for (int j=0; j<5;j++) {mapGrid[i][j] = nullptr;}}
    // position of current room (starting room)
    int x = QRandomGenerator::global()->bounded(5);
    int y = QRandomGenerator::global()->bounded(5);
    // creating the starting room
    mapGrid[x][y] = new Room(NoDoor,NoDoor,NoDoor,NoDoor, true);
    currentx = x;
    currenty = y;
    // adding first room to the scene
    mapGrid[x][y]->setPos(windowx*0.3+x*80,50+y*80);
    scene->addItem(mapGrid[x][y]);
    mapGrid[x][y]->canHaveAdditionalExits = false;

    // tree of rooms
    std::vector<int> mainRoomTreeX;
    std::vector<int> mainRoomTreeY;
    // first room not added to the tree because we will later use the tree for expansiona and do not want another exit in first room
    // Saves wether a new room has been sucessfully created
    bool newRoomGenerated;

    // Constructing direct line to boss room (5 rooms in between)
    for (int i = 0; i<6; i++) {
        if (i==5) {newRoomGenerated = tryGenerateNewRoom(x,y,BossDoor);}
        else {newRoomGenerated = tryGenerateNewRoom(x,y,DoorOpen);}

        if (newRoomGenerated) {
            // x and y are now the coordinates of the new room generated
            // adding the room to the tree
            mainRoomTreeX.push_back(x);
            mainRoomTreeY.push_back(y);
            // Populating room (with last one being boss room)
            if (i==5) { // boss room
                mapGrid[x][y]->canHaveAdditionalExits = false;
                mapGrid[x][y]->hasEnemy = true;
                mapGrid[x][y]->isBossRoom = true;
            } else { // normal room
                int randint = QRandomGenerator::global()->bounded(100);
                if (randint < 90) { // combat room
                    mapGrid[x][y]->hasEnemy = true;
                    // key
                    randint = QRandomGenerator::global()->bounded(100);
                    if (randint>=90) mapGrid[x][y]->hasKey = true;
                    // potion
                    randint = QRandomGenerator::global()->bounded(100);
                    if (randint>=50) mapGrid[x][y]->hasPotion = true;
                    // gold
                    randint = QRandomGenerator::global()->bounded(100);
                    if (randint>=50) {
                        randint = QRandomGenerator::global()->bounded(10);
                        mapGrid[x][y]->gold = randint + 5;
                    }
                } else { // chest room
                    mapGrid[x][y]->hasChest = true;
                }
            }


            // adding the room to the scene
            mapGrid[x][y]->setPos(windowx*0.3+x*80,50+y*80);
            scene->addItem(mapGrid[x][y]);

        } else { // no possibility of expanding, dead end, going back to previous part of the tree
            // try to connect dead end room to adjacent rooms not to make it a dead end anymore
            tryConnectToAdjacentRoom(x,y);

            // going one step back because we want a constant distance to boss room, not that rushing is a good strategy,
            // but we want to force exploration of the map. -1 for going back in the tree, -1 for constant distance: i = i-2
            // NB: this rule may be broken by additionnal rooms generated with random connections
            i=i-2;
            // take the coordinates of previous member of the tree for next iteration
            mainRoomTreeX.pop_back();
            mainRoomTreeY.pop_back();
            x = mainRoomTreeX.back();
            y = mainRoomTreeY.back();
        }
    }// end of main room tree generation -------------------------------------------

    // generating additionnal rooms, for exploration, leveling, rare loot
    // all new rooms added here will try to connect to rooms next to them, with some exceptions, to create an easy maze of sorts
    bool newTreeLength = true;
    // generating shop room near the boss room
    mainRoomTreeX.pop_back(); // taking room near the boss
    mainRoomTreeY.pop_back(); // making the tree to the shop the new main tree
    x = mainRoomTreeX.back(); // because we can't have additionnal door from the boss room
    y = mainRoomTreeY.back();
    while (newTreeLength) {
        newRoomGenerated = tryGenerateNewRoom(x,y,DoorOpen);
        if (newRoomGenerated) {
            tryConnectToAdjacentRoom(x,y);
            newTreeLength = false;
            // adding the shop and gold
            mapGrid[x][y]->isShop = true;
            // gold
            int randint = QRandomGenerator::global()->bounded(100);
            if (randint>=50) {
                randint = QRandomGenerator::global()->bounded(10);
                mapGrid[x][y]->gold = randint + 10;
            }
            // adding the room to the scene
            mapGrid[x][y]->setPos(windowx*0.3+x*80,50+y*80);
            scene->addItem(mapGrid[x][y]);
            // adding the room to the tree
            mainRoomTreeX.push_back(x);
            mainRoomTreeY.push_back(y);
        } else {
            mainRoomTreeX.pop_back(); // going further back in the tree
            mainRoomTreeY.pop_back();
            x = mainRoomTreeX.back();
            y = mainRoomTreeY.back();
        }
    }
    qInfo() << "position of shop" << x << y;
    // end of shop room generation ---------------------------------------------------------------------------------------

    // generating a few more random rooms, with locked rooms every 5 rooms, small chance of chest room
    int additionnalRoomsNb = QRandomGenerator::global()->bounded(6)+10; // leaves with at least 12 normal rooms to visit, 2-3(rare) locked rooms with rare chest

    qInfo() << "trying to generate" << additionnalRoomsNb << "new rooms";

    // (not accounting for the small chance of these rooms to be special) min 17 rooms (if no blocking) and max 25 rooms (maximum amount possible on the 5x5 map)
    // taking branchable rooms (removing starting room) (some might be full but will be removed in the loop)
    mainRoomTreeX.erase(mainRoomTreeX.begin());
    mainRoomTreeY.erase(mainRoomTreeY.begin());

    // choosing a random one and adding a room next to it
    int roomsGenerated = 0;
    int availableRoomsNb = mainRoomTreeX.size();
    while (roomsGenerated<additionnalRoomsNb && availableRoomsNb>0) {
        // selecting a random x and y from available ones
        int randomInt = QRandomGenerator::global()->bounded(availableRoomsNb);
        x = mainRoomTreeX[randomInt];
        y = mainRoomTreeY[randomInt];
        int oldX=x;
        int oldY=y;
        qInfo() << "generating room" << x << y << "rooms generated" << roomsGenerated;
        // trying to generate a room (locked room every 5 rooms)
        if ((roomsGenerated+1)%5==0) newRoomGenerated = tryGenerateNewRoom(x,y,DoorLocked);
        else newRoomGenerated = tryGenerateNewRoom(x,y,DoorOpen);

        while (newRoomGenerated) {
            roomsGenerated++;
            qInfo() << "rooms generated" << roomsGenerated << x << y;
            // populating the room first room is the one with the chest room
            if (roomsGenerated==1) { // room with the boss key
                mapGrid[x][y]->hasBossKeyChest = true;

            } else if (roomsGenerated%5==0) { // locked room
                qInfo() << "locked room";
                mapGrid[x][y]->canHaveAdditionalExits = false;
                mapGrid[x][y]->hasChest = true;

            } else { // normal room
                int randint = QRandomGenerator::global()->bounded(100);
                if (randint < 90) { // combat room
                    mapGrid[x][y]->hasEnemy = true;
                    // key
                    randint = QRandomGenerator::global()->bounded(100);
                    if (randint>=90) mapGrid[x][y]->hasKey = true;
                    // potion
                    randint = QRandomGenerator::global()->bounded(100);
                    if (randint>=50) mapGrid[x][y]->hasPotion = true;
                    // gold
                    randint = QRandomGenerator::global()->bounded(100);
                    if (randint>=50) {
                        randint = QRandomGenerator::global()->bounded(10);
                        mapGrid[x][y]->gold = randint + 5;
                    }
                    // room 7 has a map (if it exists and is not a chest room)
                    if (roomsGenerated==7) {qInfo() << "trying to add map"; mapGrid[x][y]->hasMap = true;}
                } else { // chest room
                    qInfo() << "trying to add chest";
                    mapGrid[x][y]->hasChest = true;
                }
            }

            // adding the room to the scene
            mapGrid[x][y]->setPos(windowx*0.3+x*80,50+y*80);
            scene->addItem(mapGrid[x][y]);
            // if not a locked room
            if (roomsGenerated%5!=0) {
                qInfo() << "trying to connect";
                // 50% chance of connecting to adjacent rooms
                int chanceOfConnection = QRandomGenerator::global()->bounded(2);
                if (chanceOfConnection) tryConnectToAdjacentRoom(x,y);
                // adding room to branchable rooms
                mainRoomTreeX.push_back(x);
                mainRoomTreeY.push_back(y);
            }
            // trying to generate another room from the same room
            x = oldX;
            y = oldY;
            if ((roomsGenerated+1)%5==0) { // (locked room every 5 rooms)
                newRoomGenerated = tryGenerateNewRoom(x,y,DoorLocked);
            } else newRoomGenerated = tryGenerateNewRoom(x,y,DoorOpen);
        }
        // no more branches from this room, remove it from the vector
        mainRoomTreeX.erase(std::find(mainRoomTreeX.begin(),mainRoomTreeX.end(),x));
        mainRoomTreeY.erase(std::find(mainRoomTreeY.begin(),mainRoomTreeY.end(),y));
        // updating number of available rooms
        availableRoomsNb = mainRoomTreeX.size();

        qInfo() << "available rooms nb" << availableRoomsNb;

    }
    // all rooms generated or generator is stuck because of rooms that can't get new exits or are surrounded by other rooms
    // end of additionnal rooms generation --------------------------------------------------------------
    qInfo() << "completed map generation";
}


bool Map::tryGenerateNewRoom(int &x, int &y, DoorState doorType) {
    // in case there is an error
    if (mapGrid[x][y]==nullptr) return false;
    // possible directions in the given room
    std::vector<Direction> possibleDirection;
    // testing rooms around the given room
    if ((y>0) && (mapGrid[x][y-1]==nullptr)) possibleDirection.push_back(N);
    if ((x<4) && (mapGrid[x+1][y]==nullptr)) possibleDirection.push_back(E);
    if ((y<4) && (mapGrid[x][y+1]==nullptr)) possibleDirection.push_back(S);
    if ((x>0) && (mapGrid[x-1][y]==nullptr)) possibleDirection.push_back(W);
    // choosing next room position if possible
    int possibleDirectionNb = possibleDirection.size();
    if (possibleDirectionNb!=0) {
        // Chosing a random direction between those available
        Direction chosenDirection;
        if (possibleDirectionNb==1) {
            chosenDirection = possibleDirection[0];
        } else {
            int direction = QRandomGenerator::global()->bounded(possibleDirectionNb);
            chosenDirection = possibleDirection[direction];
        }
        // position of new room
        if (chosenDirection==N) {
            // opening door of last room
            mapGrid[x][y]->north = doorType;
            // new room coordinates
            y = y-1;
            // creating new room with open door
            mapGrid[x][y] = new Room(NoDoor,NoDoor,DoorOpen,NoDoor);

        } else if (chosenDirection==E) {
            // opening door of last room
            mapGrid[x][y]->east = doorType;
            // new room coordinates
            x = x+1;
            // creating new room with open door
            mapGrid[x][y] = new Room(NoDoor,NoDoor,NoDoor,DoorOpen);

        } else if (chosenDirection==S) {
            // opening door of last room
            mapGrid[x][y]->south = doorType;
            // new room coordinates
            y = y+1;
            // creating new room with open door
            mapGrid[x][y] = new Room(DoorOpen,NoDoor,NoDoor,NoDoor);

        } else if (chosenDirection==W) {
            // opening door of last room
            mapGrid[x][y]->west = doorType;
            // new room coordinates
            x = x-1;
            // creating new room with open door
            mapGrid[x][y] = new Room(NoDoor,DoorOpen,NoDoor,NoDoor);
        } else return false; // error?
        return true; // new room generated
    } else {
        return false; // new room couldn't be genrated
    }
}


bool Map::tryConnectToAdjacentRoom(const int &x, const int &y) {
    // possible directions in the given room
    std::vector<Direction> possibleDirection;
    // testing rooms around the given room
    if ((y>0) && (mapGrid[x][y-1]!=nullptr) && (mapGrid[x][y-1]->canHaveAdditionalExits) && (mapGrid[x][y-1]->south == NoDoor)) possibleDirection.push_back(N);
    if ((x<4) && (mapGrid[x+1][y]!=nullptr) && (mapGrid[x+1][y]->canHaveAdditionalExits) && (mapGrid[x+1][y]->west == NoDoor)) possibleDirection.push_back(E);
    if ((y<4) && (mapGrid[x][y+1]!=nullptr) && (mapGrid[x][y+1]->canHaveAdditionalExits) && (mapGrid[x][y+1]->north == NoDoor)) possibleDirection.push_back(S);
    if ((x>0) && (mapGrid[x-1][y]!=nullptr) && (mapGrid[x-1][y]->canHaveAdditionalExits) && (mapGrid[x-1][y]->east == NoDoor)) possibleDirection.push_back(W);
    // choosing nearby room to connect if possible
    int possibleDirectionNb = possibleDirection.size();
    if (possibleDirectionNb!=0) {
        // Chosing a random direction between those available
        Direction chosenDirection;
        if (possibleDirectionNb==1) {
            chosenDirection = possibleDirection[0];
        } else {
            int direction = QRandomGenerator::global()->bounded(possibleDirectionNb);
            chosenDirection = possibleDirection[direction];
        }

        // opening door of both room with respect to direction
        if (chosenDirection==N) {
            mapGrid[x][y]->north = DoorOpen;
            mapGrid[x][y-1]->south = DoorOpen;
        } else if (chosenDirection==E) {
            mapGrid[x][y]->east = DoorOpen;
            mapGrid[x+1][y]->west = DoorOpen;
        } else if (chosenDirection==S) {
            mapGrid[x][y]->south = DoorOpen;
            mapGrid[x][y+1]->north = DoorOpen;
        } else if (chosenDirection==W) {
            mapGrid[x][y]->west = DoorOpen;
            mapGrid[x-1][y]->east = DoorOpen;
        } else return false; // error
        return true; // door generated
    } else {
        return false; // door couldn't be genrated
    }
}



std::vector<int> Map::getBranchableRooms(const vector<int> &treeX, const vector<int> &treeY) {
    const int treeSize = treeX.size();
    int x;
    int y;
    std::vector<int> returnVector;
    for (int i =0;  i<treeSize; i++) {
        x = treeX[i];
        y = treeY[i];
        if ((y>0 && mapGrid[x][y-1]==nullptr)
            || (x<4 && mapGrid[x+1][y]==nullptr)
            || (y<4 && mapGrid[x][y+1]==nullptr)
            || (x>0 && mapGrid[x-1][y]==nullptr))
        {
            returnVector.push_back(i);
        }
    }
    return returnVector;
}


void Map::displayMap() { // sets all rooms to visible
    Room* room;
    // going through the map grid
    for (int i = 0; i<5; i++) {
        for (int j=0; j<5;j++) {
            room = mapGrid[i][j];
            // checks left expression first, if false doesn't check right expression
            // meaning if room doesn't exists to begin with, it won't check its private member (which doesn't exist), avoiding an error
            if ((room!= nullptr) && !(room->visibility)) {
                room->visibility=true; // making paint function paint the room (otherwise would instantly return, see Room::paint)
                room->update(); // asking for a paint of the room
            }
        }
    }
}

// return 0 if successful, 1 if no door, 2 if door locked, 3 if door bosslocked
int Map::goToRoom(Direction direction) {
    Room* currentRoom = mapGrid[currentx][currenty];
    // retrieving door state
    int addx = 0;
    int addy = 0;
    DoorState door;
    if (direction==N) {
        addy = -1;
        door = currentRoom->north;
    } else if (direction==E) {
        addx = 1;
        door = currentRoom->east;
    } else if (direction==S) {
        addy = 1;
        door = currentRoom->south;
    } else {
        addx = -1;
        door = currentRoom->west;
    }
    // return correcponding value
    if (door==DoorOpen) {
        // change current room
        currentx += addx;
        currenty += addy;
        currentRoom = mapGrid[currentx][currenty];
        // show room if invisible
        if (!currentRoom->visibility) {
            currentRoom->visibility = true;
            currentRoom->update();
        }
        // changing position of hero
        heroImg->setPos(currentRoom->pos()+QPointF(-10,-10));
        return 0;
    } else if (door==NoDoor) return 1;
    else if (door==DoorLocked) return 2;
    else return 3;
}

Room* Map::getCurrentRoom() {return mapGrid[currentx][currenty];}

void Map::resize(){
    // move all rooms and hero
}
