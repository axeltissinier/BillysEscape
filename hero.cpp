#include "hero.h"
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QFont>
#include <QFontDatabase>
#include <QRandomGenerator>
#include <QScrollBar>



Hero::Hero(QGraphicsScene* scene) {

    qInfo() << "init of static";
    // initializing static members
    DisplayableItem::itemDisplay = new QGraphicsPixmapItem(QPixmap(":/ressources/backgrounds/blankCardRoP.png").scaled(200,300));
    DisplayableItem::imgDisplay = new QGraphicsPixmapItem(DisplayableItem::itemDisplay);
    DisplayableItem::imgDisplay->setPos(50,40);
    DisplayableItem::descriptionDisplay = new QLabel("");
    DisplayableItem::descriptionDisplay->setWordWrap(true);
    DisplayableItem::descriptionDisplay->setAlignment(Qt::AlignJustify); // justify text
    DisplayableItem::descriptionDisplay->setStyleSheet("background-color:transparent; color: white");
    DisplayableItem::textArea = new QScrollArea();
    DisplayableItem::textArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    DisplayableItem::textArea->setWidgetResizable(true);
    DisplayableItem::textArea->setGeometry(40, 150, 140, 110);
    DisplayableItem::textArea->setStyleSheet("border:none; background-color:transparent;");
    DisplayableItem::customScrollBar = new QScrollBar(); // scroll bar of the text area
    DisplayableItem::customScrollBar->setStyleSheet("QScrollBar:vertical {border: none; background-color: transparent; width: 15px;}"
                       "QScrollBar::handle:vertical {background: white; border: none; min-height: 20px;}"
                       "QScrollBar::add-line:vertical {border: none; background: none; height: 0px;subcontrol-position: bottom;subcontrol-origin: margin;}"
                       "QScrollBar::sub-line:vertical {border: none; background: none; height: 0px;subcontrol-position: top;subcontrol-origin: margin;}"
                       "QScrollBar::add-page:vertical{background-color: transparent;}"
                       "QScrollBar::sub-page:vertical{background-color: transparent;}");
    DisplayableItem::textArea->setVerticalScrollBar(DisplayableItem::customScrollBar);
    DisplayableItem::textArea->setWidget(DisplayableItem::descriptionDisplay);
    DisplayableItem::widgetProxy = new QGraphicsProxyWidget(DisplayableItem::itemDisplay);
    DisplayableItem::widgetProxy->setWidget(DisplayableItem::textArea);
    DisplayableItem::closeButton = new TextButton("Close",0,0,QFont("Calibri", 16), QFont("Calibri", 16, 75), QBrush(Qt::white), QBrush(Qt::white),QBrush(Qt::gray),DisplayableItem::itemDisplay);
    DisplayableItem::closeButton->setPos(80,265);
    QObject::connect(DisplayableItem::closeButton, &TextButton::clicked, DisplayableItem::hideDescription);
    DisplayableItem::itemDisplay->setZValue(1); // putting it over the rest of the items
    scene->addItem(DisplayableItem::itemDisplay); // adding them to the scene
    DisplayableItem::itemDisplay->hide(); // hiding them (will be shown when showDescription function is called)

    qInfo() << "creating hero: armor etc";

    // start with no armor and a sword and shield (also a healing potion)
    weapon = new Weapon(SwordAndShield, Common);
    // no armor at start
    helm = new Armor(Helmet, NoArmor, Common);
    boots = new Armor(Shoes, NoArmor, Common);
    // filling potion with one healing potion
    potions[0] = new Potion(Heal);
    potions[1] = new Potion(NoPotion);
    potions[2] = new Potion(NoPotion);

    // creating character sheet
    this->newCharacterSheet(scene);

}

Hero::~Hero() {
    delete characterSheet; // deleting all of its children, which is every pointer in private and some not keeped
    // delete private pointers
    delete weapon;
    delete helm;
    delete boots;
    delete potions[0];
    delete potions[1];
    delete potions[2];
    //delete the statics of displayable items and reset them
    delete DisplayableItem::itemDisplay;
    delete DisplayableItem::imgDisplay;
    delete DisplayableItem::descriptionDisplay;
    delete DisplayableItem::widgetProxy;
    delete DisplayableItem::textArea;
    delete DisplayableItem::customScrollBar;
    delete DisplayableItem::closeButton;
    DisplayableItem::itemDisplay = NULL;
    DisplayableItem::imgDisplay = NULL;
    DisplayableItem::descriptionDisplay = NULL;
    DisplayableItem::widgetProxy = NULL;
    DisplayableItem::textArea = NULL;
    DisplayableItem::customScrollBar = NULL;
    DisplayableItem::closeButton = NULL;

}



