#ifndef ARMOR_H
#define ARMOR_H

#include "displayableitem.h"
#include "rarityenum.h"

enum ArmorType {NoArmor, Light, Heavy, Rogue};
enum ArmorPiece {Helmet, Shoes};

class Armor : public DisplayableItem {
private:
    ArmorType type;
    ArmorPiece piece;
    Rarity rarity;
    // stats used when attacking/defending
    // 0 str, 1 vit, 2 crit chance, 3 dodge chance, 4 defence
    int stats[5] = {0};

public:
    Armor(ArmorPiece armorpiece, ArmorType armortype, Rarity r) : type(armortype), piece(armorpiece), rarity(r) {
        childType = 2; // setting child type

        // taking rarity for name
        if (rarity==Common) name = QString("Common ");
        else if (rarity==Uncommon) name = QString("Uncommon ");
        else if (rarity==Rare) name = QString("Rare ");
        // if NoArmor removing name
        if (type==NoArmor) name ="";

        // assign stats depending on type, piece and rarity
        if (armortype==Light) {
            if (armorpiece==Helmet) {
                stats[0] = 1 + r; stats[1] = 1 + r; stats[4] = 10 + 5*r;
                // assigning image and description
                img = QPixmap(":/ressources/items/sunglasses.png").scaled(20,20);
                name += QString("Light helmet");
                description = QString("Increase damage dealt by %1, increase HP by %2, and increase defense by %3.\nDefence: Block a percentage of damage taken.").arg(stats[0]).arg(stats[1]*5).arg(stats[4]);
            } else { // shoes
                childType++; // setting child type

                stats[1] = 1 + r; stats[3] = 7 + 4*r; stats[4] = 10 + 5*r;
                // assigning image and description
                img = QPixmap(":/ressources/items/sandal.png").scaled(20,20);
                name += QString("Light Shoes");
                description = QString("Increase HP by %1, increase dodge chance by %2%, and increase defense by %3.\nDefence: Block a percentage of damage taken.").arg(stats[1]*5).arg(stats[3]).arg(stats[4]);
            }

        } else if (armortype==Heavy) {
            if (armorpiece==Helmet) {
                stats[1] = 2 + 2*r; stats[2] = 4 + 3*r; stats[4] = 10 + 10*r;
                // assigning image and description
                img = QPixmap(":/ressources/items/sunglasses.png").scaled(20,20);
                name += QString("Heavy Helmet");
                description = QString("Increase HP by %1, increase critical chance by %2%, and increase defense by %3.\nDefence: Block a percentage of damage taken.").arg(stats[1]*5).arg(stats[2]).arg(stats[4]);
            } else { // shoes
                childType++; // setting child type

                stats[0] = 2 + r; stats[1] = 2 + 2*r; stats[4] = 10 + 10*r;
                // assigning image and description
                img = QPixmap(":/ressources/items/sandal.png").scaled(20,20);
                name += QString("Heavy Shoes");
                description = QString("Increase damage dealt by %1, increase HP by %2, and increase defense by %3.\nDefence: Block a percentage of damage taken.").arg(stats[0]).arg(stats[1]*5).arg(stats[4]);
            }

        } else if (armortype==Rogue) { // rogue
            if (armorpiece==Helmet) {
                stats[0] = 2 + 2*r; stats[2] = 8 + 6*r; stats[3] = 4 + 3*r; stats[4] = 4 + 3*r;
                // assigning image and description
                img = QPixmap(":/ressources/items/sunglasses.png").scaled(20,20);
                name += QString("Rogue Helmet");
                description = QString("Increase damage dealt by %1, increase critical chance by %2%, increase dodge chance by %3%, and increase defense by %4.\nDefence: Block a percentage of damage taken.").arg(stats[0]).arg(stats[2]).arg(stats[3]).arg(stats[4]);
            } else { // shoes
                childType++; // setting child type

                stats[0] = 2 + r; stats[2] = 4 + 3*r; stats[3] = 8 + 6*r; stats[4] = 4 + 3*r;
                // assigning image and description
                img = QPixmap(":/ressources/items/sandal.png").scaled(20,20);
                name += QString("Rogue Shoes");
                description = QString("Increase damage dealt by %1, increase critical chance by %2%, increase dodge chance by %3%, and increase defense by %4.\nDefence: Block a percentage of damage taken.").arg(stats[0]).arg(stats[2]).arg(stats[3]).arg(stats[4]);
            }
        } else { // No armor
            if (armorpiece==Helmet) {
                img = QPixmap(":/ressources/items/bold.png").scaled(20,20);
            } else img = QPixmap(":/ressources/items/feet.png").scaled(20,20);
        }
    }

    void getStat(int stat[5]) { //copy the stats and return them
        // 0 str, 1 vit, 2 crit chance, 3 dodge chance, 4 defence
        stat[0] = stats[0];
        stat[1] = stats[1];
        stat[2] = stats[2];
        stat[3] = stats[3];
        stat[4] = stats[4];
    }

    ArmorType getType() {return type;}
    ArmorPiece getPiece() {return piece;}
};

#endif // ARMOR_H
