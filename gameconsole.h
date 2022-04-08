#ifndef GAMECONSOLE_H
#define GAMECONSOLE_H

#include <QScrollArea>
#include <QScrollBar>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include "consoletextedit.h"
#include "wordlelock.h"
#include "map.h"
#include "hero.h"
#include "shop.h"
#include "enemy.h"

#include <QObject>

// Global variable: Size of the window
extern QSize windowSize;

class GameConsole : public QObject {
    Q_OBJECT // enable signals and slots

private:
    // Console log
    QScrollArea* logArea;
    QScrollBar* customScrollBar; // scroll bar of the log area
    QLabel* logText;
    // Entry - rectangle with custom text edit
    ConsoleTextEdit* consoleTextEntry;
    // wordle thingy
    WordleLock* wordlelock = nullptr;
    // local pointer to the map and character sheet to use their functions more easily
    Map* map;
    Hero* hero;
    Room* currentRoom;
    QGraphicsScene* gameScene;
    // shop is unique and it is easier to control it from here
    Shop shop = Shop();
    // configuration (will tell which command can be used at that point)
    int configuration = 0;
    // pointers that will be used in some configurations
    DisplayableItem* currentItem = nullptr; // for reward configuration (3)
    Enemy* currentEnemy = nullptr; // for combat configuration (2)
    // action bar
    QGraphicsRectItem* actionBarBg;
    QGraphicsSimpleTextItem* actionBarText;
    std::vector<GraphicsObjectButton* > previousButtons;
    std::vector<GraphicsObjectButton* > buttons;
    QString additionalText = "";

public:
    // Constructor
    GameConsole(QGraphicsScene* scene, Map* globalMap, Hero* globalHero);
    ~GameConsole();
    void changeActionBarConfig(int config, int subConfig = 0);

public slots:
    void resize();
    void processText(QString actionBarInput = "");
    void wordleChangeText();
    void heroHasLeveledUp();

signals:
    void refreshScene(); // when there is updates, sometimes a redraw of the scene is needed
    void goBackToStartMenu();
    void closeGame();
};

#endif // GAMECONSOLE_H
