#include "enemy.h"
#include <QRandomGenerator>
#include <QGraphicsScene>
#include <QDebug>

Enemy::Enemy(QString enemyName, int lvl) : level(lvl) {
    // creates an enemy out of a selection of foes, scaling with level (determined by nb of fights, only applicable for normal foes)
    if (enemyName=="random" || (enemyName!="Skelly" && enemyName!="Zombo" && enemyName!="Snek" && enemyName!="Boss")) { // second part is for safety
        int rand = QRandomGenerator::global()->bounded(100);
        if (rand<40) enemyName = "Skelly";
        else if (rand<80) enemyName = "Zombo";
        else enemyName = "Snek";
    }
    //if (name=="randomboss") {
        // not implemented - yet
    //}
    // skeleton
    if (enemyName=="Skelly") {
        name = "Skelly";
        hp = 15 + level*4;
        hpmax = 15 + level*4;
        // attacks // 0 chance of doing that attack/100, 1 damage, 2 armor, 3 stun, 4 weak, 5 poisonned
        // swing
        attacks[0][0] = 49; attacks[0][1] = 5 + level*3;
        // protect
        attacks[1][0] = 79; attacks[1][2] = 4 + level*2;
        // bone throw
        attacks[2][0] = 99; attacks[2][1] = 2 + level; attacks[2][3] = 10 + lvl*5;
    }
    // Zombie
    if (enemyName=="Zombo") {
        name = "Zombo";
        hp = 25 + level*5;
        hpmax = 25 + level*5;
        // attacks // 0 chance of doing that attack/100, 1 damage, 2 armor, 3 stun, 4 weak, 5 poisonned
        // braaaaaaaaaaaain
        attacks[0][0] = 79; attacks[0][1] = 5 + level*3;
        // vomit
        attacks[1][0] = 99; attacks[1][4] = 1; attacks[1][5] = 2;
    }
    // Snake
    if (enemyName=="Snek") {
        name = "Snek";
        hp = 10 + level*3;
        hpmax = 10 + level*3;
        // attacks
        // deep bite
        attacks[0][0] = 9; attacks[0][1] = 6 + level * 4;
        // go for the knees
        attacks[1][0] = 29; attacks[1][1] = 2 + level; attacks[1][4] = 1;
        // poisonned bite
        attacks[2][0] = 99; attacks[2][1] = 2 + level; attacks[2][5] = 2 + level * 2;
    }
    // Boss
    if (enemyName=="Boss") {
        level = 0;
        name = "Big Bobby";
        isBoss = true;
        hp = 100;
        hpmax = 100;
        // attacks // 0 chance of doing that attack/100, 1 damage, 2 armor, 3 stun, 4 weak, 5 poisonned
        // taunt
        attacks[0][0] = 20; attacks[0][4] = 3;
        // small bonk
        attacks[1][0] = 60; attacks[1][1] = 10; attacks[1][2] = 10;
        // medium bonk
        attacks[2][0] = 89; attacks[2][1] = 20; attacks[2][2] = 5;
        // huge bonk
        attacks[3][0] = 99; attacks[3][1] = 30; attacks[2][3] = 50;
    }
    // random choice of attack for the first attack
    int rand = QRandomGenerator::global()->bounded(100);
    int i = 0;
    while (attacks[i][0] < rand) i++;
    intent = i;
}

Enemy::~Enemy() {
    delete enemyBackground; // deleting all of the childs as well (rest of pointers)
}

bool Enemy::attacked(int effect[4], bool byPlayer) { // by player is false for a potion use, true by default
    // when attacked, 0 damage, 1 stun, 2 weak, 3 poisonned, returns if defeated
    // modify effects to take in account weak
    if (byPlayer && status[1]) effect[0] = (int)effect[0]*1.25;
    // taking damage
    if (armor) {
        int remainingdmg = armor-effect[0];
        if (remainingdmg>=0) armor = remainingdmg;
        else {armor = 0; hp += remainingdmg;}
    } else {hp -= effect[0];}
    // applying status
    status[0] += effect[1];
    status[1] += effect[2];
    status[2] += effect[3];
    // alive test
    if (!byPlayer) this->updateDisplay(); // for potions
    if (hp<=0) return true;
    else return false;
}

// 0 damage, 1 stun, 2 weak, 3 poisonned
int Enemy::getAttack(int effect[4]) { // when attacking, returns if defeated (by poison)
    // checking if stunned
    if (status[0]) { // not attacking
        status[0] = 0;
        effect[0] = 0;
        effect[1] = 0;
        effect[2] = 0;
        effect[3] = 0;
        return 0;
    } else { // attacking
        // checking weak stack for attack
        if (status[1]) effect[0] = (int)attacks[intent][1]*0.75;
        else effect[0] = attacks[intent][1];
        // copying stats
        int randint = QRandomGenerator::global()->bounded(100);
        if (randint < attacks[intent][3]) effect[1] = 1;
        else effect[1] = 0;
        // copying stats
        effect[2] = attacks[intent][4];
        effect[3] = attacks[intent][5];
        // gaining armor if applicable
        armor += attacks[intent][2];
        return attacks[intent][2];
    }
}

