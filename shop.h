#ifndef SHOP_H
#define SHOP_H

#include "displayableitem.h"
#include "potion.h"
#include "weapon.h"
#include "armor.h"
#include <QRandomGenerator>

class Shop {
    friend class Map;

private:
    DisplayableItem* items[4];
    int prices[4];
    bool sellsKey = true;

public:
    Shop() {
        this->refreshShop();
    }

    ~Shop() {
        // deleting items sold
        for (int i = 0; i<4; i++) {
            delete items[i];
        }
    }

    void refreshShop() {
        int randint;
        for (int i = 0; i<3; i++) {
            // first 2 ones are random armor 65 common, 30 uncommon, 5 rare; 3rd one is always uncommon 75 or rare 25
            Rarity r;
            randint = QRandomGenerator::global()->bounded(100);
            if (i<2) {
                if (randint>=95)  r = Rare;
                else if (randint>=95) r = Uncommon;
                else r = Common;
            } else {
                if (randint>=75)  r = Rare;
                else r = Uncommon;
            }
            randint = QRandomGenerator::global()->bounded(3); // equipment
            if (randint==0) { // weapon
                int randint2 = QRandomGenerator::global()->bounded(4); // type
                if (randint2==0) {
                    items[i] = new Weapon(SwordAndShield, r);
                } else if (randint2==1) {
                    items[i] = new Weapon(BattleAxe, r);
                } else if (randint2==2) {
                    items[i] = new Weapon(Dagger, r);
                } else {
                    items[i] = new Weapon(Bow, r);
                }
            } else { // helmet
                int randint2 = QRandomGenerator::global()->bounded(3); // type
                ArmorPiece piece;
                if (randint==1) {
                    piece = Helmet;
                } else { // shoes
                    piece = Shoes;
                }
                if (randint2==0) {
                    items[i] = new Armor(piece, Heavy, r);
                } else if (randint2==1) {
                    items[i] = new Armor(piece, Light, r);
                } else {
                    items[i] = new Armor(piece, Rogue, r);
                }
            }
            prices[i] = 50 * (r+1);
        }
        // potion
        randint = QRandomGenerator::global()->bounded(5);
        PotionType pottype;
        if (randint==0) pottype = Heal;
        else if (randint==0) pottype = Damage;
        else if (randint==0) pottype = Stun;
        else if (randint==0) pottype = Poison;
        else pottype = Weakness;
        items[3] = new Potion(pottype);
        prices[3] = 40; // price of potion is always 40
    }

    DisplayableItem* buyItem(int item) {
        // if key
        if (item==4) {
            sellsKey = false;
            return nullptr;
        }
        // else
        DisplayableItem* itemBought = items[item];
        items[item] = new DisplayableItem(QPixmap(), "Sold!");
        prices[item] = 0;
        return itemBought;
    }
    // get item list for printing, returns if it sells key
    bool getItems(DisplayableItem* itemsList[4], int itemsPrice[4]) {
        // copy items
        itemsList[0] = items[0];
        itemsList[1] = items[1];
        itemsList[2] = items[2];
        itemsList[3] = items[3];
        itemsPrice[0] = prices[0];
        itemsPrice[1] = prices[1];
        itemsPrice[2] = prices[2];
        itemsPrice[3] = prices[3];
        return sellsKey;
    }
};

#endif // SHOP_H
