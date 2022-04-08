#ifndef ABSTRACTSCENE_H
#define ABSTRACTSCENE_H

/**@file window.h
 * @brief Abstract custom scene class, it is meant to be used for its polymorphism properties in the view (game window) functions.
*/

#include <QGraphicsScene>

/**@class AbstractScene
 * @brief It is a baseline for the various scene created and allow for polymorphism.
 * It cannot be used on itself, as it is an abstract class.
*/
class AbstractScene : public QGraphicsScene {
protected:
    QPixmap BGimg;

public:
    // Called when the window is resized
    virtual void resizing() = 0; // pure virtual
};

#endif // ABSTRACTSCENE_H
