/**@file main.cpp
 * @author Axel Tissinier 21373248
 * @brief Creates the window and the view, launches the Q app. v0.1 08/02/2022
*/

#include <QApplication>
#include "gamewindow.h"
#include <QFontDatabase>
#include <QString>

// Size of the window, will be used in almost every resize event, so we make it global to access it easily.
// It will only change in the game window resize function.
QSize windowSize;
//QString bitFont = QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(":/ressources/ponderosa.ttf")).at(0);


// defining static members
#include "displayableitem.h"
QGraphicsPixmapItem * DisplayableItem::itemDisplay = NULL;
QGraphicsPixmapItem* DisplayableItem::imgDisplay = NULL;
QLabel* DisplayableItem::descriptionDisplay = NULL;
QGraphicsProxyWidget* DisplayableItem::widgetProxy = NULL;
QScrollArea* DisplayableItem::textArea = NULL;
QScrollBar* DisplayableItem::customScrollBar = NULL;
TextButton* DisplayableItem::closeButton = NULL;

int main(int argc, char *argv[])
{
    // loading the main font most of our text will use
    //int id = QFontDatabase::addApplicationFont(":/ressources/ponderosa.ttf");
    //bitFont = QFontDatabase::applicationFontFamilies(id).at(0);

    QApplication app(argc, argv);
    // Base size of window
    windowSize = QSize(850, 600);

    // Creating window
    GameWindow* gamewindow = new GameWindow();
    gamewindow->setWindowTitle(QString("Billy's Escape"));

    // creating first scene and showing start menu
    gamewindow->show();

    return app.exec();
}
