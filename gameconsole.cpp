#include "gameconsole.h"
#include "consoletextedit.h"
#include "wordlelock.h"
#include <QGraphicsRectItem>

#include <QTimer>

GameConsole::GameConsole(QGraphicsScene* scene, Map* globalMap, Hero* globalHero) {
    // copying local pointers
    map = globalMap;
    hero = globalHero;
    currentRoom = map->getCurrentRoom();
    gameScene = scene;

    // log initialisation
    logArea = new QScrollArea();
    logArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    logArea->setWidgetResizable( true );
    // logArea->setAlignment(Qt::AlignBottom); // make widget stick to bottom, doesn't work
    int x = windowSize.width();
    int y = windowSize.height();
    logArea->setGeometry( 20, 20, x*0.2, y*0.8 );
    logArea->setStyleSheet("border:none; background-color:transparent;");
    // setting log text holder
    logText = new QLabel();
    logText->setWordWrap(true);
    logText->setAlignment(Qt::AlignJustify); // justify text
    logText->setText(QString("You wake up in a cell, with no memory of how you got there. The only thing that you remember is your name: Billy, and how to use a sword.\nYou should try to look around to find a way to leave this place.\n"));
    logText->setStyleSheet("background-color:transparent;");
    // log scroll bar initialisation
    customScrollBar = new QScrollBar();
    customScrollBar->setStyleSheet("QScrollBar:vertical {border: none; background-color: transparent; width: 15px;}"
                       "QScrollBar::handle:vertical {background: white; border: none; min-height: 20px;}"
                       "QScrollBar::add-line:vertical {border: none; background: none; height: 0px;subcontrol-position: bottom;subcontrol-origin: margin;}"
                       "QScrollBar::sub-line:vertical {border: none; background: none; height: 0px;subcontrol-position: top;subcontrol-origin: margin;}"
                       "QScrollBar::add-page:vertical{background-color: transparent;}"
                       "QScrollBar::sub-page:vertical{background-color: transparent;}");
    logArea->setVerticalScrollBar(customScrollBar);
    // adding log
    logArea->setWidget(logText);
    scene->addWidget(logArea);

    // entry initialisation
    consoleTextEntry = new ConsoleTextEdit();
    consoleTextEntry->setPlaceholderText(QString("Write here")); // placeholder text (in gray)
    consoleTextEntry->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    consoleTextEntry->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // consoleTextEntry->setCurrentCharFormat(QTextCharFormat); // sets text format (font, size, etc)
    consoleTextEntry->setStyleSheet("border:none; background-color: rgba(180,180,180,0%)"); // style of rectangle containing the text
    //consoleTextEntry->setStyleSheet("border:none; background: none");
    consoleTextEntry->setGeometry(20, y*0.85, x*0.2, 30);
    consoleTextEntry->grabKeyboard(); // grab all keypresses for wordle? Could grab everything at all time (for esc key)
    scene->addWidget(consoleTextEntry);
    // connect to signal emitted when enter pressed
    QObject::connect(consoleTextEntry, SIGNAL(enterPressed()), this, SLOT(processText()));

    // wordle lock
    wordlelock = new WordleLock(scene);
    QObject::connect(consoleTextEntry, SIGNAL(wordleChanged()), this, SLOT(wordleChangeText()));
    (wordlelock->lockBg)->hide(); // hiding it to start

    // level up of the hero
    QObject::connect(hero, SIGNAL(leveledUp()), this, SLOT(heroHasLeveledUp()));

    // action bar
    actionBarBg = new QGraphicsRectItem(0,0,10,10);
    actionBarBg->setBrush(Qt::NoBrush);
    actionBarBg->setPen(Qt::NoPen);
    actionBarText = new QGraphicsSimpleTextItem(actionBarBg);
    actionBarText->setText(QString("You wake up in a cell, with no memory of how you got there.\nThe only thing that you remember is your name: Billy, and how to use a sword.\nYou should try to look around to find a way to leave this place.\n"));
    actionBarText->setPos(10,10);
    QGraphicsSimpleTextItem* dummyText = new QGraphicsSimpleTextItem("Look around");
    GraphicsObjectButton* dummyButton = new GraphicsObjectButton(dummyText->boundingRect().width()+10,dummyText->boundingRect().height()+10,5,actionBarBg);
    buttons.push_back(dummyButton);
    QObject::connect(buttons[0], &GraphicsObjectButton::clicked, [this]{buttons[0]->hide();actionBarText->setText("You find nothing but a mechanism is locking the cell door...\nMaybe you can try to crack it open..."); this->processText("Look around");});
    dummyText->setParentItem(dummyButton);
    dummyText->setPos(5,5);
    dummyButton->setPos(100,100);
    actionBarBg->setPos(x*0.30,y*0.75);
    scene->addItem(actionBarBg);

}

// destructor (deleting all privates)
GameConsole::~GameConsole() {
    delete logArea; // also deleting logText
    delete customScrollBar;
    delete consoleTextEntry;
    if (wordlelock!=nullptr) delete wordlelock;
    if (currentItem!=nullptr) delete currentItem;
    if (currentEnemy!=nullptr) delete currentEnemy;
}

