#ifndef WORDLELOCK_H
#define WORDLELOCK_H

#include <QString>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>

// Global variable: Size of the window
extern QSize windowSize;
//extern QString bitFont;

class WordleLock {
    friend class GameConsole;
private:
    QString chosenWord;
    int currentLine = 0;
    int NbOfPossibleWords = 0;
    // visuals that will be added to the scene
    QGraphicsPixmapItem* lockBg;
    QString fixedText = ""; // corespond to text that has been entered
    QGraphicsSimpleTextItem* displayText;
    QGraphicsRectItem* letterNodes[5][6];

public:
    WordleLock(QGraphicsScene* scene);
    ~WordleLock();
    void updateText(const QString &text);
    bool isTextAllowed(const QString &text); // assuming that text is stored alphabetically BE WARY WHEN ADDING WORD TO THE ALLOWED WORD TXT
    int enterText(const QString &text, QString &state); // return 0 if incorrect guess, 1 if correct guess, 2 if incorrect guess but last line allowed
    void newWord();
    void resize();

};

#endif // WORDLELOCK_H
