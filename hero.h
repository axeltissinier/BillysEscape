#ifndef HERO_H
#define HERO_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include "graphicsobjectbutton.h"
#include "weapon.h"
#include "armor.h"
#include "potion.h"

// Global variable: Size of the window
extern QSize windowSize;
//extern QString bitFont;

class Hero : public QObject {
    Q_OBJECT // enable signals and slots
private:
    // attributes
    int hp = 50;
    int hpmax = 50;
    int armor = 0;
    int exp = 0;
    int expmax = 20; // exp needed to reach next level
    int level = 0;
    int pointsToSpend = 0;
    int gold = 0;
    // stats
    int strenght = 0; //boosts raw dmg
    int vitality = 0; //boosts max hp
    int agility = 0; //boosts dodge chance
    int additionalDodgeChance = 0; // for attacks that boost it
    int clarity = 0; //boosts crit chance
    // 0 stun, 1 weak, 2 poisonned
    int status[3] = {0};
    // items and others
    // weapon
    Weapon* weapon;
    // armor
    Armor* helm;
    Armor* boots;
    // potions
    Potion* potions[3];
    // keys
    int keys = 0;
    bool bossKey = false;

    // Character sheet visuals
    QGraphicsRectItem* characterSheet;
    QGraphicsRectItem* hpbar;
    QGraphicsRectItem* armorbar;
    QGraphicsSimpleTextItem* hptext;
    QGraphicsRectItem* expbar;
    QGraphicsSimpleTextItem* exptext;
    // buttons for equipment
    GraphicsObjectButton* equipmentButtons[5];
    QGraphicsPixmapItem* equipmentImg[3];
    // buttons for stats up
    GraphicsObjectButton* statsUpButtons[4];
    QGraphicsSimpleTextItem* statsUpTexts[4];
    // buttons for potions
    GraphicsObjectButton* potionsButtons[3];
    QGraphicsPixmapItem* potionsImg[3];
    // other
    QGraphicsSimpleTextItem* keyText; // number of keys
    QGraphicsSimpleTextItem* goldText; // amount of gold
    QGraphicsPixmapItem* bossKeyPixmap; // image of the boss key

public:
    Hero(QGraphicsScene* scene);
    ~Hero();
    void newCharacterSheet(QGraphicsScene* scene);
    void showLevelUp(bool display);
    bool startOfTurn();
    int getAttack(int attack, int effect[4]); // return armor gain for text display
    bool receiveAttack(int effect[4]);
    bool endOfTurn(int &poisonDmg);
    void endOfCombat();
    PotionType usePotion(int slot);
    void refreshEquipmentImg();
    int isLevel();
    bool canLevelUp();

public slots:
    bool changeHp(int dmg, int armorgain = 0);
    void changeExp(int expgain);
    void levelUp(int statChosen);
    // perform a swap with current item
    bool changeGear(DisplayableItem* &item, int pos=-1); // pos and bool return are for potions
    // change the values if possible
    bool changeGold(int amount);
    bool changeKey(bool getKey);
    bool hasKey(bool isBossKey=false);
    bool changeBossKey(bool getKey);
    void resize();

signals:
    void leveledUp();
};

#endif // HERO_H
