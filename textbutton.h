#ifndef TEXTBUTTON_H
#define TEXTBUTTON_H

/**@file textbutton.h
 * @brief Custom button class with hover event
*/

#include <QObject>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QString>
#include <QBrush>
#include <QFont>

// Global variable: Size of the window
extern QSize windowSize;

/**@class TextButton
 * @brief Text which becomes more visible when hovered and produce a signal when clicked on
*/
class TextButton : public QObject, public QGraphicsSimpleTextItem {
    Q_OBJECT // Enable signals and slots
public:
    //constructor with text displayed, and position in default frame (scene)
    TextButton (QString label, float x, float y, QFont defFont, QFont hovFont, QBrush defBrush, QBrush hovBrush, QBrush clickBrush, QGraphicsItem* parent = nullptr);

    // Event methods
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;

signals:
    // Signal emited when clicked
    void clicked();

public slots:
    // Method called when resized
    void resize();

private:
    // Style of the text (default, hover and clicked)
    QFont defaultFont;
    QFont hoverFont;
    QBrush defaultBrush;
    QBrush hoverBrush;
    QBrush clickedBrush;
    // To push the button you need to have pressed and release the right click on the button
    // This variable stores whether it has been clicked on or not
    bool clickedOn = false; // default value is always false

    // Relative position of text in window
    float xfac;
    float yfac;

};


#endif // TEXTBUTTON_H
