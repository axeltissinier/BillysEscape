#ifndef STARTMENUSCENE_H
#define STARTMENUSCENE_H

/**@file startmenuscene.h
 * @brief Start menu scene class definition and all of its childrens (buttons, text...).
 * It is a child of AbstractScene which itself is a child of QGraphicsScene, for polymorphism reasons.
*/

#include "abstractscene.h"
#include "textbutton.h"
#include "gamewindow.h"
#include <QLabel>

// Global variable: Size of the window
extern QSize windowSize;

/**@class StartMenuScene
 * @brief Scene class that is displayed at start and when coming back to the start menu.
 * Also contains its children for better management.
*/
class StartMenuScene : public AbstractScene {
    Q_OBJECT // Enable signals and slots
private:
    // Buttons
    TextButton* playButton;
    TextButton* instructionButton;
    TextButton* infoButton;
    TextButton* quitButton;
    TextButton* goBackButton;
    // Title
    QGraphicsSimpleTextItem* title;
    // Instruction and version text
    QScrollArea* infoText;
    QLabel* infoLabel;

public:
    // constructor
    StartMenuScene(GameWindow* gamewindow);
    // Overriding AbractScene pure virtual function
    virtual void resizing() override;

public slots:
    // button trigered events
    void startGame();
    void displayInfo(bool instructions);
    void goBack();

signals:
    // signal send to childrens to tell them to resize.
    void resizeSignal();
};

#endif // STARTMENUSCENE_H