bool Enemy::endOfTurn(int &poisondmg) {
    // taking damage equal to poison
    poisondmg = status[2];
    hp -= status[2];
    // losing one stack of weak if applicable
    if(status[1]) status[1]--;
    // alive test
    if (hp<=0) return true;
    else {
        // random choice of attack for next attack
        int rand = QRandomGenerator::global()->bounded(100);
        int i = 0;
        while (attacks[i][0] < rand) i++;
        qInfo() << i;

        intent = i;
        this->updateDisplay(true);
        return false;
    }
}

void Enemy::getReward(int &gold, int &exp) {
    if (isBoss) {
        gold = (level+1) * 100;
        exp = (level+1) * 150;
    } else {
        int rand = QRandomGenerator::global()->bounded(10);
        gold = rand + (level+1)*5;
        rand = QRandomGenerator::global()->bounded(30);
        exp = rand + (level+1)*15;
    }
}

void Enemy::display(QGraphicsScene* scene) { // load the images and display them
    // generate the background, this will be the parent of the rest of the items
    enemyBackground = new QGraphicsPixmapItem(QPixmap(":/ressources/backgrounds/blankCardRoP.png").scaled(200,300));
    // Image
    enemyImage = new QGraphicsPixmapItem(enemyBackground);
    // setting img depending on enemy --------------------------------------------------------------------------------------------------
    if (name=="Skelly") enemyImage->setPixmap(QPixmap(":/ressources/items/minecraftSkeleton.png").scaled(150,150));
    else if (name=="Zombo") enemyImage->setPixmap(QPixmap(":/ressources/items/minecraftZombie.png").scaled(150,150));
    else if (name=="Snek") enemyImage->setPixmap(QPixmap(":/ressources/items/snake.png").scaled(150,150));
    else enemyImage->setPixmap(QPixmap(":/ressources/items/Boss.png").scaled(150,150));
    enemyImage->setPos(25,25);
    // name
    enemyName = new QGraphicsSimpleTextItem(enemyBackground);
    enemyName->setText(name);
    enemyName->setFont(QFont("Calibri", 20, -1,true));
    enemyName->setBrush(QBrush(Qt::white));
    enemyName->setPos(25,170);
    // hp bar, armor bar and hp text
    hpbar = new QGraphicsRectItem(25,210,150,30, enemyBackground);
    hpbar->setBrush(QBrush(Qt::green));
    hpbar->setPen(Qt::NoPen);
    armorbar= new QGraphicsRectItem(25,210,0,30, enemyBackground); //0 width because no armor
    armorbar->setBrush(QBrush(Qt::gray));
    armorbar->setPen(Qt::NoPen);
    hptext = new QGraphicsSimpleTextItem(enemyBackground);
    hptext->setText(QString::number(hp) + '/' + QString::number(hpmax));
    hptext->setFont(QFont("Calibri", 14));
    hptext->setPos(25,215);
    // intent for first attack
    intentImg = new QGraphicsPixmapItem(enemyBackground);
    intentImg->setPixmap(QPixmap(":/ressources/items/str.png").scaled(20,20));
    intentImg->setPos(60,250);
    intentDmg = new QGraphicsSimpleTextItem(enemyBackground);
    intentDmg->setBrush(QBrush(Qt::white));
    if (attacks[intent][1]==0) intentDmg->setText("");
    else if (status[1]) intentDmg->setText(QString::number((int)attacks[intent][1]*0.75));
    else intentDmg->setText(QString::number(attacks[intent][1]));
    intentDmg->setFont(QFont("Calibri", 14, 70));
    intentDmg->setPos(85,245);
    // display the enemy
    enemyBackground->setPos(windowSize.width()*0.5 - 50, windowSize.height()*0.5 - 150); // centering it
    scene->addItem(enemyBackground);
    this->updateDisplay();
}

void Enemy::updateDisplay(bool updateIntents) {
    // updating graphics
    hpbar->setRect(hpbar->rect().x(), hpbar->rect().y(), 150*hp/hpmax, hpbar->rect().height());
    int armorbarwidth;
    if (armor>hpmax) armorbarwidth = hpmax;
    else armorbarwidth = armor;
    armorbar->setRect(armorbar->rect().x(),armorbar->rect().y(), 150*armorbarwidth/hpmax, armorbar->rect().height());

    QString txt("");
    if (armor!=0) txt += '(' + QString::number(armor) + ") ";
    txt += QString::number(hp) + '/' + QString::number(hpmax);
    hptext->setText(txt);
    // updating intents if necessary
    if (updateIntents) {
        if (attacks[intent][1]==0) intentDmg->setText("");
        else if (status[1]) intentDmg->setText(QString::number((int)attacks[intent][1]*0.75));
        else intentDmg->setText(QString::number(attacks[intent][1]));
        if (attacks[intent][1]!=0) intentImg->setPixmap(QPixmap(":/ressources/items/str.png").scaled(20,20));
        else if (attacks[intent][2]!=0) intentImg->setPixmap(QPixmap(":/ressources/items/armor.png").scaled(20,20));
        else intentImg->setPixmap(QPixmap(":/ressources/items/special.png").scaled(20,20));
    }
}
