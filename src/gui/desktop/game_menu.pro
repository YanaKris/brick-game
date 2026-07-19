QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../brick_game/snake/s21_snake_model.cc \
    ../../brick_game/snake/snake_game.cc \
    ../../brick_game/tetris/tetris_model.cc \
    ../../brick_game/tetris/tetromino.cc \
    ../../brick_game/tetris/tetris_game.cc \
    ../../brick_game/common/high_score.cc \
    ../../brick_game/common/game_factory.cc \
    game_presenter.cc \
    snake_view.cc \
    tetris_view.cc \
    controller/s21_snake_controller.cc \
    controller/s21_tetris_controller.cc \
    main.cc \
    view.cc

HEADERS += \
    ../../brick_game/snake/s21_snake_model.h \
    ../../brick_game/snake/snake_game.h \
    ../../brick_game/tetris/tetris_model.h \
    ../../brick_game/tetris/tetromino.h \
    ../../brick_game/tetris/tetris_game.h \
    ../../brick_game/common/high_score.h \
    ../../brick_game/common/game_factory.h \
    game_presenter.h \
    snake_view.h \
    tetris_view.h \
    controller/s21_snake_controller.h \
    controller/s21_tetris_controller.h \
    view.h

FORMS += \
    view.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
