#include "startmenuscene.h"
#include "textbutton.h"

#include <QString>
#include <QFont>
#include <QBrush>

#include <QScrollArea>
#include <QLabel>
#include <QScrollBar>

#include <QGraphicsSimpleTextItem>

// Copy the constructor of AbstractScene class, which is the constructor of QGraphicsScene by default, creating our scene.
// We then initialize it and place elements on it
StartMenuScene::StartMenuScene(GameWindow* gamewindow) : AbstractScene() {
    // Background
    BGimg.load(":/ressources/backgrounds/DungeonER.jpg");
    setSceneRect(0,0,windowSize.width(), windowSize.height());
    setBackgroundBrush(BGimg.scaled(windowSize));

    // Title
    title = new QGraphicsSimpleTextItem(QString("Billy's Escape"));
    QFont titleFont ("Calibri", 30, 50);
    QBrush titleBrush (Qt::white);
    title->setFont(titleFont);
    title->setBrush(titleBrush);
    addItem(title);

    // Setting position with respect to current window size
    float xpos = windowSize.width()*0.5 - title->boundingRect().width()/2;
    float ypos = windowSize.height()*0.2 - title->boundingRect().height()/2;
    title->setPos(xpos, ypos);

    // Setting up the buttons
    QFont deffont ("Calibri", 20);
    QFont hovfont ("Calibri", 20, 75);
    QBrush defbrush (Qt::white);
    QBrush hovbrush (Qt::white);
    QBrush clickbrush (Qt::gray);
    // Play button
    playButton = new TextButton(QString("Play"), 0.5, 0.4, deffont, hovfont, defbrush, hovbrush, clickbrush);
    QObject::connect(playButton, SIGNAL(clicked()), gamewindow, SLOT(displayGame()));
    QObject::connect(this, SIGNAL(resizeSignal()), playButton, SLOT(resize()));
    addItem(playButton);

    // Instruction button
    instructionButton = new TextButton(QString("Instructions"), 0.5, 0.55, deffont, hovfont, defbrush, hovbrush, clickbrush);
    QObject::connect(instructionButton, &TextButton::clicked, this, [this]{displayInfo(true);});
    QObject::connect(this, SIGNAL(resizeSignal()), instructionButton, SLOT(resize()));
    addItem(instructionButton);

    // Version button
    infoButton = new TextButton(QString("Credits"), 0.5, 0.7, deffont, hovfont, defbrush, hovbrush, clickbrush);
    QObject::connect(infoButton, &TextButton::clicked, this, [this]{displayInfo(false);});
    QObject::connect(this, SIGNAL(resizeSignal()), infoButton, SLOT(resize()));
    addItem(infoButton);

    // Quit button
    quitButton = new TextButton(QString("Close game"), 0.5, 0.85, deffont, hovfont, defbrush, hovbrush, clickbrush);
    QObject::connect(quitButton, SIGNAL(clicked()), gamewindow, SLOT(close()));
    QObject::connect(this, SIGNAL(resizeSignal()), quitButton, SLOT(resize()));
    addItem(quitButton);

    // Button to go back from instruction and version, hiding by default
    goBackButton = new TextButton(QString("Back"), 0.5, 0.85, deffont, hovfont, defbrush, hovbrush, clickbrush);
    QObject::connect(goBackButton, SIGNAL(clicked()), this, SLOT(goBack()));
    QObject::connect(this, SIGNAL(resizeSignal()), goBackButton, SLOT(resize()));
    // Adding the button later to make it on top of the text widget

    // Instruction and version text initialisation
    infoText = new QScrollArea();
    infoText->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    infoText->setWidgetResizable( true );
    int x = windowSize.width();
    int y = windowSize.height();
    infoText->setGeometry( x*0.2, y*0.3, x*0.6, y*0.6 );
    infoText->setStyleSheet("border:none; background-color:transparent;");
    infoLabel = new QLabel();
    infoLabel->setWordWrap(true);
    infoLabel->setAlignment(Qt::AlignJustify); // justify text
    infoLabel->setTextFormat(Qt::RichText); // allow the use of html to format text
    infoLabel->setText(QString(""));
    infoLabel->setStyleSheet("background-color:transparent;");

    // adding text
    infoText->setWidget(infoLabel);
    addWidget(infoText);
    infoText->setVisible(false); // hiding it

    // Adding button (in top of the text)
    addItem(goBackButton);
    goBackButton->setVisible(false);
}

// may be removed in the future
void StartMenuScene::startGame() {
    // construct the game scene and launch the game
    // gamewindow->displayGame();
}

void StartMenuScene::displayInfo(bool instructions) {
    playButton->setVisible(false);
    instructionButton->setVisible(false);
    infoButton->setVisible(false);
    quitButton->setVisible(false);

    // showing the go back button and text
    infoText->setVisible(true);
    goBackButton->setVisible(true);
    // Instruction or version text
    QString text;
    if (instructions) {
        text = QString("<h1 style=\"color:white; font-size:20px;\">You wake up in a cell, with no idea how you ended up here. You only remember two things: your name, Billy, and how to use a sword.<br><br>Now, you must find a way to leave this cell, and leave this dungeon if you don't want to die there.<br><br>Fight enemies, equip more powerful equipment, and beat the boss of the level to win the game.</h1>");
    } else {
        text = QString("<h1 style=\"color:white; font-size:20px;\">Billy's Escape<br><br>Version: 1.0<br>Date: 8 April 2022<br><br>nCredits:<br><br>Programming: Axel Tissinier<br><br>Images: Various Videos games, and free stock images</h1>");
    }
    infoLabel->setText(text);
}

void StartMenuScene::resizing() {
    // Resize the background
    setBackgroundBrush(BGimg.scaled(windowSize));
    setSceneRect(0,0,windowSize.width(), windowSize.height());
    // resize text
    int x = windowSize.width();
    int y = windowSize.height();
    infoText->setGeometry( x*0.2, y*0.3, x*0.6, y*0.6 );
    // resize title
    float xpos = windowSize.width()*0.5 - title->boundingRect().width()/2;
    float ypos = windowSize.height()*0.2 - title->boundingRect().height()/2;
    title->setPos(xpos, ypos);
    // Send a signal so that children custom class resize
    emit resizeSignal();
};

void StartMenuScene::goBack() {
    // Hiding go back button
    goBackButton->setVisible(false);
    infoText->setVisible(false);
    // showing default button
    playButton->setVisible(true);
    instructionButton->setVisible(true);
    infoButton->setVisible(true);
    quitButton->setVisible(true);
}
