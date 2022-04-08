#ifndef GAMESCENE_H
#define GAMESCENE_H

/**@file gamescene.h
 * @brief Game scene class with all its children.
 * It is a child of AbstractScene which itself is a child of QGraphicsScene, for polymorphism reasons.
*/

#include "abstractscene.h"
#include "gamewindow.h"
#include "gameconsole.h"
#include "map.h"
#include "hero.h"

// Global variable: Size of the window
extern QSize windowSize;

/**@class GameScene
 * @brief Scene class that is displayed when launching game.
 * Also contains its children for better management.
*/
class GameScene : public AbstractScene {
    Q_OBJECT // Enable signals and slots -- necessary?
private:
    // console log and entry
    GameConsole* gameconsole;
    // Bar with actions/buttons
    //ActionBar* actionbar;
    // map of the level
    Map levelMap;
    // hero data and functions
    Hero hero;

public:
    // constructor
    GameScene(GameWindow* gamewindow);
    ~GameScene();
    // Overriding AbstractScene pure virtual function
    virtual void resizing() override;
};

#endif // GAMESCENE_H
