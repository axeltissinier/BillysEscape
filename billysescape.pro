QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    enemy.cpp \
    gameconsole.cpp \
    gamescene.cpp \
    gamewindow.cpp \
    graphicsobjectbutton.cpp \
    hero.cpp \
    main.cpp \
    map.cpp \
    room.cpp \
    startmenuscene.cpp \
    textbutton.cpp \
    wordlelock.cpp

HEADERS += \
    abstractscene.h \
    armor.h \
    consoletextedit.h \
    displayableitem.h \
    enemy.h \
    gameconsole.h \
    gamescene.h \
    gamewindow.h \
    graphicsobjectbutton.h \
    hero.h \
    map.h \
    potion.h \
    rarityenum.h \
    room.h \
    shop.h \
    startmenuscene.h \
    textbutton.h \
    weapon.h \
    wordlelock.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ressources.qrc

RC_ICONS += gameicon.ico