void Hero::newCharacterSheet(QGraphicsScene* scene) {
    // creating sheet background, this will be the parent of all other item contained in the sheet
    characterSheet = new QGraphicsRectItem(0,0,200,500);
    characterSheet->setBrush(Qt::NoBrush);
    characterSheet->setPen(Qt::NoPen);

    // name
    QGraphicsSimpleTextItem* dummyTxt = new QGraphicsSimpleTextItem(QString("Billy"), characterSheet);
    dummyTxt->setFont(QFont("Calibri", 20, -1,true));
    dummyTxt->setPos(20,10);
    // hp bar and stroke
    QGraphicsRectItem* hpRect = new QGraphicsRectItem(20,50,160,30,characterSheet);
    hpRect->setBrush(Qt::NoBrush);
    hpRect->setPen(QPen(QBrush(Qt::black),3));
    hpbar = new QGraphicsRectItem(20,50,160,30,characterSheet);
    hpbar->setBrush(QBrush(Qt::green));
    hpbar->setPen(Qt::NoPen);
    armorbar = new QGraphicsRectItem(20,50,0,30,characterSheet); // width to zero because no armor at start
    armorbar->setBrush(QBrush(Qt::gray));
    armorbar->setPen(Qt::NoPen);
    hptext = new QGraphicsSimpleTextItem(QString("50/50"), characterSheet);
    hptext->setFont(QFont("Calibri", 18));
    hptext->setToolTip(QString("Health points: if they fall to 0, it's game over."));
    hptext->setPos(25,50);

    // exp bar
    QGraphicsRectItem* expRect = new QGraphicsRectItem(20,100,160,25,characterSheet);
    expRect->setBrush(QBrush(Qt::gray));
    expRect->setPen(QPen(QBrush(Qt::black),3));
    expbar = new QGraphicsRectItem(20,100,0,25,characterSheet); // no experience at start
    expbar->setBrush(QBrush(Qt::blue));
    expbar->setPen(Qt::NoPen);
    exptext = new QGraphicsSimpleTextItem(QString("0/20"), characterSheet);
    exptext->setFont(QFont("Calibri", 14));
    exptext->setToolTip(QString("Experience points: obtained by killing enemies;\nlevel up grants a point to improve one stat."));
    exptext->setPos(25,100);

    // equipment buttons

    qInfo() << "creating visuals for equipment";

    // weapon
    equipmentButtons[0] = new GraphicsObjectButton(30,30,100,characterSheet);
    equipmentImg[0] = new QGraphicsPixmapItem(weapon->img.scaled(20,20), equipmentButtons[0]);
    equipmentImg[0]->setPos(5,5);
    equipmentButtons[0]->setPos(20,150);
    QObject::connect(equipmentButtons[0], SIGNAL(clicked()), weapon, SLOT(showDescription()));
    // helmet
    qInfo() << "helmet";
    equipmentButtons[1] = new GraphicsObjectButton(30,30,100,characterSheet);
    equipmentImg[1] = new QGraphicsPixmapItem(helm->img.scaled(20,20), equipmentButtons[1]);
    equipmentImg[1]->setPos(5,5);
    equipmentButtons[1]->setPos(20,190);
    QObject::connect(equipmentButtons[1], SIGNAL(clicked()),helm, SLOT(showDescription()));
    // abs
    equipmentButtons[2] = new GraphicsObjectButton(30,30,100,characterSheet);
    DisplayableItem* dummyDispItem = new DisplayableItem(QPixmap(":/ressources/items/abs.png").scaled(20,20), "Your abs", "Who needs armor when you have steels abs.");
    QGraphicsPixmapItem* dummyPixmap = new QGraphicsPixmapItem(dummyDispItem->img, equipmentButtons[2]);
    dummyPixmap->setPos(5,5);
    equipmentButtons[2]->setPos(65,190);
    QObject::connect(equipmentButtons[2], SIGNAL(clicked()), dummyDispItem, SLOT(showDescription()));
    // lucky pants
    equipmentButtons[3] = new GraphicsObjectButton(30,30,100,characterSheet);
    dummyDispItem = new DisplayableItem(QPixmap(":/ressources/items/pants.png").scaled(20,20), "Lucky pants", "Never took them off as far as you can remember, and planning to. Increase luck by 100%.");
    dummyPixmap = new QGraphicsPixmapItem(dummyDispItem->img, equipmentButtons[3]);
    dummyPixmap->setPos(5,5);
    equipmentButtons[3]->setPos(110,190);
    QObject::connect(equipmentButtons[3], SIGNAL(clicked()), dummyDispItem, SLOT(showDescription()));
    // boots
    equipmentButtons[4] = new GraphicsObjectButton(30,30,100,characterSheet);
    equipmentImg[2] = new QGraphicsPixmapItem(boots->img.scaled(20,20), equipmentButtons[4]);
    equipmentImg[2]->setPos(5,5);
    equipmentButtons[4]->setPos(155,190);
    QObject::connect(equipmentButtons[4], SIGNAL(clicked()), boots, SLOT(showDescription()));

    qInfo() << "stats up";

    // stats up button, stats pixmap & text
    statsUpButtons[0] = new GraphicsObjectButton(12,12,10,characterSheet);
    dummyTxt = new QGraphicsSimpleTextItem(QString('+'),statsUpButtons[0]);
    dummyTxt->setFont(QFont("Calibri", 14));
    dummyTxt->setPos(2,-6);
    statsUpButtons[0]->setPos(30,240);
    QObject::connect(statsUpButtons[0], &GraphicsObjectButton::clicked, [this]{levelUp(0);});
    dummyPixmap = new QGraphicsPixmapItem(QPixmap(":/ressources/items/str.png").scaled(25,25), characterSheet);
    dummyPixmap->setPos(20, 250);
    statsUpTexts[0] = new QGraphicsSimpleTextItem("0", characterSheet);
    statsUpTexts[0]->setFont(QFont("Calibri", 12));
    statsUpTexts[0]->setToolTip(QString("Strenght: Increase damage from attacks by this number."));
    statsUpTexts[0]->setPos(40,250);

    statsUpButtons[1] = new GraphicsObjectButton(12,12,10,characterSheet);
    dummyTxt = new QGraphicsSimpleTextItem(QString('+'), statsUpButtons[1]);
    dummyTxt->setFont(QFont("Calibri", 14));
    dummyTxt->setPos(2,-6);
    statsUpButtons[1]->setPos(70,240);
    QObject::connect(statsUpButtons[1], &GraphicsObjectButton::clicked, [this]{levelUp(1);});
    dummyPixmap = new QGraphicsPixmapItem(QPixmap(":/ressources/items/vit.png").scaled(25,25), characterSheet);
    dummyPixmap->setPos(60, 250);
    statsUpTexts[1] = new QGraphicsSimpleTextItem("0", characterSheet);
    statsUpTexts[1]->setFont(QFont("Calibri", 12));
    statsUpTexts[1]->setToolTip(QString("Vitality: Increase health by 5 times this number."));
    statsUpTexts[1]->setPos(80,250);

    statsUpButtons[2] = new GraphicsObjectButton(12,12,10,characterSheet);
    dummyTxt = new QGraphicsSimpleTextItem(QString('+'), statsUpButtons[2]);
    dummyTxt->setFont(QFont("Calibri", 14));
    dummyTxt->setPos(2,-6);
    statsUpButtons[2]->setPos(110,240);
    QObject::connect(statsUpButtons[2], &GraphicsObjectButton::clicked, [this]{levelUp(2);});
    dummyPixmap = new QGraphicsPixmapItem(QPixmap(":/ressources/items/agi.png").scaled(25,25), characterSheet); // -------------------
    dummyPixmap->setPos(100, 250);
    statsUpTexts[2] = new QGraphicsSimpleTextItem("0", characterSheet);
    statsUpTexts[2]->setFont(QFont("Calibri", 12));
    statsUpTexts[2]->setToolTip(QString("Agility: Increase dodge chance by this number."));
    statsUpTexts[2]->setPos(125,250);

    statsUpButtons[3] = new GraphicsObjectButton(12,12,10,characterSheet);
    dummyTxt = new QGraphicsSimpleTextItem(QString('+'), statsUpButtons[3]);
    dummyTxt->setFont(QFont("Calibri", 14));
    dummyTxt->setPos(2,-6);
    statsUpButtons[3]->setPos(150,240);
    QObject::connect(statsUpButtons[3], &GraphicsObjectButton::clicked, [this]{levelUp(3);});
    dummyPixmap = new QGraphicsPixmapItem(QPixmap(":/ressources/items/cla.png").scaled(25,25), characterSheet); // -------------------
    dummyPixmap->setPos(140, 250);
    statsUpTexts[3] = new QGraphicsSimpleTextItem("0", characterSheet);
    statsUpTexts[3]->setFont(QFont("Calibri", 12));
    statsUpTexts[3]->setToolTip(QString("Clarity: Increase critical chance by this number."));
    statsUpTexts[3]->setPos(160,250);

    this->showLevelUp(false); // hides all stats buttons because level 0;

    qInfo() << "gold & keys";
    // gold
    dummyPixmap = new QGraphicsPixmapItem(QPixmap(":/ressources/items/coin.png").scaled(15,15), characterSheet); // ------------------------ gold
    dummyPixmap->setPos(35,300);
    goldText = new QGraphicsSimpleTextItem(QString('0'), characterSheet);
    goldText->setFont(QFont("Calibri", 10));
    goldText->setPos(55,300);
    // key
    dummyPixmap = new QGraphicsPixmapItem(QPixmap(":/ressources/items/key.png").scaled(15,10), characterSheet); // ------------------------ key
    dummyPixmap->setPos(90,300);
    keyText = new QGraphicsSimpleTextItem(QString('0'), characterSheet);
    keyText->setFont(QFont("Calibri", 10));
    keyText->setPos(110,300);
    bossKeyPixmap = new QGraphicsPixmapItem(QPixmap(":/ressources/items/bossKeyZelda.png").scaled(20,20), characterSheet);
    bossKeyPixmap->setToolTip(QString("Opens the door the the boss room."));
    bossKeyPixmap->setPos(150,300);
    bossKeyPixmap->hide();

    qInfo() << "potion slots";
    // potion button & images
    potionsButtons[0] = new GraphicsObjectButton(30,30,20,characterSheet);
    potionsImg[0] = new QGraphicsPixmapItem(potions[0]->img.scaled(20,20), potionsButtons[0]);
    potionsImg[0]->setPos(5,5);
    potionsButtons[0]->setPos(40,330);
    QObject::connect(potionsButtons[0], SIGNAL(clicked()), potions[0], SLOT(showDescription()));

    potionsButtons[1] = new GraphicsObjectButton(30,30,20,characterSheet);
    potionsImg[1] = new QGraphicsPixmapItem(potions[1]->img.scaled(20,20), potionsButtons[1]);
    potionsImg[1]->setPos(5,5);
    potionsButtons[1]->setPos(90,330);
    QObject::connect(potionsButtons[1], SIGNAL(clicked()), potions[1], SLOT(showDescription()));

    potionsButtons[2] = new GraphicsObjectButton(30,30,20,characterSheet);
    potionsImg[2] = new QGraphicsPixmapItem(potions[2]->img.scaled(20,20), potionsButtons[2]);
    potionsImg[2]->setPos(5,5);
    potionsButtons[2]->setPos(140,330);
    QObject::connect(potionsButtons[2], SIGNAL(clicked()), potions[2], SLOT(showDescription()));

    qInfo()  << "adding to the scene";
    // adding parent to the scene
    int windowx = windowSize.width();
    characterSheet->setPos(windowx*0.75,20);
    scene->addItem(characterSheet);
}


