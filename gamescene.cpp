#include "gamescene.h"
#include "gameconsole.h"
#include "map.h"

GameScene::GameScene(GameWindow* gamewindow) : levelMap(this), hero(this) {
    qInfo() << "after generation?";
    // Background initialisation
    BGimg.load(":/ressources/backgrounds/testBG.png");
    setSceneRect(0,0,windowSize.width(), windowSize.height());
    setBackgroundBrush(BGimg.scaled(windowSize));
    // Console initialisation
    qInfo() << "generating console";
    gameconsole = new GameConsole(this, &levelMap, &hero);

    // other initialisation
    QObject::connect(gameconsole, &GameConsole::refreshScene, this, [this]{this->update(100,10,windowSize.width()-100, windowSize.height()-100);});
    QObject::connect(gameconsole, SIGNAL(closeGame()), gamewindow, SLOT(close()));
    QObject::connect(gameconsole, SIGNAL(goBackToStartMenu()), gamewindow, SLOT(displayStartMenu()));
    qInfo() << "end of game scene construction";

    // Quit button
    QFont deffont ("Calibri", 14);
    QFont hovfont ("Calibri", 14, 75);
    QBrush defbrush (Qt::white);
    QBrush hovbrush (Qt::white);
    QBrush clickbrush (Qt::gray);
    TextButton* quitButton = new TextButton(QString("Close game"), 0.2, 0.9, deffont, hovfont, defbrush, hovbrush, clickbrush);
    QObject::connect(quitButton, SIGNAL(clicked()), gamewindow, SLOT(close()));
    quitButton->setPos(20, windowSize.height()*0.95);
    this->addItem(quitButton);
}

GameScene::~GameScene() {
    delete gameconsole;
}

// Overriding AbractScene pure virtual function
void GameScene::resizing() {
    // Resize the background
    setBackgroundBrush(BGimg.scaled(windowSize));
    setSceneRect(0,0,windowSize.width(), windowSize.height());

    // Tell children to resize
    // gameconsole->resize();
    // actionbar->resize();
    // map->resize();
}

