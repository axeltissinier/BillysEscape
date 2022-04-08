#ifndef DISPLAYABLEITEM_H
#define DISPLAYABLEITEM_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsSimpleTextItem>
#include <QString>
#include <QGraphicsProxyWidget>
#include <QScrollArea>
#include <QScrollBar>
#include <QLabel>
#include "textbutton.h"

#include <QDebug>
#include <QTimer>

// Global variable: Size of the window
extern QSize windowSize;

class DisplayableItem : public QObject {

    Q_OBJECT //enable signals and slots

public:
    friend class Hero;

protected:
    // img and text displayed
    int childType = -1; // type of the children created
    QPixmap img;
    QString name;
    QString description;

    // rectangle that will appear when asked for description, only one existing at all times so it is made static, same for the img and description
    static QGraphicsPixmapItem* itemDisplay;
    static QGraphicsPixmapItem* imgDisplay;
    static QGraphicsProxyWidget* widgetProxy;
    static QScrollArea* textArea;
    static QScrollBar* customScrollBar; // scroll bar of the log area
    static QLabel* descriptionDisplay;
    static TextButton* closeButton;

public:
    // used for special items
    DisplayableItem(QPixmap image = QPixmap(), QString nameText = "", QString text = "") : img(image), name(nameText), description(text) {}
    QString getName() {return name;}
    QString getDescription() {return description;}

public slots:
    // showing description of item called
    void showDescription() {
        if (name=="") {qInfo() << "No items in this slot, not showing"; return;} // for empty potion & armor slots
        // showing the item
        if (!itemDisplay->isVisible()) {
            itemDisplay->setPos(windowSize.width()*0.5-100,windowSize.height()*0.5-250);
            itemDisplay->show();
        }
        // updating content
        imgDisplay->setPixmap(img.scaled(100, 100));
        descriptionDisplay->setText(name + '\n' + description);
    }
    // hiding any description (any item currently displayed) needs to be static because not linked to a specific item
    static void hideDescription() {
        itemDisplay->hide();
    }

};


#endif // DISPLAYABLEITEM_H
