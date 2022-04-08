#include "graphicsobjectbutton.h"
#include <QPainter>
#include <QPen>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>

#include <QLabel>

GraphicsObjectButton::GraphicsObjectButton(int wid, float hei, int roundingPercentage, QGraphicsItem* parent)
    : QGraphicsObject(parent), width(wid), height(hei), percent(roundingPercentage)
{
    //allow response to hover and drops events
    setAcceptHoverEvents(true);
    setAcceptDrops(true);

    // text example
    /*
    QGraphicsTextItem* texttest;
    texttest = new QGraphicsTextItem(QString("test text"), this);

    //set position of text within button in middle of thingy
    int xPos = wid/2 - texttest->boundingRect().width()/2;
    int yPos = hei/2 - texttest->boundingRect().height()/2;
    texttest->setPos(xPos, yPos); //places text in the middle of the button


    // image example
    QGraphicsPixmapItem* testimage;
    testimage = new QGraphicsPixmapItem(this);
    testimage->setPos(0,0);
    testimage->setPixmap(QPixmap(":/ressources/items/imp.png").scaled(35,40));
    */

}

//public member functions
void GraphicsObjectButton::mousePressEvent(QGraphicsSceneMouseEvent *) {
    // Changes text style to clicked style
    clickState = 2;
    // repainting rect
    this->update();
}

void GraphicsObjectButton::mouseReleaseEvent(QGraphicsSceneMouseEvent *) {
    if (clickState == 2 && isUnderMouse()) {
        emit clicked(); // send clicked signal
        clickState = 1;
    } else {clickState = 0;}
    // reset it (for after drag leave mouse release or next appearance if applicable)
    this->update();
}

void GraphicsObjectButton::hoverEnterEvent(QGraphicsSceneHoverEvent *) {
    clickState=1; //
    // trigger a repaint to show
    this->update();
}

void GraphicsObjectButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *){
    // reset to default state
    clickState = 0;
    this->update();
}

void GraphicsObjectButton::dragLeaveEvent(QGraphicsSceneDragDropEvent *) {
    clickState = 0; // resetting value so button will not be trigered
}


// pure virtual from QGraphicsItem to replace
QRectF GraphicsObjectButton::boundingRect() const {
    // return the rectangle
    return QRectF(0, 0, width, height);
}

// paint function, called whenver the item is updated or moved
void GraphicsObjectButton::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    // sets the background
    painter->setBrush(Qt::NoBrush);
    // sets the border
    QPen pen(Qt::white);
    pen.setWidth(1); // default white 1
    if (clickState) pen.setWidth(3); // when hovering and clicked on
    if (clickState==2) pen.setColor(Qt::gray); // when clicked on
    painter->setPen(pen);
    // draw rectangle
    painter->drawRoundedRect(0,0, width, height, percent, percent);
}

void GraphicsObjectButton::setSize(int hei, int wid) {
    height = hei;
    width = wid;
}