void Hero::showLevelUp(bool display) {
    if (display) {
        for (int i =0; i<4; i++) statsUpButtons[i]->show();
    } else {
        for (int i =0; i<4; i++) statsUpButtons[i]->hide();
    }
}

int Hero::isLevel() {
    return level;
}

bool Hero::canLevelUp() {
    if (pointsToSpend==0) return false;
    else return true;
}

bool Hero::changeHp(int dmg, int armorgain) {
    // change in health
    if (dmg>0 && armor>0) { // dmg partially negated
        int remainingdmg = armor-dmg;
        if (remainingdmg>=0) armor = remainingdmg;
        else {armor = 0; hp += remainingdmg;}
    } else { // heal or non negated dmg
        hp = hp-dmg;
        if (hp>hpmax) hp=hpmax;
    }
    // change in armor
    armor += armorgain;
    // updating graphics
    hpbar->setRect(hpbar->rect().x(),hpbar->rect().y(),160*hp/hpmax,hpbar->rect().height());
    int armorbarwidth;
    if (armor>hpmax) armorbarwidth = hpmax;
    else armorbarwidth = armor;
    armorbar->setRect(armorbar->rect().x(),armorbar->rect().y(),armorbarwidth*160/hpmax,armorbar->rect().height());
    // text in hp bar
    QString str = QString::number(hp)+'/'+QString::number(hpmax);
    if (armor>0) str = '(' + QString::number(armor) + ") "+ str;
    hptext->setText(str);

    // return if dead or alive
    if (hp<=0) return false;
    else return true;
}