void GameConsole::processText(QString actionBarInput) {
    // get current text
    QString text;
    if (actionBarInput=="") {
        text = consoleTextEntry->toPlainText();
        text = text.toLower();
    } else text = actionBarInput.toLower();
    // special case if in wordle mode
    if (consoleTextEntry->wordling) {
        // cheat code
        if (text=="sudo") {logText->setText(logText->text() + wordlelock->chosenWord + '\n'); return;}
        // tests for entered world
        qInfo() << "testing if word is allowed";
        if (wordlelock->isTextAllowed(text)) { // this is an accepted word
            // add text
            logText->setText(logText->text() +"> " + text + '\n');
            // clear entered text
            consoleTextEntry->clear();
            QString wordState;
            qInfo() << "testing if word is good";
            int lockState = wordlelock->enterText(text, wordState);
            qInfo() << "lock state" << lockState;
            if (lockState!=0) { // this is the correct or we've reached the maximum number of lines
                consoleTextEntry->wordling=false;
                if (lockState==2) {
                    logText->setText(logText->text() +"You found the password!\nYou must now find a way to leave this dungeon...\n");
                    delete wordlelock;
                    wordlelock = nullptr;
                    //wordlelock->lockBg->hide();
                    configuration = 1; // change to exploration mode
                    actionBarText->setText("You found the password!\nYou must now find a way to leave this dungeon...\n\nThis Bar is a WIP, and is not finshed/makes the game crash so it is disabled here.");
                    //this->changeActionBarConfig(1);
                    QTimer::singleShot(20, customScrollBar, [this]{customScrollBar->setValue(customScrollBar->maximum());});
                    return;
                } else {
                    logText->setText(logText->text() +"The lock stopped responding...\n");
                }
            } else { // this is not the correct word
                logText->setText(logText->text() +"  " + wordState + '\n');
            }
        } else { // this is not an accepted word
            logText->setText(logText->text() +"This word doesn't exist!\n");
        }

    } else {
        qInfo() << "configuration" << configuration << "entered" << text;
        // add text
        logText->setText(logText->text() +"> " + text + '\n');
        // clear entered text
        consoleTextEntry->clear();
        QStringList list = text.split(QRegExp("\\s"));

        // out of configuration commands
        if (list[0]=="upgrade" || list[0]=="improve") {
            if (hero->canLevelUp()) {
                if (list.size()>1) {
                    if (list[1]=="str" || list[1]=="strength") hero->levelUp(0);
                    else if (list[1]=="vit" || list[1]=="vitality") hero->levelUp(0);
                    else if (list[1]=="agi" || list[1]=="agility") hero->levelUp(0);
                    else if (list[1]=="cla" || list[1]=="clarity") hero->levelUp(0);
                    else logText->setText(logText->text() +"You need to specify what to upgrade! [str, vit, agi, cla]\n");
                } else logText->setText(logText->text() +"You need to specify what to upgrade! [str, vit, agi, cla]\n");
            } else logText->setText(logText->text() +"You don't have level up points to spend!\n");
            QTimer::singleShot(20, customScrollBar, [this]{customScrollBar->setValue(customScrollBar->maximum());});
            return;
        }

        // cheat command (to make it easier, in case it wasn't already)
        if (list[0]=="sudo" && list.size()>1) {
            if (list[1]=="displaymap") map->displayMap();
            if (list[1]=="getkey") hero->changeKey(true);
            if (list[1]=="getgold") hero->changeGold(100);
            if (list[1]=="heal") hero->changeHp(-100);
            QTimer::singleShot(20, customScrollBar, [this]{customScrollBar->setValue(customScrollBar->maximum());});
            return;
        }


        // processing text depending on configuration
        if (configuration==0) { // start of the game, will trigger worlde lock to appear
            if ((list[0]=="inspect" || list[0]=="look" || list[0]=="search" || list[0]=="escape" || list[0]=="explore") && !((wordlelock->lockBg)->isVisible())) {
                logText->setText(logText->text() +"You look around you cell and find nothing but a strange lock. You attempt to unlock it.\n");
                consoleTextEntry->wordling=true;
                (wordlelock->lockBg)->show(); // showing lock
                buttons[0]->hide();
            }
            else if ((list[0]=="try" || list[0]=="retry" || list[0]=="reset") && (wordlelock->currentLine == 6)) {
                wordlelock->newWord();
                consoleTextEntry->wordling=true;
            }
            else {logText->setText(logText->text() +"You might want to look around first.\n");}



        // in a room --------------------------------------------------------------------------------------------------------------------------
        } else if (configuration==1) {
            // moving to another room --------------------------------------------------------------------------------------------------
            if (list[0]=="go" || list[0]=="move" || list[0]=="head") {
                if (list.size()>1) {
                    int result = -1;
                    if (list[1]=="north" || list[1]=="up" || list[1]=="n" || list[1]=="top") result = map->goToRoom(N);
                    else if (list[1]=="east" || list[1]=="right" || list[1]=="e") result = map->goToRoom(E);
                    else if (list[1]=="south" || list[1]=="down" || list[1]=="s" || list[1]=="bottom") result = map->goToRoom(S);
                    else if (list[1]=="west" || list[1]=="left" || list[1]=="w") result = map->goToRoom(W);
                    // testing results
                    if (!result) { // went to new room
                        currentRoom = map->getCurrentRoom();
                        if (currentRoom->hasChest || currentRoom->hasBossKeyChest) {
                            logText->setText(logText->text() +"You enter a new room. A chest is standing in the middle of it.\n");
                        } else if (currentRoom->hasEnemy) {
                            logText->setText(logText->text() +"You enter a new room, but as soon as you enter, a monster attacks you!\n");
                            // creating enemy
                            if (!currentRoom->isBossRoom) currentEnemy = new Enemy("random", (int)hero->isLevel()/2);
                            else currentEnemy = new Enemy("Boss");
                            //display the enemy
                            currentEnemy->display(gameScene);
                            // change to combat configuration
                            currentRoom->hasEnemy = false;
                            configuration = 2;
                            //this->changeActionBarConfig(2);
                            QTimer::singleShot(20, customScrollBar, [this]{customScrollBar->setValue(customScrollBar->maximum());});
                            return;

                        } else if (currentRoom->isShop) {
                            logText->setText(logText->text() +"You enter a new room, a shadowy figure seems to be selling various items here.\n");
                        } else {
                            logText->setText(logText->text() +"You enter a new room.\n");
                        }
                        // need to refresh the scene to make the new room appear
                        //this->changeActionBarConfig(1);
                        emit refreshScene();

                    } else if (result==1) logText->setText(logText->text() +"You tried walking through a wall, it didn't work.\n");
                    else if (result==2) logText->setText(logText->text() +"This door is locked, you need to unlock it first!\n");
                    else if (result==3) logText->setText(logText->text() +"This door has a special lock, you need to unlock it first with a special key!\n");
                    else logText->setText(logText->text() +"The direction you choose was not clear! [North, East, South, West]\n");
                } else {
                    logText->setText(logText->text() +"You need to choose a direction to go to! [North, East, South, West]\n");
                }
            }
            // when searching rooms -----------------------------------------------------------------------------------------
            else if (list[0]=="search" || list[0]=="inspect" || list[0]=="look" || list[0]=="examine") {
                QString txt = "";
                if (currentRoom->gold) {
                    hero->changeGold(currentRoom->gold);
                    txt += QString("You search the room and find %1 gold!\n").arg(currentRoom->gold);
                    currentRoom->gold = 0;
                }
                if (currentRoom->hasKey) {
                    if (txt!="") txt += "You also";
                    else txt += "You";
                    txt += " find a rusty key, maybe it can be used to open some doors?\n";
                    hero->changeKey(true);
                    currentRoom->hasKey = false;
                }
                if (currentRoom->hasMap) {
                    if (txt!="") txt += "You also";
                    else txt += "You";
                    txt += " find a map of the dungeon!\n";
                    currentRoom->hasMap = false;
                    map->displayMap();
                    emit refreshScene();
                }
                if (currentRoom->hasPotion) {
                    // generating new potion
                    PotionType pottype;
                    int randint = QRandomGenerator::global()->bounded(5);
                    if (randint==0) pottype = Heal;
                    else if (randint==1) pottype = Damage;
                    else if (randint==2) pottype = Stun;
                    else if (randint==3) pottype = Poison;
                    else pottype = Weakness;
                    currentItem = new Potion(pottype);
                    // text
                    if (txt!="") txt += "You also";
                    else txt += "You";
                    txt += " find a " + currentItem->getName() + "! Do you want to equip it?\n";
                    logText->setText(logText->text() + txt);
                    currentRoom->hasPotion = false;
                    // changing to reward configuration
                    configuration = 3;
                    //this->changeActionBarConfig(3);
                    QTimer::singleShot(20, customScrollBar, [this]{customScrollBar->setValue(customScrollBar->maximum());});
                    return;
                }
                if (txt=="") logText->setText(logText->text() +"You search every inch of the room but find nothing of value.\n");
                else logText->setText(logText->text() + txt);
            }
            // for rooms with chests -------------------------------------------------------------------------
            else if (list[0]=="open") {
                if (currentRoom->hasChest) {
                    // generate random (rare) item
                    Rarity r;
                    int randint = QRandomGenerator::global()->bounded(100);
                    if (randint>=75)  r = Rare;
                    else r = Uncommon;
                    // equipment
                    randint = QRandomGenerator::global()->bounded(3);
                    if (randint==0) { // weapon
                        int randint2 = QRandomGenerator::global()->bounded(4); // type
                        if (randint2==0) {
                            currentItem = new Weapon(SwordAndShield, r);
                        } else if (randint2==1) {
                            currentItem = new Weapon(BattleAxe, r);
                        } else if (randint2==2) {
                            currentItem = new Weapon(Dagger, r);
                        } else {
                            currentItem = new Weapon(Bow, r);
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
                            currentItem = new Armor(piece, Heavy, r);
                        } else if (randint2==1) {
                            currentItem = new Armor(piece, Light, r);
                        } else {
                            currentItem = new Armor(piece, Rogue, r);
                        }
                    }
                    logText->setText(logText->text() +"You find a " + currentItem->getName() + "! Do you want to equip it?\n");
                    currentRoom->hasChest = false;
                    // changing to reward configuration
                    configuration = 3;
                    //this->changeActionBarConfig(3);
                    QTimer::singleShot(20, customScrollBar, [this]{customScrollBar->setValue(customScrollBar->maximum());});
                    return;
                } else if (currentRoom->hasBossKeyChest) {
                    logText->setText(logText->text() +"You find the Boss Room Key! You can now go unlock the Boss door.\n");
                    currentRoom->hasBossKeyChest = false;
                    hero->changeBossKey(true);
                // nothing to do
                } else logText->setText(logText->text() +"There is no chest to open in the room.\n");
            }


            // for items in the room ------------------------------------------------------------------------------------------------------
            else if (list[0]=="item" || list[0]=="items" || list[0]=="list") {
                int listsize = (currentRoom->itemList).size();
                if (listsize==0) {
                    logText->setText(logText->text() +"There is no item lying in the room.\n");
                } else {
                    logText->setText(logText->text() +"The following items are lying in the room:\n");
                    for (int i = 0; i<listsize; i++) {
                        logText->setText(logText->text() + QString("%1- ").arg(i+1) + (currentRoom->itemList[i])->getName() + '\n');
                    }
                }
            }
            else if (list[0]=="take" || list[0]=="equip" || list[0]=="grab") {
                if (list.size()<2) logText->setText(logText->text() + "You must specify which item you wish to take. [number]\n");
                else {
                    // item chosen
                    int slot = list[1].toInt();
                    // additionnal arg for potion when necessary
                    int potslot = -1;
                    if (list.size()>2) potslot = list[2].toInt();
                    // checking if number entered is ok
                    int listsize = (currentRoom->itemList).size();
                    if (slot>0 && slot<=listsize) {
                        // swapping item
                        DisplayableItem* wantedItem = currentRoom->itemList[slot - 1];
                        QString oldName = wantedItem->getName();
                        bool completed = hero->changeGear(wantedItem, potslot);
                        // checking completion
                        if (completed) {
                            // remove old item from the room
                            currentRoom->itemList.erase(currentRoom->itemList.begin() + slot - 1);
                            // put new item in the room if applicable (not a nullptr)
                            if (wantedItem!=nullptr) {
                                currentRoom->itemList.push_back(wantedItem);
                                logText->setText(logText->text() + "You dropped a " + wantedItem->getName() + " and equiped a " + oldName + "!\n");
                            } else logText->setText(logText->text() + "You equiped a " + oldName + "!\n");

                        } else { // potion in wrong spot or no available spot
                            logText->setText(logText->text() + "Your potion bag is full and you need to specify which one to replace. [1,2,3]\n");
                        }
                    } else logText->setText(logText->text() + "You must input a valid number of the items list.\n");
                }
            }
            // for unlocking locked door ---------------------------------------------------------------------------------------------
            else if (list[0]=="unlock" || (list.size()>1 && list[0]=="use" && list[1]=="key")) {
                // adjusting for additionnal size
                if (list[0]=="use") {
                    if (list.size()>2) list[1] = list[2];
                    else list[1]="";
                }
                // normal door loop
                if (hero->hasKey() || hero->hasKey(true)) {
                    int nbRoomLocked=0;
                    if (currentRoom->north==DoorLocked || currentRoom->north==BossDoor) nbRoomLocked++;
                    if (currentRoom->east==DoorLocked || currentRoom->east==BossDoor) nbRoomLocked++;
                    if (currentRoom->south==DoorLocked || currentRoom->south==BossDoor) nbRoomLocked++;
                    if (currentRoom->west==DoorLocked || currentRoom->west==BossDoor) nbRoomLocked++;
                    if (nbRoomLocked==1) {
                        if (currentRoom->north==DoorLocked && hero->hasKey()) {
                            currentRoom->north=DoorOpen;
                            logText->setText(logText->text() +"You unlocked the North door.\n");
                            hero->changeKey(false);
                            emit refreshScene();
                        }
                        else if (currentRoom->east==DoorLocked && hero->hasKey()) {
                            currentRoom->east=DoorOpen;
                            logText->setText(logText->text() +"You unlocked the East door.\n");
                            hero->changeKey(false);
                            emit refreshScene();
                        }
                        else if (currentRoom->south==DoorLocked && hero->hasKey()) {
                            currentRoom->south=DoorOpen;
                            logText->setText(logText->text() +"You unlocked the South door.\n");
                            hero->changeKey(false);
                            emit refreshScene();
                        }
                        else if (currentRoom->west==DoorLocked && hero->hasKey()){
                            currentRoom->west=DoorOpen;
                            logText->setText(logText->text() +"You unlocked the West door.\n");
                            hero->changeKey(false);
                            emit refreshScene();
                        }
                        else if (currentRoom->north==BossDoor && hero->hasKey(true)) {
                            currentRoom->north=DoorOpen;
                            logText->setText(logText->text() +"You unlocked the door to the Boss.\n");
                            hero->changeBossKey(false);
                            emit refreshScene();
                        }
                        else if (currentRoom->east==BossDoor && hero->hasKey(true)) {
                            currentRoom->east=DoorOpen;
                            logText->setText(logText->text() +"You unlocked the door to the Boss.\n");
                            hero->changeBossKey(false);
                            emit refreshScene();
                        }
                        else if (currentRoom->south==BossDoor && hero->hasKey(true)) {
                            currentRoom->south=DoorOpen;
                            logText->setText(logText->text() +"You unlocked the door to the Boss.\n");
                            hero->changeBossKey(false);
                            emit refreshScene();
                        }
                        else if (currentRoom->west==BossDoor && hero->hasKey(true)){
                            currentRoom->west=DoorOpen;
                            logText->setText(logText->text() +"You unlocked the door to the Boss.\n");
                            hero->changeBossKey(false);
                            emit refreshScene();
                        } else logText->setText(logText->text() +"You don't have the corresponding key!\n");

                    } else if (nbRoomLocked>1) {
                        if (list.size()>1) {
                            if (hero->hasKey() && currentRoom->north==DoorLocked && (list[1]=="north" || list[1]=="up" || list[1]=="n")) {
                                currentRoom->north=DoorOpen;
                                logText->setText(logText->text() +"You unlocked the North door.\n");
                                hero->changeKey(false);
                                emit refreshScene();
                            } else if (hero->hasKey() && currentRoom->east==DoorLocked && (list[1]=="east" || list[1]=="right" || list[1]=="e")) {
                                currentRoom->east=DoorOpen;
                                logText->setText(logText->text() +"You unlocked the East door.\n");
                                hero->changeKey(false);
                                emit refreshScene();
                            } else if (hero->hasKey() && currentRoom->south==DoorLocked && (list[1]=="south" || list[1]=="down" || list[1]=="s")) {
                                currentRoom->south=DoorOpen;
                                logText->setText(logText->text() +"You unlocked the South door.\n");
                                hero->changeKey(false);
                                emit refreshScene();
                            } else if (hero->hasKey() && currentRoom->west==DoorLocked && (list[1]=="west" || list[1]=="left" || list[1]=="w")) {
                                currentRoom->west=DoorOpen;
                                logText->setText(logText->text() +"You unlocked the West door.\n");
                                hero->changeKey(false);
                                emit refreshScene();
                            } else if (hero->hasKey(true) && currentRoom->north==BossDoor && (list[1]=="north" || list[1]=="up" || list[1]=="n")) {
                                currentRoom->north=DoorOpen;
                                logText->setText(logText->text() +"You unlocked the North door.\n");
                                hero->changeKey(false);
                                emit refreshScene();
                            } else if (hero->hasKey(true) && currentRoom->east==BossDoor && (list[1]=="east" || list[1]=="right" || list[1]=="e")) {
                                currentRoom->east=DoorOpen;
                                logText->setText(logText->text() +"You unlocked the East door.\n");
                                hero->changeKey(false);
                                emit refreshScene();
                            } else if (hero->hasKey(true) && currentRoom->south==BossDoor && (list[1]=="south" || list[1]=="down" || list[1]=="s")) {
                                currentRoom->south=DoorOpen;
                                logText->setText(logText->text() +"You unlocked the South door.\n");
                                hero->changeKey(false);
                                emit refreshScene();
                            } else if (hero->hasKey(true) && currentRoom->west==BossDoor && (list[1]=="west" || list[1]=="left" || list[1]=="w")) {
                                currentRoom->west=DoorOpen;
                                logText->setText(logText->text() +"You unlocked the West door.\n");
                                hero->changeKey(false);
                                emit refreshScene();
                            } else logText->setText(logText->text() +"You must specify a direction, or don't have the corresponding key! [North, East, South, West]\n");
                        } else logText->setText(logText->text() +"You must specify a direction! [North, East, South, West]\n");
                    } else logText->setText(logText->text() +"There is no room to unlock!\n");
                } else logText->setText(logText->text() +"You don't have any keys!\n");
            }

            // shop --------------------------------------------------------------------------------------------------------------------
            else if (list[0]=="shop" || list[0]=="buy") {
                // print some text and current shop items and prices
                logText->setText(logText->text() + "You stop a moment to look at what the strange man sells.\n\"Ha yes, take a look and see for yourself, I only sell high quality equipment!\"\n");
                DisplayableItem* itemsList[4];
                int itemsPrice[4] = {0};
                bool sellsKey = shop.getItems(itemsList, itemsPrice);
                for (int i=0; i<4; i++) {
                    if (itemsPrice[i]==0) logText->setText(logText->text() + QString("%1- Sold!\n").arg(i+1));
                    else logText->setText(logText->text() + QString("%1- ").arg(i+1) + itemsList[i]->getName() + " - " + QString::number(itemsPrice[i]) + " gold\n");
                }
                if  (sellsKey) logText->setText(logText->text() + "5- A rusty key - 80 gold\n");
                logText->setText(logText->text() + "Refresh shop - 40 gold\n");
                // changes to shop configuration
                configuration=4;
                //this->changeActionBarConfig(4);
                QTimer::singleShot(20, customScrollBar, [this]{customScrollBar->setValue(customScrollBar->maximum());});
                return;
            }
            else logText->setText(logText->text() + "Command was not understood. [info]\n");




        // in combat ------------------------------------------------------------------------------------------------------------------------------
        } else if (configuration==2) {

            qInfo() << "enemy hp" << currentEnemy->hp << "armor" << currentEnemy->armor;
            qInfo() << "enemy status" << currentEnemy->status[0] << currentEnemy->status[1] << currentEnemy->status[2];


            // integer that states the outcome of the combat 0 not finished 1 player win 2 player lose
            int combatResult = 0;
            // remove use if you spell "use potion 1" to reduce it to "potion 1"; same with attacks
            if (list.size()>1 && list[0]=="use") list.removeFirst();
            if (list[0]=="attack" || list[0]=="strike" || list[0]=="move" || list[0]=="power") {
                if (list.size()>1) {
                    int index = list[1].toInt();
                    if (index==1 || index==2 || index==3) {
                        // main combat code ----------------------------------------------------------------------------------
                        int attack[4];
                        // player turn
                        int armorgain = hero->getAttack(index-1, attack);
                        bool combatState = currentEnemy->attacked(attack);
                        // log
                        if (attack[0]!=0) logText->setText(logText->text() + QString("You attack the enemy for %1 damage.\n").arg(attack[0]));
                        if (armorgain!=0) logText->setText(logText->text() + QString("You gain %1 armor.\n").arg(armorgain));
                        if (attack[1]!=0) logText->setText(logText->text() + QString("You stun the enemy.\n"));
                        if (attack[2]!=0) logText->setText(logText->text() + QString("You weaken the enemy for %1 turns.\n").arg(attack[2]));
                        if (attack[3]!=0) logText->setText(logText->text() + QString("You poison the enemy for %1 damage per turn.\n").arg(attack[3]));
                        // living test
                        if (combatState) combatResult = 1; // enemy dies from attack
                        else {
                            bool stunned = true; // to start the loop
                            while (stunned) {
                                // resetting stunned
                                stunned = false;
                                // end of player turn
                                int poisondmg;
                                combatState = hero->endOfTurn(poisondmg);
                                if (poisondmg!=0) logText->setText(logText->text() + QString("You take %1 damage from poison!\n").arg(poisondmg));
                                if (!combatState) combatResult = 2; // player dies from poison
                                else {
                                    // enemy turn
                                    armorgain = currentEnemy->getAttack(attack);
                                    if (armorgain==0 && attack[0]==0 && attack[1]==0 && attack[2]==0 && attack[3]==0) {
                                        logText->setText(logText->text() + "The enemy is stunned and can't attack!\n");
                                        combatState = true;
                                    } else {
                                        int enemyAttack[4] = {attack[0], attack[1], attack[2], attack[3]};
                                        combatState = hero->receiveAttack(attack);
                                        if (attack[0]!=enemyAttack[0] && attack[1]!=enemyAttack[1] && attack[2]!=enemyAttack[2] && attack[3]!=enemyAttack[3]) {
                                            logText->setText(logText->text() + "You masterfully dodge the enemy attack!\n");
                                        } else {
                                            if (attack[0]!=0) logText->setText(logText->text() + QString("The enemy attacks you for %1 damage.\n").arg(attack[0]));
                                            if (armorgain!=0) logText->setText(logText->text() + QString("The enemy gains %1 armor.\n").arg(armorgain));
                                            if (attack[1]!=0) logText->setText(logText->text() + QString("The enemy stuns you.\n"));
                                            if (attack[2]!=0) logText->setText(logText->text() + QString("The enemy weakens you for %1 turns.\n").arg(attack[2]));
                                            if (attack[3]!=0) logText->setText(logText->text() + QString("The enemy poisons you for %1 damage per turn.\n").arg(attack[3]));
                                        }
                                    }
                                    if (!combatState) combatResult = 2; // player dies from attack
                                    else {
                                        // end of enemy turn
                                        combatState = currentEnemy->endOfTurn(poisondmg);
                                        if (poisondmg!=0) logText->setText(logText->text() + QString("The enemy takes %1 damage from poison!\n").arg(poisondmg));
                                        if (combatState) combatResult = 1; // enemy dies from poison
                                        else {
                                            // start of the player turn; test if he is stunned, in which case the enemy gets a free new attack
                                            stunned = hero->startOfTurn();
                                            if (stunned) logText->setText(logText->text() + QString("You are stunned and can't attack!\n"));
                                        }
                                    }
                                }
                            }
                        }
                    } else logText->setText(logText->text() + "You must specify which attack to perform! [1,2,3]\n");
                } else logText->setText(logText->text() + "You must specify which attack to perform! [1,2,3]\n");
            } else if (list[0]=="potion" || list[0]=="pot") {
                if (list.size()>1) {
                    int index = list[1].toInt();
                    if (index==1 || index==2 || index==3) {
                        // getting potion
                        PotionType pot = hero->usePotion(index-1);
                        // applying corresponding effect
                        if (pot==Heal) {
                            hero->changeHp(-50);
                            logText->setText(logText->text() + "You use a Healing Potion and heal 50 HP.\n");
                        } else if (pot==Damage) {
                            int poteffect[] = {20,0,0,0};
                            if (currentEnemy->attacked(poteffect, false)) combatResult = 1;
                            logText->setText(logText->text() + "You use a Damage Potion and deal 20 damage.\n");
                        } else if (pot==Stun) {
                            int poteffect[] = {0,1,0,0};
                            currentEnemy->attacked(poteffect, false);
                            logText->setText(logText->text() + "You use a Stun Potion and stun the enemy for their next turn.\n");
                        } else if (pot==Weakness) {
                            int poteffect[] = {0,0,2,0};
                            currentEnemy->attacked(poteffect, false);
                            logText->setText(logText->text() + "You use a Weakness Potion and weaken the enemy for their next two turn.\n");
                        } else if (pot==Poison) {
                            int poteffect[] = {0,0,0,5};
                            currentEnemy->attacked(poteffect, false);
                            logText->setText(logText->text() + "You use a Poison Potion and give 5 stack of poison to the enemy.\n");
                        } else logText->setText(logText->text() + "There is no potion in this slot!\n");

                    } else logText->setText(logText->text() + "You must specify which potion to use! [1,2,3]\n");
                } else logText->setText(logText->text() + "You must specify which potion to use! [1,2,3]\n");
            } else logText->setText(logText->text() + "You can't escape and must fight! Use your attacks and potions to defeat your foe!\n");


            // testing result of action - combat reward -----------------------------------------------------------------------------
            if (combatResult==2) { // game over
                logText->setText(logText->text() + "You faint from your wounds.\n\nGAME OVER\n\nGo back to main menu or close the game?\n");
                configuration=5;
                //this->changeActionBarConfig(5);
            } else if (combatResult==1) {
                hero->endOfCombat();
                if (currentRoom->isBossRoom) { // game over
                    logText->setText(logText->text() + "The Boss is dead, you can finally escape.\n\nVICTORY\n\nGo back to main menu or close the game?\n");
                    configuration=5;
                    //this->changeActionBarConfig(5);
                    delete currentEnemy;
                    currentEnemy = nullptr;
                    QTimer::singleShot(20, customScrollBar, [this]{customScrollBar->setValue(customScrollBar->maximum());});
                    return;
                } else {
                    // normal unit: gold, exp, drops an item
                    int goldreward;
                    int expreward;
                    currentEnemy->getReward(goldreward, expreward);
                    hero->changeGold(goldreward);
                    hero->changeExp(expreward);
                    logText->setText(logText->text() + QString("The creature is dead. You find %1 gold coins on its corpse. You gain %2 experience points from the fight.\n").arg(goldreward).arg(expreward));
                    // random item
                    Rarity r;
                    int randint = QRandomGenerator::global()->bounded(100);
                    if (randint>=90)  r = Rare;
                    else if (randint>=60) r = Uncommon;
                    else r = Common;
                    // equipment
                    randint = QRandomGenerator::global()->bounded(3);
                    if (randint==0) { // weapon
                        int randint2 = QRandomGenerator::global()->bounded(4); // type
                        if (randint2==0) {
                            currentItem = new Weapon(SwordAndShield, r);
                        } else if (randint2==1) {
                            currentItem = new Weapon(BattleAxe, r);
                        } else if (randint2==2) {
                            currentItem = new Weapon(Dagger, r);
                        } else {
                            currentItem = new Weapon(Bow, r);
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
                            currentItem = new Armor(piece, Heavy, r);
                        } else if (randint2==1) {
                            currentItem = new Armor(piece, Light, r);
                        } else {
                            currentItem = new Armor(piece, Rogue, r);
                        }
                    }
                    logText->setText(logText->text() +"The creature also drops a " + currentItem->getName() + "! Do you want to equip it?\n");
                    configuration = 3;
                    //this->changeActionBarConfig(3);
                    delete currentEnemy;
                    currentEnemy = nullptr;
                    emit refreshScene();
                    QTimer::singleShot(20, customScrollBar, [this]{customScrollBar->setValue(customScrollBar->maximum());});
                    return;
                }
            }


        // reward equip/take choice -----------------------------------------------------------------------------------------------------------------
        } else if (configuration==3) {
            if (list[0]=="yes" || list[0]=="y" || list[0]=="take" || list[0]=="equip") {
                int potslot = -1;
                if (list.size()>1) potslot = list[1].toInt();
                QString oldName = currentItem->getName();
                bool completed = hero->changeGear(currentItem, potslot);
                // checking completion
                if (completed) {
                    // put the old item in the room if applicable (not a nullptr)
                    if (currentItem!=nullptr) {
                        currentRoom->itemList.push_back(currentItem);
                        logText->setText(logText->text() + "You dropped " + currentItem->getName() + " and equiped " + oldName + "!\n");
                    } else logText->setText(logText->text() + "You equiped " + oldName + "!\n");
                    configuration = 1;
                    //this->changeActionBarConfig(1);
                    QTimer::singleShot(20, customScrollBar, [this]{customScrollBar->setValue(customScrollBar->maximum());});
                    return;
                } else { // potion in wrong spot or no available spot
                    logText->setText(logText->text() + "Your potion bag is full and you need to specify which one to replace. [1,2,3]\n");
                }
            } else if (list[0]=="no" || list[0]=="n" || list[0]=="leave" || list[0]=="discard") {
                // put the item in the room
                currentRoom->itemList.push_back(currentItem);
                logText->setText(logText->text() +"You drop the " + currentItem->getName() + " on the ground. You may come back and take it later.\n");
                currentItem = nullptr;
                configuration = 1;
                //this->changeActionBarConfig(1);
                QTimer::singleShot(20, customScrollBar, [this]{customScrollBar->setValue(customScrollBar->maximum());});
                return;
            } else logText->setText(logText->text() +"Do you wish to take the item? [y,n]\n");




        // shop interface
        } else if (configuration==4) {
            // list, buy, refresh, leave
            if (list[0]=="list" || list[0]=="item" || list[0]=="items" || list[0]=="goods" ) {
                logText->setText(logText->text() + "\"Here is a list of my goods up for sale:\"\n");
                DisplayableItem* itemsList[4];
                int itemsPrice[4] = {0};
                bool sellsKey = shop.getItems(itemsList, itemsPrice);
                for (int i=0; i<4; i++) {
                    if (itemsPrice[i]==0) logText->setText(logText->text() + QString("%1- Sold!\n").arg(i+1));
                    else logText->setText(logText->text() + QString("%1- ").arg(i+1) + itemsList[i]->getName() + " - " + QString::number(itemsPrice[i]) + " gold\n");
                }
                if  (sellsKey) logText->setText(logText->text() + "5- A rusty key - 80 gold\n");
                logText->setText(logText->text() + "Refresh shop - 40 gold\n");


            } else if (list[0]=="buy") {
                if (list.size()>1) {
                    int itemChosen = list[1].toInt();
                    if (itemChosen>0 && itemChosen<6) {
                        DisplayableItem* itemsList[4];
                        int itemsPrice[4] = {0};
                        bool sellsKey = shop.getItems(itemsList, itemsPrice);
                        if (itemChosen==5) { // key
                            if (sellsKey) {
                                if (hero->changeGold(-80)) {
                                    shop.buyItem(4);
                                    hero->changeKey(true);
                                    logText->setText(logText->text() + "\"That key can surely open the door of your dreams...\"\n");
                                } else logText->setText(logText->text() + "\"You don't have enough money for that precious key!\"\n");
                            } else logText->setText(logText->text() + "\"I have no more keys to sell...\"\n");
                        } else if (itemsPrice[itemChosen]==0) {
                            logText->setText(logText->text() + "\"This item is already sold...\"\n");

                        } else if (itemChosen==1 && hero->changeGold(-itemsPrice[0])) {
                            DisplayableItem* itemBought = shop.buyItem(0);
                            logText->setText(logText->text() + "\"This " + itemBought->getName() + " surely was worth the price...\"\n");
                            hero->changeGear(itemBought); // changing gear
                            if (itemBought!=nullptr) currentRoom->itemList.push_back(itemBought); // putting old item in the room

                        } else if (itemChosen==2 && hero->changeGold(-itemsPrice[1])) {
                            DisplayableItem* itemBought = shop.buyItem(1);
                            logText->setText(logText->text() + "\"This " + itemBought->getName() + " surely was worth the price...\"\n");
                            hero->changeGear(itemBought); // changing gear
                            if (itemBought!=nullptr) currentRoom->itemList.push_back(itemBought); // putting old item in the room

                        } else if (itemChosen==3 && hero->changeGold(-itemsPrice[2])) {
                            DisplayableItem* itemBought = shop.buyItem(2);
                            logText->setText(logText->text() + "\"This " + itemBought->getName() + " surely was worth the price...\"\n");
                            hero->changeGear(itemBought); // changing gear
                            if (itemBought!=nullptr) currentRoom->itemList.push_back(itemBought); // putting old item in the room

                        } // this is a potion and a slot must be available or specified
                        else if (itemChosen==4 && hero->changeGold(-itemsPrice[3])) {
                            DisplayableItem* itemBought = shop.buyItem(3);
                            logText->setText(logText->text() + "\"This " + itemBought->getName() + " surely was worth the price...\"\n");
                            bool completed = hero->changeGear(itemBought); // changing gear
                            if (!completed) {
                                logText->setText(logText->text() + "\"Ha! You don't have space left for this potion... Not to worry, I'll put it in this room, and you can come back and take it whenever you want...\"\n");
                                currentRoom->itemList.push_back(itemBought);
                            }

                        } else logText->setText(logText->text() + "\"You don't have enough gold for that, come back when you can pay the price.\"\n");


                    } else logText->setText(logText->text() + "\"I didn't get which item you want to buy.\" [number]\n");
                } else logText->setText(logText->text() + "\"You need to tell me which item you want to buy.\" [number]\n");


            } else if (list[0]=="refresh" || list[0]=="reset") {
                if (hero->changeGold(-40)) {
                    shop.refreshShop();
                    logText->setText(logText->text() + "\"Wait a moment...\nHere, take a look at these:\"\n");
                    DisplayableItem* itemsList[4];
                    int itemsPrice[4] = {0};
                    bool sellsKey = shop.getItems(itemsList, itemsPrice);
                    for (int i=0; i<4; i++) {
                        if (itemsPrice[i]==0) logText->setText(logText->text() + QString("%1- Sold!\n").arg(i+1));
                        else logText->setText(logText->text() + QString("%1- ").arg(i+1) + itemsList[i]->getName() + " - " + QString::number(itemsPrice[i]) + " gold\n");
                    }
                    if (sellsKey) logText->setText(logText->text() + "5- A rusty key - 80 gold\n");
                    logText->setText(logText->text() + "Refresh shop - 40 gold\n");
                } else logText->setText(logText->text() + "\"If you want me to bring new goods, you must be able to pay the price...\"\n");


            } else if (list[0]=="leave" || list[0]=="turn" || list[0]=="return" || list[0]=="back") {
                logText->setText(logText->text() + "You decide to leave the merchant.\n");
                configuration = 1;
                //this->changeActionBarConfig(1);
                QTimer::singleShot(20, customScrollBar, [this]{customScrollBar->setValue(customScrollBar->maximum());});
                return;
            } else logText->setText(logText->text() +"\"Don't touch anything! You either buy something or leave... I can also refresh my items if you're willing to pay the price...\"\n");




        // victory/defeat screen ---------------------------------------------------------------------------------------------------------------
        } else if (configuration==5) {
            if (list[0]=="go" || list[0]=="back" || list[0]=="start" || list[0]=="menu") {
                emit goBackToStartMenu();
            }
            else if (list[0]=="close" || list[0]=="end" || list[0]=="quit") {
                emit closeGame();
            }
        }
    }
    // set the scrollbar so it is always at the bottom, displaying text just entered
    // timer is needed because we need to wait for the widget to take in account the fact that it has been changed
    // update() and repaint() on the scroll area and text display don't work, 10 ms make it so you don't see the two state change
    QTimer::singleShot(20, customScrollBar, [this]{customScrollBar->setValue(customScrollBar->maximum());});
}

