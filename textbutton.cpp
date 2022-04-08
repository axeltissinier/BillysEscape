#include "textbutton.h"
#include "gamewindow.h"
#include <QGraphicsSimpleTextItem>
#include <QBrush>
#include <QString>
#include <QFont>


// Constructor: Pass by reference for copied values for efficiency?
TextButton::TextButton (QString label, float x, float y, // xPos and yPos inputed are relative position on the screen, on a scale from 0 to 1
                        QFont defFont, QFont hovFont, QBrush defBrush, QBrush hovBrush, QBrush clickBrush, // text style for each type of event
                        QGraphicsItem* parent)
                        : QGraphicsSimpleTextItem(parent) {
    // saving private variables
    xfac = x;
    yfac = y;
    defaultFont = defFont;
    hoverFont = hovFont;
    defaultBrush = defBrush;
    hoverBrush = hovBrush;
    clickedBrush = clickBrush;

    // setting text with its style
    this->setFont(defaultFont);
    this->setBrush(defaultBrush);
    this->setText(label);

    // setting position with respect to current window size
    float xpos = windowSize.width()*xfac - this->boundingRect().width()/2;
    float ypos = windowSize.height()*yfac - this->boundingRect().height()/2;
    this->setPos(xpos, ypos);

    //allow response to hover events
    setAcceptHoverEvents(true);
}

//public member functions
void TextButton::mousePressEvent(QGraphicsSceneMouseEvent *) {
    // Changes text style to clicked style
    this->setBrush(clickedBrush);
    // set to trigger value if released
    clickedOn = true;
}

void TextButton::mouseReleaseEvent(QGraphicsSceneMouseEvent *) {
    if (clickedOn && isUnderMouse()) {
        // send signal
        emit clicked();
        // reset font for next appearance if applicable
        this->setFont(defaultFont);
        this->setBrush(defaultBrush);
    }
}

void TextButton::hoverEnterEvent(QGraphicsSceneHoverEvent *) {
    QPointF oldSize = QPointF(this->boundingRect().width(), this->boundingRect().height());
    // Changes text style to hover style
    this->setFont(hoverFont);
    this->setBrush(hoverBrush);
    // adter bolding, size is changed, and there is an offset, we correct that offset by looking at the difference between old and new position
    QPointF newSize = QPointF(this->boundingRect().width(), this->boundingRect().height());
    // correcting position
    this->setPos(this->pos()+(oldSize-newSize)/2);
}
void TextButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
    QPointF oldSize = QPointF(this->boundingRect().width(), this->boundingRect().height());
    // Changes text style back to default
    this->setFont(defaultFont);
    this->setBrush(defaultBrush);
    // adter bolding, size is changed, and there is an offset, we correct that offset by looking at the difference between old and new position
    QPointF newSize = QPointF(this->boundingRect().width(), this->boundingRect().height());
    // correcting position
    this->setPos(this->pos()+(oldSize-newSize)/2);
}

void TextButton::dragLeaveEvent(QGraphicsSceneDragDropEvent *) {
    clickedOn = false;
}

// resize function
void TextButton::resize() {
    // setting new position relative to view size
    float xpos = windowSize.width()*xfac - this->boundingRect().width()/2;
    float ypos = windowSize.height()*yfac - this->boundingRect().height()/2;
    this->setPos(xpos, ypos);
}