void Hero::changeExp(int expgain) {
    exp +=expgain;
    while (exp>expmax) {
        exp-=expmax;
        level++;
        pointsToSpend++;
        expmax = expmax+10*level;
        this->showLevelUp(true);
        emit leveledUp();
    }
    //updating graphics
    qreal expleft = 160*exp/expmax;
    expbar->setRect(expbar->rect().x(),expbar->rect().y(),expleft,expbar->rect().height());
    // text in exp bar
    exptext->setText(QString::number(exp)+'/'+QString::number(expmax));
}

void Hero::levelUp(int statChosen) {
    qInfo() << "leveling up" << statChosen;

    if (statChosen==0) {strenght++; statsUpTexts[0]->setText(QString::number(strenght));}
    else if (statChosen==1) {
        vitality++;
        hp +=5;
        hpmax+=5;
        changeHp(0); // update visuals
        statsUpTexts[1]->setText(QString::number(vitality));
    }
    else if (statChosen==2) {agility++; statsUpTexts[2]->setText(QString::number(agility));}
    else {clarity++;  statsUpTexts[3]->setText(QString::number(clarity));}
    pointsToSpend--;
    if (pointsToSpend==0) showLevelUp(false);
}

// swap this item with the item equiped (can be a No Armor or No Potion, then nullptr is returned)
// returns false if it failed (for potion)
bool Hero::changeGear(DisplayableItem* &item, int pos){
    if (item->childType==0) { // potion
        Potion* pot = dynamic_cast<Potion*>(item);
        if (pos==1 || (pos==-1 && potions[0]->getPotion()==NoPotion)) { // slot 1
            Potion* equipedPotion = potions[0];
            bool returnnull = (equipedPotion->getPotion()==NoPotion);
            potions[0] = pot;
            // disconnect old item and connect new item (for description panel)
            QObject::disconnect(potionsButtons[0], SIGNAL(clicked()), equipedPotion, SLOT(showDescription()));
            QObject::connect(potionsButtons[0], SIGNAL(clicked()), potions[0], SLOT(showDescription()));
            // returns a nullptr to not add an inexistant item to the room
            if (returnnull) {delete equipedPotion; item = nullptr;}
            else item = equipedPotion;
        } else if (pos==2 || (pos==-1 && potions[1]->getPotion()==NoPotion)) { // slot 2
            Potion* equipedPotion = potions[1];
            bool returnnull = (equipedPotion->getPotion()==NoPotion);
            potions[1] = pot;
            // disconnect old item and connect new item (for description panel)
            QObject::disconnect(potionsButtons[1], SIGNAL(clicked()), equipedPotion, SLOT(showDescription()));
            QObject::connect(potionsButtons[1], SIGNAL(clicked()), potions[1], SLOT(showDescription()));
            // returns a nullptr to not add an inexistant item to the room
            if (returnnull) {delete equipedPotion; item = nullptr;}
            else item = equipedPotion;
        } else if (pos==3 || (pos==-1 && potions[2]->getPotion()==NoPotion)) { // slot 3
            Potion* equipedPotion = potions[2];
            bool returnnull = (equipedPotion->getPotion()==NoPotion);
            potions[2] = pot;
            // disconnect old item and connect new item (for description panel)
            QObject::disconnect(potionsButtons[2], SIGNAL(clicked()), equipedPotion, SLOT(showDescription()));
            QObject::connect(potionsButtons[2], SIGNAL(clicked()), potions[2], SLOT(showDescription()));
            // returns a nullptr to not add an inexistant item to the room
            if (returnnull) {delete equipedPotion; item = nullptr;}
            else item = equipedPotion;
        } else return false;

    } else if (item->childType==1) { // weapon
        DisplayableItem* itemequiped = weapon;
        Weapon* newweapon = dynamic_cast<Weapon*>(item);
        weapon = newweapon;
        // disconnect old item and connect new item (for description panel)
        QObject::disconnect(equipmentButtons[0], SIGNAL(clicked()), itemequiped, SLOT(showDescription()));
        QObject::connect(equipmentButtons[0], SIGNAL(clicked()), weapon, SLOT(showDescription()));
        // return item to add to the room
        item = itemequiped;

    } else if (item->childType==2) { // helmet
        int armorstats[5];
        helm->getStat(armorstats);
        int oldHpBuff = 5*armorstats[1];

        DisplayableItem* itemequiped = helm;
        bool returnnull = (helm->getType()==NoArmor);
        Armor* newarmor = dynamic_cast<Armor*>(item);
        helm = newarmor;

        helm->getStat(armorstats);
        int newHpBuff = 5*armorstats[1];
        hp+=newHpBuff-oldHpBuff;
        hpmax+=newHpBuff-oldHpBuff;
        changeHp(0); //update visuals

        // disconnect old item and connect new item (for description panel)
        QObject::disconnect(equipmentButtons[1], SIGNAL(clicked()),itemequiped, SLOT(showDescription()));
        QObject::connect(equipmentButtons[1], SIGNAL(clicked()),helm, SLOT(showDescription()));
        // returns a nullptr to not add an inexistant item to the room
        if (returnnull) {delete itemequiped; item = nullptr;}
        else item = itemequiped;

    } else if (item->childType==3) { // shoes
        int armorstats[5];
        boots->getStat(armorstats);
        int oldHpBuff = 5*armorstats[1];

        DisplayableItem* itemequiped = boots;
        bool returnnull = (boots->getType()==NoArmor);
        Armor* newarmor = dynamic_cast<Armor*>(item);
        boots = newarmor;

        boots->getStat(armorstats);
        int newHpBuff = 5*armorstats[1];
        hp+=newHpBuff-oldHpBuff;
        hpmax+=newHpBuff-oldHpBuff;
        changeHp(0); //update visuals

        // disconnect old item and connect new item (for description panel)
        QObject::disconnect(equipmentButtons[4], SIGNAL(clicked()), itemequiped, SLOT(showDescription()));
        QObject::connect(equipmentButtons[4], SIGNAL(clicked()), boots, SLOT(showDescription()));
        // returns a nullptr to not add an inexistant item to the room
        if (returnnull) {delete itemequiped; item = nullptr;}
        else item = itemequiped;
    } else item = nullptr;

    // update graphics
    this->refreshEquipmentImg();
    return true;
}


