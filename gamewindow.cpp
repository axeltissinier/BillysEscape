/**@file gamewindow.cpp
 @brief GameWindow class functions, mainly changing the scenes and some event handling
*/

#include "gamewindow.h"
#include "startmenuscene.h"
#include "gamescene.h"
#include <QDebug>

/**@brief constructs the main screen and view */
GameWindow::GameWindow() {
    // Disabling scrollbars
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // Setting minimum size and base size
    setMinimumSize(850, 600);
    resize(windowSize);
    // disabling window border (for aesthetics)
    this->setFrameStyle(0);
    // Creating first scene and displaying it
    currentScene = new StartMenuScene(this);
    this->setScene(currentScene);
}


void GameWindow::displayStartMenu() {
    // swapping game scene and start menu scene
    AbstractScene* oldGameScene = currentScene;
    currentScene = oldScene;
    this->setScene(currentScene);
    oldScene = oldGameScene;
    // Delete game scene later because if we deleted it at the same time, would make it crash (deleted while still in loop)
    // oldGameScene->deleteLater(); // doesn't work
}


void GameWindow::displayGame() {
    /*
    // deleting gamescene if coming from there
    qInfo() << "deleting gamescene";
    if (oldScene!=nullptr) {GameScene* sceneToDelete = dynamic_cast<GameScene*>(oldScene); delete sceneToDelete;}
    */
    // keeping start menu scene
    oldScene = currentScene;
    // creating and displaying the new scene (game)
    qInfo() << "creating game scene";
    currentScene = new GameScene(this);
    qInfo() << "setting scene to window";
    this->setScene(currentScene);
    qInfo() << "completed";
}

void GameWindow::resizeEvent(QResizeEvent*)
{
    windowSize = this->size();
    currentScene->resizing();
}
