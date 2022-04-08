#ifndef ENEMY_H
#define ENEMY_H

#include <QString>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>

// Global variable: Size of the window
extern QSize windowSize;

class Enemy {
    friend class GameConsole;
private:
    // attributes
    QString name = "";
    int hp = 10;
    int hpmax = 10;
    int armor = 0;
    int level = 0;
    bool isBoss = false;
    // 0 stun, 1 weak, 2 poisonned
    int status[3] = {0};
    // 0 chance of doing that attack/100, 1 damage, 2 armor, 3 stun, 4 weak, 5 poisonned
    // must be sum of chances: last attack slot used must have a value of 99
    int attacks[4][6]={{0}}; // can have up to 4 different attacks
    int intent = 0; // next attack intent
    // display elements
    QGraphicsPixmapItem* enemyBackground; // backgound of the enemy sprite and parent of all other graphics element of the enemy class
    QGraphicsPixmapItem* enemyImage;
    QGraphicsSimpleTextItem* enemyName;
    QGraphicsRectItem* hpbar;
    QGraphicsRectItem* armorbar;
    QGraphicsSimpleTextItem* hptext;
    QGraphicsPixmapItem* intentImg;
    QGraphicsSimpleTextItem* intentDmg;

public:
    Enemy(QString name = "random", int lvl = 0);
    ~Enemy();
    bool attacked(int effect[4], bool byPlayer = true); // when attacked, 0 damage, 1 stun, 2 weak, 3 poisonned, returns if defeated
    int getAttack(int effect[4]); // when attacking 0 damage, 1 stun, 2 weak, 3 poisonned, return armor for text display
    bool endOfTurn(int &poisondmg); // returns if defeated (by poison)
    void getReward(int &gold, int &exp);
    void display(QGraphicsScene* scene);
    void updateDisplay(bool updateIntents = false); // update intents only after attacking, while display is updated after attacks/potion/getting attacked
};


#endif // ENEMY_H