// combat stuff ----------------------------------------------------------------------------------------------------------------
bool Hero::startOfTurn() { // return if he is stunned
    // remove additionnal dodge chance
    additionalDodgeChance = 0;
    // if stunned
    if (status[0]) {
        status[0] = 0;
        return true;
    } else {
        return false;
    }
}

// 0 damage, 1 stun, 2 weak, 3 poisonned
int Hero::getAttack(int attack, int effect[4]) {
    // gathering stats
    // 0 raw dmg, 1 armor, 2 dodge chance, 3 crit chance, 4 stun chance percent, 5 weak applied, 6 poison applied
    int weaponStats[7];
    weapon->getStats(attack, weaponStats);
    // 0 str, 1 vit, 2 crit chance, 3 dodge chance, 4 defence
    int helmetStats[5];
    helm->getStat(helmetStats);
    // 0 str, 1 vit, 2 crit chance, 3 dodge chance, 4 defence
    int shoesStats[5];
    boots->getStat(shoesStats);
    // raw dmg
    int dmg = weaponStats[0];
    if (dmg!=0) {
        // increase by stats
        dmg += strenght + helmetStats[0] + shoesStats[0];
        // doubled by crit chance
        int randint = QRandomGenerator::global()->bounded(100);
        if (randint<(clarity + weaponStats[3] + helmetStats[2] + shoesStats[2])) dmg = dmg*2;
        // weakness if applicable
        if (status[1]) dmg = (int)dmg*0.75;
    }
    effect[0] = dmg;
    // armor
    armor += weaponStats[1];
    // additional dodge chance (for when attacked during enemy's turn
    additionalDodgeChance = weaponStats[2];
    // stati applied
    // stun
    int randint = QRandomGenerator::global()->bounded(100);
    if (randint<weaponStats[4]) effect[1] = 1;
    else effect[1] = 0;
    //poison and weak
    effect[2] = weaponStats[5];
    effect[3] = weaponStats[6];
    return weaponStats[1];
}

