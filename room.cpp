#include "room.h"
#include <QPainter>

Room::Room(DoorState northDoor, DoorState eastDoor, DoorState southDoor, DoorState westDoor, bool visible)
    : north(northDoor), east(eastDoor), south(southDoor), west (westDoor), visibility(visible)
    {}

Room::~Room() {
    int itemsNb = itemList.size();
    for (int i=0; i<itemsNb; i++) {
        delete itemList[i];
    }
    itemList.clear();
}

// pure virtual from QGraphicsItem (parent of QGraphicsObject) to replace
QRectF Room::boundingRect() const {
    return QRectF(-30,-30,30,30);
}

void Room::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    if (!visibility) return; // not drawn if invisible
    // sets how it will be painted
    QPen pen(Qt::white);
    pen.setWidth(3);
    painter->setPen(pen);
    // drawing the room
    // North line
    if (north==NoDoor) {
        painter->drawLine(-30,-30,30,-30);
    } else {
        painter->drawLine(-30,-30,-10,-30);
        painter->drawLine(10,-30,30,-30);
        if (north==DoorLocked) {
            painter->drawPixmap(-10,-30, QPixmap(":/ressources/items/key.png").scaled(15,10));
        } else if (north==BossDoor) {
            painter->drawPixmap(-10,-30, QPixmap(":/ressources/items/bossKeyZelda.png").scaled(20,20));
        }
    }
    // East line
    if (east==NoDoor) {
        painter->drawLine(30,-30,30,30);
    } else {
        painter->drawLine(30,-30,30,-10);
        painter->drawLine(30,10,30,30);
        if (east==DoorLocked) {
            painter->drawPixmap(30,-10, QPixmap(":/ressources/items/key.png").scaled(15,10));
        } else if (east==BossDoor) {
            painter->drawPixmap(30,-10, QPixmap(":/ressources/items/bossKeyZelda.png").scaled(20,20));
        }
    }
    // South Line
    if (south==NoDoor) {
        painter->drawLine(30,30,-30,30);
    } else {
        painter->drawLine(30,30,10,30);
        painter->drawLine(-10,30,-30,30);
        if (south==DoorLocked) {
            painter->drawPixmap(-10,30, QPixmap(":/ressources/items/key.png").scaled(15,10));
        } else if (south==BossDoor) {
            painter->drawPixmap(-10,30, QPixmap(":/ressources/items/bossKeyZelda.png").scaled(20,20));
        }
    }
    // West line
    if (west==NoDoor) {
        painter->drawLine(-30,30,-30,-30);
    } else {
        painter->drawLine(-30,30,-30,10);
        painter->drawLine(-30,-10,-30,-30);
        if (west==DoorLocked) {
            painter->drawPixmap(-30,-10, QPixmap(":/ressources/items/key.png").scaled(15,10));
        } else if (west==BossDoor) {
            painter->drawPixmap(-30,-10, QPixmap(":/ressources/items/bossKeyZelda.png").scaled(20,20));
        }
    }

    if (this->isBossRoom) painter->drawPixmap(-10,-10, QPixmap(":/ressources/items/Boss.png").scaled(20,20));
    else if (this->hasEnemy) painter->drawPixmap(-25,-25, QPixmap(":/ressources/items/minecraftZombie.png").scaled(10,10));
    else if (this->isShop) painter->drawPixmap(25,-15, QPixmap(":/ressources/items/coin.png").scaled(10,10));
    else if (hasChest) painter->drawPixmap(-25,15, QPixmap(":/ressources/items/chest.png").scaled(10,8));

}