// work in progress, makes the game crash
void GameConsole::changeActionBarConfig(int config, int subConfig) {
    // swapping new and old vectors of buttons
    buttons.swap(previousButtons);
    // hiding previous buttons
    int sizeButtonVect = previousButtons.size();
    qInfo() << "hiding" << sizeButtonVect;
    for (int i = 0; i<sizeButtonVect; i++) {
        previousButtons[i]->hide();
    }
    // removing old buttons
    sizeButtonVect = buttons.size();
    qInfo() << "deleting" << sizeButtonVect;
    for (int i = 0; i<sizeButtonVect; i++) {
        if (buttons[i]!=nullptr) delete buttons[i];
    }
    buttons.clear();

    qInfo() << "actions depending on config choice";
    // in a room
    if (config==1) {
        // main menu
        if (subConfig==0) {
            // move
            QGraphicsSimpleTextItem* dummyText = new QGraphicsSimpleTextItem("Move");
            GraphicsObjectButton* dummyButton = new GraphicsObjectButton(dummyText->boundingRect().width()+10,dummyText->boundingRect().height()+10,5,actionBarBg);
            buttons.push_back(dummyButton);
            QObject::connect(dummyButton, &GraphicsObjectButton::clicked, [this]{this->changeActionBarConfig(1,1);});
            dummyText->setParentItem(dummyButton);
            dummyText->setPos(5,5);
            dummyButton->setPos(20,100);
            /* makes the game crash sometimes -- deactivated
            // inspect
            dummyText = new QGraphicsSimpleTextItem("Search");
            dummyButton = new GraphicsObjectButton(dummyText->boundingRect().width()+10,dummyText->boundingRect().height()+10,5,actionBarBg);
            buttons.push_back(dummyButton);
            QObject::connect(buttons[1], &GraphicsObjectButton::clicked, [this]{buttons[1]->hide(); if (currentRoom->hasPotion) this->changeActionBarConfig(3); this->processText("Search the room for loot"); });
            dummyText->setParentItem(dummyButton);
            dummyText->setPos(5,5);
            dummyButton->setPos(80,100);
            if (!(currentRoom->gold || currentRoom->hasKey || currentRoom->hasMap || currentRoom->hasPotion)) buttons[1]->hide();

            // items
            dummyText = new QGraphicsSimpleTextItem("Items");
            dummyButton = new GraphicsObjectButton(dummyText->boundingRect().width()+10,dummyText->boundingRect().height()+10,5,actionBarBg);
            buttons.push_back(dummyButton);
            QObject::connect(buttons[2], &GraphicsObjectButton::clicked, [this]{this->changeActionBarConfig(1,2);});
            dummyText->setParentItem(dummyButton);
            dummyText->setPos(5,5);
            dummyButton->setPos(140,100);
            if (currentRoom->itemList.size()<1) buttons[2]->hide();
            */
            // shop
            dummyText = new QGraphicsSimpleTextItem("Shop");
            dummyButton = new GraphicsObjectButton(dummyText->boundingRect().width()+10,dummyText->boundingRect().height()+10,5, actionBarBg);
            buttons.push_back(dummyButton);
            QObject::connect(buttons[3], &GraphicsObjectButton::clicked, [this]{this->processText("Buy equipment");});
            dummyText->setParentItem(dummyButton);
            dummyText->setPos(5,5);
            dummyButton->setPos(200,100);
            if (!currentRoom->isShop) buttons[3]->hide();

            // chest
            dummyText = new QGraphicsSimpleTextItem("Open chest");
            dummyButton = new GraphicsObjectButton(dummyText->boundingRect().width()+10,dummyText->boundingRect().height()+10,5, actionBarBg);
            buttons.push_back(dummyButton);
            QObject::connect(buttons[4], &GraphicsObjectButton::clicked, [this]{this->processText("Open the chest");});
            dummyText->setParentItem(dummyButton);
            dummyText->setPos(5,5);
            dummyButton->setPos(200,100);
            if (!(currentRoom->hasChest || currentRoom->hasBossKeyChest)) buttons[4]->hide();

        // moving direction menu
        } else if (subConfig==1) {
             actionBarText->setText("Where do you want to go?");
             QGraphicsSimpleTextItem*  dummyText = new QGraphicsSimpleTextItem("Go North");
             if (currentRoom->north==DoorLocked) dummyText->setText("Use a key and go North");
             if (currentRoom->north==BossDoor) dummyText->setText("Fight Big Bobby");
             GraphicsObjectButton* dummyButton = new GraphicsObjectButton(dummyText->boundingRect().width()+10,dummyText->boundingRect().height()+10,5,actionBarBg);
             buttons.push_back(dummyButton);
             QObject::connect(buttons[0], &GraphicsObjectButton::clicked, [this]{
                 if (currentRoom->north==DoorLocked || currentRoom->north==BossDoor) processText("Unlock North");
                 processText("Move North");
             });
             dummyText->setParentItem(dummyButton);
             dummyText->setPos(5,5);
             dummyButton->setPos(75,50);
             if (!(currentRoom->north==DoorOpen || (currentRoom->north==DoorLocked && hero->hasKey()) || (currentRoom->north==BossDoor && hero->hasKey(true)))) buttons[0]->hide();

             dummyText = new QGraphicsSimpleTextItem("Go East");
             if (currentRoom->east==DoorLocked) dummyText->setText("Use a key and go East");
             if (currentRoom->east==BossDoor) dummyText->setText("Fight Big Bobby");
             dummyButton = new GraphicsObjectButton(dummyText->boundingRect().width()+10,dummyText->boundingRect().height()+10,5,actionBarBg);
             buttons.push_back(dummyButton);
             QObject::connect(buttons[1], &GraphicsObjectButton::clicked, [this]{
                 if (currentRoom->east==DoorLocked || currentRoom->east==BossDoor) processText("Unlock East");
                 processText("Move East");
             });
             dummyText->setParentItem(dummyButton);
             dummyText->setPos(5,5);
             dummyButton->setPos(100,80);
             if (!(currentRoom->east==DoorOpen || (currentRoom->east==DoorLocked && hero->hasKey()) || (currentRoom->east==BossDoor && hero->hasKey(true)))) buttons[1]->hide();

             dummyText = new QGraphicsSimpleTextItem("Go South");
             if (currentRoom->south==DoorLocked) dummyText->setText("Use a key and go South");
             if (currentRoom->south==BossDoor) dummyText->setText("Fight Big Bobby");
             dummyButton = new GraphicsObjectButton(dummyText->boundingRect().width()+10,dummyText->boundingRect().height()+10,5,actionBarBg);
             buttons.push_back(dummyButton);
             QObject::connect(buttons[2], &GraphicsObjectButton::clicked, [this]{
                 if (currentRoom->south==DoorLocked || currentRoom->south==BossDoor) processText("Unlock South");
                 processText("Move South");
             });
             dummyText->setParentItem(dummyButton);
             dummyText->setPos(5,5);
             dummyButton->setPos(75,100);
             if (!(currentRoom->south==DoorOpen || (currentRoom->south==DoorLocked && hero->hasKey()) || (currentRoom->south==BossDoor && hero->hasKey(true)))) buttons[2]->hide();

             dummyText = new QGraphicsSimpleTextItem("Go West");
             if (currentRoom->west==DoorLocked) dummyText->setText("Use a key and go West");
             if (currentRoom->west==BossDoor) dummyText->setText("Fight Big Bobby");
             dummyButton = new GraphicsObjectButton(dummyText->boundingRect().width()+10,dummyText->boundingRect().height()+10,5,actionBarBg);
             buttons.push_back(dummyButton);
             QObject::connect(buttons[3], &GraphicsObjectButton::clicked, [this]{
                 if (currentRoom->west==DoorLocked || currentRoom->west==BossDoor) processText("Unlock West");
                 processText("Move West");
             });
             dummyText->setParentItem(dummyButton);
             dummyText->setPos(5,5);
             dummyButton->setPos(20,75);
             if (!(currentRoom->west==DoorOpen || (currentRoom->west==DoorLocked && hero->hasKey()) || (currentRoom->west==BossDoor && hero->hasKey(true)))) buttons[3]->hide();

             // back button
             dummyText = new QGraphicsSimpleTextItem("Back");
             dummyButton = new GraphicsObjectButton(dummyText->boundingRect().width()+10,dummyText->boundingRect().height()+10,5, actionBarBg);
             buttons.push_back(dummyButton);
             QObject::connect(buttons[4], &GraphicsObjectButton::clicked, [this]{this->changeActionBarConfig(1);});
             dummyText->setParentItem(dummyButton);
             dummyText->setPos(5,5);
             dummyButton->setPos(200, 100);


        // take an item of the room
        } else if (subConfig==2) {
            actionBarText->setText("The following items are lying in the room; Which one do you wish to equip?");
            additionalText = ""; // resetting the text for special occasion

            int nbItems = currentRoom->itemList.size();
            // there is a maximum of two items in a room and a minimum of one if this function is called
            QString name = currentRoom->itemList[0]->getName();
            QGraphicsSimpleTextItem*  dummyText = new QGraphicsSimpleTextItem(name);
            GraphicsObjectButton* dummyButton = new GraphicsObjectButton(dummyText->boundingRect().width()+10,dummyText->boundingRect().height()+10,5,actionBarBg);
            buttons.push_back(dummyButton);

            QObject::connect(buttons[0], &GraphicsObjectButton::clicked, [this]{
                DisplayableItem* wantedItem = currentRoom->itemList[0];
                QString oldName = wantedItem->getName();
                bool completed = hero->changeGear(wantedItem);
                // checking completion
                if (completed) {
                    // remove old item from the room (just the pointer, not the item)
                    currentRoom->itemList.erase(currentRoom->itemList.begin());
                    // put new item in the room if applicable (not a nullptr)
                    if (wantedItem!=nullptr) {
                        currentRoom->itemList.push_back(wantedItem);
                        actionBarText->setText("You dropped a " + wantedItem->getName() + " and equiped a " + oldName + "!");
                    } else actionBarText->setText("You equiped a " + oldName + "!");
                    // going back to main room menu
                    this->changeActionBarConfig(1);

                } else { // potion in wrong spot or no available spot
                    actionBarText->setText("Your potion bag is full; which one you want to replace?");
                    additionalText = "Equip 1 ";
                    this->changeActionBarConfig(1,3);
                }
            });
            dummyText->setParentItem(dummyButton);
            dummyText->setPos(5,5);
            dummyButton->setPos(10,50);

            if (nbItems==2) {
                name = currentRoom->itemList[1]->getName();
                dummyText = new QGraphicsSimpleTextItem(name);
                dummyButton = new GraphicsObjectButton(dummyText->boundingRect().width()+10,dummyText->boundingRect().height()+10,5,actionBarBg);
                buttons.push_back(dummyButton);

                QObject::connect(buttons[1], &GraphicsObjectButton::clicked, [this]{
                    DisplayableItem* wantedItem = currentRoom->itemList[1];
                    QString oldName = wantedItem->getName();
                    bool completed = hero->changeGear(wantedItem);
                    // checking completion
                    if (completed) {
                        // remove old item from the room (just the pointer, not the item)
                        currentRoom->itemList.erase(currentRoom->itemList.begin()+1);
                        // put new item in the room if applicable (not a nullptr)
                        if (wantedItem!=nullptr) {
                            currentRoom->itemList.push_back(wantedItem);
                            actionBarText->setText("You dropped a " + wantedItem->getName() + " and equiped a " + oldName + "!");
                        } else actionBarText->setText("You equiped a " + oldName + "!");
                        // going back to main room menu
                        this->changeActionBarConfig(1);

                    } else { // potion in wrong spot or no available spot
                        actionBarText->setText("Your potion bag is full; which one you want to replace?");
                        additionalText = "Equip 2 ";
                        this->changeActionBarConfig(1,3);
                    }
                });
                dummyText->setParentItem(dummyButton);
                dummyText->setPos(5,5);
                dummyButton->setPos(10,80);

                // back button
                dummyText = new QGraphicsSimpleTextItem("Back");
                dummyButton = new GraphicsObjectButton(dummyText->boundingRect().width()+10,dummyText->boundingRect().height()+10,5, actionBarBg);
                buttons.push_back(dummyButton);
                QObject::connect(buttons[2], &GraphicsObjectButton::clicked, [this]{this->changeActionBarConfig(1);});
                dummyText->setParentItem(dummyButton);
                dummyText->setPos(5,5);
                dummyButton->setPos(150, 60);

            } else {
                // back button
                dummyText = new QGraphicsSimpleTextItem("Back");
                dummyButton = new GraphicsObjectButton(dummyText->boundingRect().width()+10,dummyText->boundingRect().height()+10,5, actionBarBg);
                buttons.push_back(dummyButton);
                QObject::connect(buttons[1], &GraphicsObjectButton::clicked, [this]{this->changeActionBarConfig(1);});
                dummyText->setParentItem(dummyButton);
                dummyText->setPos(5,5);
                dummyButton->setPos(150, 60);
            }

        // potion choice with additional text
        } else if (subConfig==3) {
            QGraphicsSimpleTextItem* dummyText = new QGraphicsSimpleTextItem("1");
            GraphicsObjectButton* dummyButton = new GraphicsObjectButton(dummyText->boundingRect().width()+10,dummyText->boundingRect().height()+10,5, actionBarBg);
            buttons.push_back(dummyButton);
            QObject::connect(buttons[0], &GraphicsObjectButton::clicked, [this]{this->processText(additionalText+'1'); this->changeActionBarConfig(1);});
            dummyText->setParentItem(dummyButton);
            dummyText->setPos(5,5);
            dummyButton->setPos(20,100);

            dummyText = new QGraphicsSimpleTextItem("2");
            dummyButton = new GraphicsObjectButton(dummyText->boundingRect().width()+10,dummyText->boundingRect().height()+10,5, actionBarBg);
            buttons.push_back(dummyButton);
            QObject::connect(buttons[1], &GraphicsObjectButton::clicked, [this]{this->processText(additionalText+'2'); this->changeActionBarConfig(1);});
            dummyText->setParentItem(dummyButton);
            dummyText->setPos(5,5);
            dummyButton->setPos(50,100);

            dummyText = new QGraphicsSimpleTextItem("3");
            dummyButton = new GraphicsObjectButton(dummyText->boundingRect().width()+10,dummyText->boundingRect().height()+10,5, actionBarBg);
            buttons.push_back(dummyButton);
            QObject::connect(buttons[2], &GraphicsObjectButton::clicked, [this]{this->processText(additionalText+'3'); this->changeActionBarConfig(1);});
            dummyText->setParentItem(dummyButton);
            dummyText->setPos(5,5);
            dummyButton->setPos(80,100);

            // back button
            dummyText = new QGraphicsSimpleTextItem("Back");
            dummyButton = new GraphicsObjectButton(dummyText->boundingRect().width()+10,dummyText->boundingRect().height()+10,5, actionBarBg);
            buttons.push_back(dummyButton);
            QObject::connect(buttons[3], &GraphicsObjectButton::clicked, [this]{this->changeActionBarConfig(1);});
            dummyText->setParentItem(dummyButton);
            dummyText->setPos(5,5);
            dummyButton->setPos(110,100);

        } else logText->setText(logText->text() +"There has been an error.\n");
    }


    // in combat
    else if (config==2) {
        // main menu: attack or potion
        if (subConfig==0) {




        // attack choice
        } else if (subConfig==1) {



        // potion choice
        } else if (subConfig==2) {





        } else logText->setText(logText->text() +"There has been an error.\n");
    }


    // reward equipping
    else if (config==3) {
        // main menu
        if (subConfig==0) {




        // potion bag is full, choice of slot
        } else if (subConfig==1) {


        } else logText->setText(logText->text() +"There has been an error.\n");
    }


    // shopping
    else if (config==4) {
        // main shop
        if (subConfig==0) {




        // potion bag is full
        } else if (subConfig==1) {




        } else logText->setText(logText->text() +"There has been an error.\n");
    }

    // end screen
    else if (config==5) {



    } else logText->setText(logText->text() +"There has been an error.\n");
}

void GameConsole::resize() {
    // Resizing log area
    // int x = windowSize.width();
    int y = windowSize.height();
    logArea->setGeometry( 0, 0, 200, y*0.6 );
    customScrollBar->setValue(customScrollBar->maximum());
    // resizing console entry
    consoleTextEntry->setGeometry(0, y*0.6, 200, 30);
    // resize lock
}

void GameConsole::wordleChangeText() {
    QString text = consoleTextEntry->toPlainText();
    wordlelock->updateText(text.toLower());
}

void GameConsole::heroHasLeveledUp() {
    logText->setText(logText->text() + "You leveled up! You can use a point to boost your strength, vitality, agility or clarity! [upgrade stat]\n");
}