// 0 damage, 1 stun, 2 weak, 3 poisonned
bool Hero::receiveAttack(int effect[4]) {

    // gathering stats
    // 0 str, 1 vit, 2 crit chance, 3 dodge chance, 4 defence
    int helmetStats[5];
    helm->getStat(helmetStats);
    // 0 str, 1 vit, 2 crit chance, 3 dodge chance, 4 defence
    int shoesStats[5];
    boots->getStat(shoesStats);
    // dodge chance
    int randint = QRandomGenerator::global()->bounded(100);
    if (randint < (agility + helmetStats[2] + shoesStats[2])) {effect[0] = 0; effect[1] = 0; effect[2] = 0; effect[3] = 0; return true;}
    // weak effect
    if (status[1]) effect[0] = (int)effect[0]*1.25;
    // defense effect
    effect[0] = (int) effect[0] * (100 - helmetStats[4] - shoesStats[4]) / 100;
    // taking dmg
    bool result = this->changeHp(effect[0]);
    // applying status
    status[0] = effect[1];
    status[1] += effect[2];
    status[2] += effect[3];
    return result;
}

PotionType Hero::usePotion(int slot) {
    PotionType pot = potions[slot]->getPotion();
    // if there is a potion, return it and create a new empty one in its place
    if (pot!=NoPotion) {
        delete potions[slot];
        potions[slot] = new Potion(NoPotion);
    }
    return pot;
}

