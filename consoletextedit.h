#ifndef CONSOLETEXTEDIT_H
#define CONSOLETEXTEDIT_H

#include <QPlainTextEdit>
#include <QKeyEvent>
#include <QMimeData>
#include <QDebug>

// the console text edit need to have its key input event changed in order to limit the number of characters
// and in the case of wordle entry, limit to five characters and only letters

class ConsoleTextEdit: public QPlainTextEdit {
    Q_OBJECT // enable signals and slots

    friend class GameConsole; // the console will also need access to wordling boolean,
    // this way is faster than getters and setter (as its the only class needing this value)

private:
    bool wordling = false;

protected:
    virtual void keyPressEvent(QKeyEvent* event) override {
        // text and key
        QString txt = event->text();
        // key is in ascii code (int)
        int key = event->key();

        // testing for enter
        if ((key == Qt::Key_Return) || key == Qt::Key_Enter) {
            // return if wordling and not a 5 letter word or if text entered is empty
            // sudo gives out the word ~cheat code of sorts
            if ((wordling && (this->toPlainText().length() != 5) && (this->toPlainText()!="sudo") && (this->toPlainText()!="pass")) || (toPlainText().length() == 0)) return;
            // otherwise emit signal for the console log to process
            emit enterPressed();
            return;
        }
        // we don't want to insert tabulation in the text
        if (key == Qt::Key_Tab) return;

        // check for a combination of keys, because Alt+number breaks the rules we will use next and crashes the game, so we return if we detect it
        int keyInt = key;
        Qt::KeyboardModifiers modifiers = event->modifiers();
        QString keyText = event->text();
        if(modifiers & Qt::AltModifier) keyInt += Qt::ALT;
        QString str = QKeySequence(keyInt).toString(QKeySequence::NativeText);
        if (str=="Alt+0" || str=="Alt+1" || str=="Alt+2" || str=="Alt+3" || str=="Alt+4" ||
            str=="Alt+5" || str=="Alt+6" || str=="Alt+7" || str=="Alt+8" ||str=="Alt+9") {qInfo() << "blocked keypress" << str; return;}

        // emits a signal to make escape menu appear
        if (key == Qt::Key_Escape){ qInfo() << "esc"; emit escPressed(); return;}

        // when the length is over the maximum, accepts only delete, return and escape
        if (!wordling) {
            // In normal phase, 30 chars
            if (toPlainText().length() >= 30) {
                // delete keys OR statement
                bool delCondition = (key == Qt::Key_Delete) || (key == Qt::Key_Backspace) || (key == Qt::Key_Cancel);
                // if not an actual text (text is empty or a delete key is pressed)
                if (txt.isEmpty() || delCondition) {QPlainTextEdit::keyPressEvent(event);}
            } else { // can enter
                QPlainTextEdit::keyPressEvent(event);
            }

        // When in wordle input mode, only accept 5 letters and emit a signal to change text on screen
        } else {
            // delete keys OR statement
            bool delCondition = (key == Qt::Key_Delete) || (key == Qt::Key_Backspace) || (key == Qt::Key_Cancel);
            // there is a special condition with alt + keypad which doesn't return an event with text
            // so we disable all other keys for wordling because it makes the game crash
            bool isLetter = false;
            if (!txt.isEmpty() && toPlainText().length()<5) {
                char letter = (txt.at(0)).toLatin1();
                isLetter = ((letter > 64 && letter < 91) || (letter > 96 && letter < 123));
            }
            if (txt.isEmpty() || delCondition || isLetter) QPlainTextEdit::keyPressEvent(event);
            if (delCondition || isLetter) emit wordleChanged();
        }
    }
    // For copy & paste and text drag
    virtual void insertFromMimeData(const QMimeData* source) override {
        // not available in wordling mode
        if (wordling) return;
        // data to copy (can be more than plain text, but we'll be limiting it to plain text)
        QMimeData txtCopy;
        if (source->hasText()) {
            QString textToPaste = source->text();
            QString curText = toPlainText();
            int totalLength = curText.length() + textToPaste.length();
            if (curText.length() == 30) {
                txtCopy.setText(QString());
            } else if (totalLength > 30) {
                // removing excess copy
                int numToDelete = totalLength - 30;
                textToPaste = textToPaste.left(textToPaste.length() - numToDelete);
                txtCopy.setText(textToPaste);
            } else {
                 txtCopy.setText(textToPaste);
            }
        }
        // inserting result
        QPlainTextEdit::insertFromMimeData(&txtCopy);
    }

signals:
    void enterPressed();
    void escPressed();
    void wordleChanged();
};

#endif // CONSOLETEXTEDIT_H
