#include "wordlelock.h"
#include <QGraphicsScene>

// to read from the text files
#include <QTextStream>
#include <QFile>
#include <QFont>

#include <QRandomGenerator>
#include <QDebug>
#include <QFontDatabase>


WordleLock::WordleLock(QGraphicsScene* scene) {
    // get number of possible words
    QFile file(":/ressources/wordle_possible_words.txt");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream TxtStream(&file);
        while (TxtStream.readLineInto(nullptr)) NbOfPossibleWords++;
    }
    file.close();

    // adding the lock background
    lockBg = new QGraphicsPixmapItem(QPixmap(":/ressources/backgrounds/grey.png").scaled(300,400));

    // adding rectangle to be colored to the scene
    for (int i=0; i<5; i++) {
        for (int j=0; j<6; j++) {
            letterNodes[i][j] = new QGraphicsRectItem(20+i*55,20+j*62,50,57,lockBg); // -------------  make bg parent
            letterNodes[i][j]->setBrush(Qt::NoBrush);
            letterNodes[i][j]->setPen(QPen(QBrush(Qt::black),3));
        }
    }

    // text (monospaced font)
    int id = QFontDatabase::addApplicationFont(":/ressources/ponderosa.ttf");
    QString bitFont = QFontDatabase::applicationFontFamilies(id).at(0);
    // adding the text holder to the scene
    displayText = new QGraphicsSimpleTextItem(lockBg);
    QFont font(bitFont, 50);
    displayText->setFont(font);
    displayText->setBrush(QBrush(Qt::white));
    displayText->setPos(20,22);

    // adding everything to the scene
    lockBg->setPos(300,20);
    scene->addItem(lockBg);

    // choose a random word
    this->newWord();
    qInfo() << chosenWord;
}

WordleLock::~WordleLock() {
    delete lockBg; // deleting all child at the same time
}


void WordleLock::updateText(const QString &text) {
    displayText->setText(fixedText + text);
}


bool WordleLock::isTextAllowed(const QString &text) {
    // opening file
    QFile file(":/ressources/wordle_allowed_words.txt");
    if (file.open(QIODevice::ReadOnly)) {

        qInfo() << "file opened";

        QTextStream TxtStream(&file);
        // words are stored alphabetically in the original file so it is assumed by this function
        // BE WARY WHEN ADDING WORD TO THE ALLOWED WORDS TEXT FILE -----------------------------
        QString line;
        TxtStream.readLineInto(&line);
        // read line and comapare them lexically, -1 is before, 0 is equal, and 1 is after
        while (QString::compare(line, text, Qt::CaseInsensitive)<0) TxtStream.readLineInto(&line);

        // return if this is the correct word
        file.close();
        return line==text;
    }
    file.close();
    return false;
}

// function will apply color to visual and modify state for display in console (0=grey, 1=yellow, 2=green)
int WordleLock::enterText(const QString &text, QString &state) {
    if (text==chosenWord) return 2;
    // creating local copy of text entered and chosen word because we will need to modify them
    QString chosenWordCopy = chosenWord.left(-1); // deep copy of the string

    // first look if any char is in the right place then remove them (from the deep copy)
    for (int i=0; i<5; i++) {
        if (chosenWord.at(i)==text.at(i)) {
            state[i]='o';
            letterNodes[i][currentLine]->setBrush(QBrush(Qt::green));
            // remove it so that next step can't find it
            chosenWordCopy.remove(i,1);
        } else {
            // putting a value in state in case previous value stored at this location was 'o'
            state[i] = '_';
        }
    }
    // look at the rest of the letters and check if they should be yellow
    int charIndex;
    for (int i=0; i<5; i++) {
        // trying to find the char within the remaining letters of the word if not already attributed
        if (state.at(i)!='o') {
            charIndex = chosenWordCopy.indexOf(text.at(i));
            if (charIndex!=-1) { // letter found (first one starting left)
                state[i]='-';
                letterNodes[i][currentLine]->setBrush(QBrush(Qt::yellow));
                // removing it in case another identical letter comes up
                chosenWordCopy.remove(charIndex,1);
            } else {
                // state[i]='_'; // already done in the first step
                letterNodes[i][currentLine]->setBrush(QBrush(Qt::gray));
            }
        }
    }

    // this line is now complete, we need to increment our line counter and update our text display
    currentLine++;
    fixedText+=text + '\n';
    displayText->setText(fixedText);

    // state has been modified, but we still want to return the result and stopping point
    if (currentLine==6) return 1; // means that we've reached the last line and further input is not possible
    return 0;
}

void WordleLock::newWord() {
    // delete previous visuals and reset data
    currentLine = 0;
    displayText->setText("");
    for (int i=0; i<5; i++) {
        for (int j=0; j<6; j++) {
            letterNodes[i][j]->setBrush(Qt::NoBrush);
        }
    }

    // choose new word
    QFile file(":/ressources/wordle_possible_words.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream TxtStream(&file);
    int randomWordPosition = QRandomGenerator::global()->bounded(NbOfPossibleWords);
    for (int i=0; i<randomWordPosition-1; i++) {TxtStream.readLineInto(nullptr);} // ignore all previous lines
    chosenWord = TxtStream.readLine(); // copy the i-th line to our chosen word
}

void WordleLock::resize() {
    // move background
}
