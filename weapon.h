#ifndef WEAPON_H
#define WEAPON_H

#include "displayableitem.h"
#include "rarityenum.h"
#include <QDebug>

enum WeaponType {SwordAndShield, BattleAxe, Dagger, Bow};

class Weapon : public DisplayableItem {
private:
    WeaponType weapontype; // necessary? --*-*-*-*-*-**-*-*-**-*-*-*-*-*-*-*--*-*-*-*-*--*---*-*-*-*-*-*-*--
    Rarity rarity;
    // names of attacks and short description
    QString shortDesc[3][2];
    // 0 raw dmg, 1 armor, 2 dodge chance, 3 crit chance, 4 stun chance percent, 5 weak applied, 6 poison applied
    int stats[3][7] = {{0}}; // fill with zeros

public:
    Weapon(WeaponType type, Rarity r) : weapontype(type), rarity(r) {
        childType = 1; // setting child type

        // taking rarity for name
        if (rarity==Common) name = QString("Common ");
        else if (rarity==Uncommon) name = QString("Uncommon ");
        else name = QString("Rare ");

        // assign stats depending on type and rarity
        if (type==SwordAndShield) {
            stats[0][0] = 6 + 3*r; stats[0][1] = 6 + 3*r;
            stats[1][0] = 4 + 3*r; stats[1][1] = 4 + 3*r; stats[1][4] = 30 + 20*r;
            stats[2][1] = 14 + r*6;
            // assigning image and description
            img = QPixmap(":/ressources/items/SwordAndShield.png").scaled(20,20);
            name += QString("Sword and shield");
            // description
            shortDesc[0][0] = QString("Safe Blow");
            shortDesc[0][1] = QString("Deal %1 damage and gain %2 armor points.").arg(stats[0][0]).arg(stats[0][1]);
            shortDesc[1][0] = QString("Shield Bash");
            shortDesc[1][1] = QString("Deal %1 damage and gain %2 armor points. Has %3% chance to stun the enemy.").arg(stats[1][0]).arg(stats[1][1]).arg(stats[1][4]);
            shortDesc[2][0] = QString("Armor Up!");
            shortDesc[2][1] = QString("Gain %1 armor points.").arg(stats[2][1]);
            description = "1- " + shortDesc[0][0] + ": " + shortDesc[0][1] + "\n2- " + shortDesc[1][0] + ": " + shortDesc[1][1] + "\n3- " + shortDesc[2][0] + ": " + shortDesc[2][1];
            description += "\nArmor Points: Protect from enemy attacks.\nStun: Passes its turn.";

        } else if (type==BattleAxe) {
            stats[0][0] = 6 +3*r; stats[0][1] = 4 + 2*r;
            stats[1][0] = 12 + r*4;
            stats[2][0] = 1 + r; stats[2][4] = 20 + 10*r; stats[2][5] = 1 + r;
            // assigning image and description
            img = QPixmap(":/ressources/items/battleaxe.png").scaled(20,20);
            name += QString("Battle Axe");
            // description
            shortDesc[0][0] = QString("Safe Blow");
            shortDesc[0][1] = QString("Deal %1 damage and gain %2 armor points.").arg(stats[0][0]).arg(stats[0][1]);
            shortDesc[1][0] = QString("Heavy Blow");
            shortDesc[1][1] = QString("Deal %1 damage.").arg(stats[1][0]);
            shortDesc[2][0] = QString("Battle Shout");
            shortDesc[2][1] = QString("Deal %1 damage and apply %2 weak to the enemy. Has %3% chance to stun the enemy.").arg(stats[2][0]).arg(stats[2][5]).arg(stats[2][4]);
            description = "1- " + shortDesc[0][0] + ": " + shortDesc[0][1] + "\n2- " + shortDesc[1][0] + ": " + shortDesc[1][1] + "\n3- " + shortDesc[2][0] + ": " + shortDesc[2][1];
            description += "\nArmor Points: Protect from enemy attacks.\nStun: Passes its turn.\nWeak: Take 25% more damage from attacks and deal 25% less damage. Lose 1 stack at the end of their turn.";

        } else if (type==Dagger) {
            stats[0][0] = 6 + r*3; stats[0][2] = 30 + r*20;
            stats[1][0] = 8 + r*3; stats[1][3] = 40 + r*20;
            stats[2][4] = 20 + r*15; stats[2][5] = 1 + r; stats[2][6] = 2 + r;
            // assigning image and description
            img = QPixmap(":/ressources/items/dagger.png").scaled(20,20);
            name += QString("Dagger");
            // description
            shortDesc[0][0] = QString("Evasive Stab");
            shortDesc[0][1] = QString("Deal %1 damage and gain %2% dodge chance for this turn.").arg(stats[0][0]).arg(stats[0][2]);
            shortDesc[1][0] = QString("Go for the Throat");
            shortDesc[1][1] = QString("Gain %1% critical chance for this turn and deal %2 damage.").arg(stats[1][3]).arg(stats[1][0]);
            shortDesc[2][0] = QString("Strange Potion");
            shortDesc[2][1] = QString("Apply %1 poison and %2 weak to the enemy. Has %3% chance to stun the enemy.").arg(stats[2][6]).arg(stats[2][5]).arg(stats[2][4]);
            description = "1- " + shortDesc[0][0] + ": " + shortDesc[0][1] + "\n2- " + shortDesc[1][0] + ": " + shortDesc[1][1] + "\n3- " + shortDesc[2][0] + ": " + shortDesc[2][1];
            description += "Stun: Passes its turn.\nPoison: Take damage equal to the number of stacks at the end of their turn. Ignore armor.\nWeak: Take 25% more damage from attacks and deal 25% less damage. Lose 1 stack at the end of their turn.";

        } else {
            stats[0][0] = 6 + r*3; stats[0][2] = 30 + r*20;
            stats[1][0] = 8 + r*3; stats[1][3] = 20 + r*15; stats[1][4] = 20 + r * 10;
            stats[2][0] = 2 + r; stats[2][6] = 4 + r*3;
            // assigning image and description
            img = QPixmap(":/ressources/items/bowMC.png").scaled(20,20);
            name += QString("Bow");
            // description
            shortDesc[0][0] = QString("Evasive Shot");
            shortDesc[0][1] = QString("Deal %1 damage and gain %2% dodge chance for this turn.").arg(stats[0][0]).arg(stats[0][2]);
            shortDesc[1][0] = QString("Aim for the Head");
            shortDesc[1][1] = QString("Gain %1% critical chance for this turn and deal %2 damage. Has %3% chance to stun the enemy.").arg(stats[1][3]).arg(stats[1][0]).arg(stats[1][4]);
            shortDesc[2][0] = QString("Poison Arrow");
            shortDesc[2][1] = QString("Deal %1 damage and apply %2 poison to the enemy").arg(stats[2][0]).arg(stats[2][6]);
            description = "1- " + shortDesc[0][0] + ": " + shortDesc[0][1] + "\n2- " + shortDesc[1][0] + ": " + shortDesc[1][1] + "\n3- " + shortDesc[2][0] + ": " + shortDesc[2][1];
            description += "Stun: Passes its turn.\nPoison: Take damage equal to the number of stacks at the end of their turn. Ignore armor.\nWeak: Take 25% more damage from attacks and deal 25% less damage. Lose 1 stack at the end of their turn.";
        }
    }
    // copy the stats and return them depending on attack planned
    void getStats(int attackNb, int stat[7]) {
        qInfo() << "getting info for attack" << attackNb;
        stat[0]=stats[attackNb][0];
        stat[1]=stats[attackNb][1];
        stat[2]=stats[attackNb][2];
        stat[3]=stats[attackNb][3];
        stat[4]=stats[attackNb][4];
        stat[5]=stats[attackNb][5];
        stat[6]=stats[attackNb][6];
    }
    // return the description for item display in action bar
    void getShortDesc(QString shrtdesc[3][2]) {
        // copying values
        shrtdesc[0][0] = shortDesc[0][0];
        shrtdesc[0][1] = shortDesc[0][1];
        shrtdesc[1][0] = shortDesc[1][0];
        shrtdesc[1][1] = shortDesc[1][1];
        shrtdesc[2][0] = shortDesc[2][0];
        shrtdesc[2][1] = shortDesc[2][1];
    }
};

#endif // WEAPON_H
