#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

/**@file window.h
 * @brief Custom view class, that will be the main window of the game and will be changing scene.
*/

#include <QGraphicsView>
#include "abstractscene.h"

// Global variable: Size of the window
extern QSize windowSize;

/**@class GameWindow
 * @brief It acts as the main window of the program. It is used to manage resize event and change between scenes.
*/
class GameWindow : public QGraphicsView {
    Q_OBJECT // Enable signals and slots

private:
    // Our scene that the view (GameWindow) is looking at; what is displayed on the screen
    // It is an abstract class but will be used here for its polymorphism properties.
    AbstractScene* oldScene = nullptr;
    AbstractScene* currentScene;

public:
    //constructor
    GameWindow();
    // resize event override to catch those event and transfert them to the scene so that it resizes properly.
    virtual void resizeEvent(QResizeEvent*) override;
public slots:
    // Change scene to start menu
    void displayStartMenu();
    // Change scene to game scene
    void displayGame();
};

#endif // GAMEWINDOW_H
