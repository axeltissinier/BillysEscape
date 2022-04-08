#ifndef ROOM_H
#define ROOM_H

#include <QGraphicsItem>
#include "displayableitem.h"

// Global variable: Size of the window
extern QSize windowSize;

enum DoorState {NoDoor, DoorOpen, DoorLocked, BossDoor};

class Room : public QGraphicsItem {
    // give the map access to data for easier interactions, instead of getters and setters
    friend class Map;
    friend class GameConsole; // the game console will be the class that command every interactions, so to make it easier, it is a friend
private:
    // state of each exits
    DoorState north;
    DoorState east;
    DoorState south;
    DoorState west;
    // state of the room for graphic display
    bool visibility;
    // if the room is a special room and can't be further connected to other room
    bool canHaveAdditionalExits = true;
    // various things inside the room
    bool hasEnemy = false;
    bool hasKey = false;
    bool hasPotion = false;
    bool hasChest = false;
    bool hasBossKeyChest = false;
    bool hasMap = false;
    bool isBossRoom = false;
    int gold = 0;
    std::vector<DisplayableItem*> itemList;
    // for shop room
    bool isShop = false;


public:
    // constructor & destructor
    Room(DoorState northDoor, DoorState eastDoor, DoorState southDoor, DoorState westDoor, bool visible = false);
    ~Room();
    // pure virtual from QGraphicsItem (parent of QGraphicsObject) to replace
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};


#endif // ROOM_H
