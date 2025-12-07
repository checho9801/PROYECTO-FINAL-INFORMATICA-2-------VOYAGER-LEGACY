QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bala.cpp \
    balaenemiga.cpp \
    barraVida.cpp \
    collisionmanager.cpp \
    enemigo.cpp \
    gamemanager.cpp \
    gema.cpp \
    main.cpp \
    mainwindow.cpp \
    meteorito.cpp \
    nave.cpp \
    naveN3.cpp \
    nivel1.cpp \
    nivel2.cpp \
    nivel3.cpp \
    nivelbase.cpp \
    nivelmanager.cpp

HEADERS += \
    bala.h \
    balaenemiga.h \
    barraVida.h \
    collisionmanager.h \
    constants.h \
    enemigo.h \
    gamemanager.h \
    gema.h \
    mainwindow.h \
    meteorito.h \
    nave.h \
    naveN3.h \
    nivel1.h \
    nivel2.h \
    nivel3.h \
    nivelbase.h \
    nivelmanager.h

FORMS += \
    ../../../Downloads/Nivel_1/Nivel_1/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../../../Downloads/Nivel_1/Nivel_1/Space.png \
    ../../../Downloads/Nivel_1/Nivel_1/luna.png \
    ../../../Downloads/Nivel_1/Nivel_1/spriteMe.png \
    ../../../Downloads/Nivel_1/Nivel_1/voyager.png

RESOURCES += \
    resources.qrc