bool Hero::endOfTurn(int &poisonDmg) {
    // taking damage equal to poison
    poisonDmg = status[2];
    this->changeHp(status[2]);
    // losing one stack of weak if applicable
    if(status[1]) status[1]--;
    // alive test
    if (hp<=0) return false;
    else return true;
}


void Hero::endOfCombat() {
    // remove armor & status
    armor = 0;
    status[0] = 0;
    status[1] = 0;
    status[2] = 0;
    // heal?
    changeHp(0); // update visuals
};

// inventory managment ------------------------------------------------------------------------------------------------------
bool Hero::changeGold(int amount) {
    if (amount<0 && gold<=-amount) {
        return false; // not enough gold to purchase
    } else {
        gold += amount;
        goldText->setText(QString::number(gold));
        return true;
    }
}

bool Hero::hasKey(bool isBossKey) {
    if (isBossKey) return bossKey;
    else return keys;
}

bool Hero::changeKey(bool getKey) { // can be false or true for use or get
    if (getKey) {
        keys++;
        keyText->setText(QString::number(keys)); // update visuals
    } else {
        if (keys==0) return false; // can't use one if you don't have one
        keys--;
        keyText->setText(QString::number(keys)); // update visuals
    }
    return true;
}

bool Hero::changeBossKey(bool getKey) {// can be false or true for use or get
    if (getKey) {
        bossKey=true;
        bossKeyPixmap->show();
    } else {
        if (!bossKey) return false; // cant use it if you don't have it
        bossKey=false;
        bossKeyPixmap->hide();
    }
    return true;
}

// other
void Hero::refreshEquipmentImg() {
    for (int i=0; i<3; i++) {
        potionsImg[i]->setPixmap(potions[i]->img.scaled(20,20));
    }
    equipmentImg[0]->setPixmap(weapon->img.scaled(20,20));
    equipmentImg[1]->setPixmap(helm->img.scaled(20,20));
    equipmentImg[2]->setPixmap(boots->img.scaled(20,20));
}

void Hero::resize() {
    // will move all graphics
    characterSheet->setPos(700,50);
}
