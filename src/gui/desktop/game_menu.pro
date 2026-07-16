QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../brick_game/snake/s21_snake_model.cc \
    controller/s21_snake_controller.cc \
    controller/s21_tetris_controller.cc \
    ../../brick_game/tetris/tetris.c \
    s21_help_field.cc \
    main.cc \
    view.cc

HEADERS += \
    ../../brick_game/snake/s21_snake_model.h \
    controller/s21_snake_controller.h \
    controller/s21_tetris_controller.h \
    ../../brick_game/tetris/tetris.h \
    s21_help_field.h \
    view.h

FORMS += \
    view.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
