#ifndef POTION_H
#define POTION_H

#include "displayableitem.h"

enum PotionType {NoPotion=0, Heal, Damage, Stun, Poison, Weakness};

class Potion : public DisplayableItem {
private:
    PotionType type;
public:
    Potion(PotionType potion) : type(potion) {
        childType = 0; // setting child type

        if (potion==Heal) {
            img = QPixmap(":/ressources/items/healpot.png").scaled(20,20);
            name = QString("Healing Potion");
            description = QString("Heals 50 hp.");
        } else if (potion==Damage) {
            img = QPixmap(":/ressources/items/dmgpot.png").scaled(20,20);
            name = QString("Damage Potion");
            description = QString("Deals 20 damage to the enemy.");
        } else if (potion==Stun){
            img = QPixmap(":/ressources/items/stunpot.png").scaled(20,20);
            name = QString("Stun Potion");
            description = QString("Stuns the enemy; they pass their turn.");
        } else if (potion==Poison) {
            img = QPixmap(":/ressources/items/poisonpot.png").scaled(20,20);
            name = QString("Poison Potion");
            description = QString("Gives 5 poison to the enemy, dealing 5 damage to them at the end of their turn.");
        } else if (potion==Weakness) {
            img = QPixmap(":/ressources/items/weakpot.png").scaled(20,20);
            name = QString("Weakness Potion");
            description = QString("Gives the enemy weak 2, they take 25% more damage from attacks and deal 25% less. Loses 1 stack at the end of their turn.");
        } else { // No potion
            img = QPixmap(":/ressources/items/emptypot.png").scaled(20,20);
        }
    }
    // getting the potion type when used
    PotionType getPotion() {
        return type;
    }
};

#endif // POTION_H
