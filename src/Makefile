CC       = gcc
CXX      = g++
CFLAGS   = -Wall -Wextra -Werror -std=c11 -g
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -g
GCOV_FLAGS = --coverage
LDGTEST  = -lgtest -lgtest_main -lpthread

# --- platform-specific ncurses libraries ---
ifeq ($(shell uname), Linux)
	CURSES = -lncurses -ltinfo
endif
ifeq ($(shell uname), Darwin)
	CURSES = -lncurses
endif

# --- clang-format: config file lives next to this Makefile (src/.clang-format) ---
STYLE = file

# --- sources ---
TETRIS_C  = brick_game/tetris/tetris.c
TETRIS_CC = brick_game/tetris/tetris_model.cc brick_game/tetris/tetromino.cc \
	brick_game/tetris/tetris_game.cc
SNAKE_CC  = brick_game/snake/s21_snake_model.cc brick_game/snake/snake_game.cc
CLI_C    = gui/cli/s21_front.c
CLI_CC   = gui/cli/main.cc gui/cli/s21_game_view_cli.cc gui/cli/s21_snake_controller.cc
API_CC   = api/game_api.cc
BIN      = brickgame

# files checked/formatted by style/format
FORMAT_FILES = interface.h \
	api/*.h api/*.cc \
	brick_game/common/*.h \
	brick_game/snake/*.cc brick_game/snake/*.h \
	brick_game/tetris/*.c brick_game/tetris/*.cc brick_game/tetris/*.h \
	gui/cli/*.cc gui/cli/*.h gui/cli/*.c \
	gui/desktop/*.cc gui/desktop/*.h \
	gui/desktop/controller/*.cc gui/desktop/controller/*.h \
	tests/*.cc

.PHONY: all cli run install uninstall clean dvi dist \
        test test_snake test_common test_tetris test_api gcov_report style format \
        valgrind

all: cli

cli: clean
	$(CC) $(CFLAGS) -c $(TETRIS_C) -o tetris.o
	$(CC) $(CFLAGS) -c $(CLI_C) -o front.o
	$(CXX) $(CXXFLAGS) $(CLI_CC) $(SNAKE_CC) tetris.o front.o -o $(BIN) $(CURSES)
	rm -f tetris.o front.o

run: cli
	./$(BIN)

install:
	mkdir -p build
	$(CC) $(CFLAGS) -c $(TETRIS_C) -o build/tetris.o
	$(CC) $(CFLAGS) -c $(CLI_C) -o build/front.o
	$(CXX) $(CXXFLAGS) $(CLI_CC) $(SNAKE_CC) build/tetris.o build/front.o -o build/$(BIN) $(CURSES)
	rm -f build/tetris.o build/front.o
	echo 0 > build/high_score_snake.txt
	echo 0 > build/record.txt

uninstall:
	rm -rf build/

test: test_snake test_common test_tetris test_api

test_snake:
	$(CXX) $(CXXFLAGS) tests/tests.cc tests/test_snake_fsm.cc $(SNAKE_CC) \
		$(LDGTEST) -o test_snake
	./test_snake

test_common:
	$(CXX) $(CXXFLAGS) tests/test_common.cc $(LDGTEST) -o test_common
	./test_common

test_tetris:
	$(CC) $(CFLAGS) -c $(TETRIS_C) -o tetris_legacy.o
	$(CXX) $(CXXFLAGS) tests/test_tetris.cc $(TETRIS_CC) tetris_legacy.o \
		$(LDGTEST) -o test_tetris
	rm -f tetris_legacy.o
	./test_tetris

test_api:
	$(CXX) $(CXXFLAGS) tests/test_game_api.cc $(API_CC) $(SNAKE_CC) $(TETRIS_CC) \
		$(LDGTEST) -o test_api
	./test_api

gcov_report:
	$(CC) $(CFLAGS) $(GCOV_FLAGS) -c $(TETRIS_C) -o tetris_legacy.o
	$(CXX) $(CXXFLAGS) $(GCOV_FLAGS) tests/tests.cc tests/test_snake_fsm.cc \
		tests/test_common.cc tests/test_tetris.cc tests/test_game_api.cc \
		$(SNAKE_CC) $(TETRIS_CC) $(API_CC) \
		tetris_legacy.o $(LDGTEST) -o gcov_test
	./gcov_test
	lcov --capture --directory . --output-file coverage.info --no-external \
		--ignore-errors inconsistent,mismatch
	lcov --remove coverage.info '/usr/*' '*/tests/*' '*gtest*' \
		--output-file coverage.info --ignore-errors inconsistent,unused
	genhtml coverage.info --output-directory report --ignore-errors inconsistent
	@echo "Coverage report generated: report/index.html"

valgrind: test_snake
	valgrind --leak-check=full --error-exitcode=1 ./test_snake

style:
	clang-format -style=$(STYLE) -n --Werror $(FORMAT_FILES)

format:
	clang-format -style=$(STYLE) -i $(FORMAT_FILES)

dvi:
	open description.html 2>/dev/null || xdg-open description.html 2>/dev/null || echo "See description.html"

dist:
	mkdir -p dist
	cp -a brick_game gui api interface.h Makefile .clang-format description.html dist/
	tar -czf brickgame-2.0.tar.gz -C dist .
	rm -rf dist

clean:
	rm -rf *.o *.info *.gcda *.gcno *.gcov report/ build/ dist/
	rm -f $(BIN) main tetris snake test test_snake test_common test_tetris \
		test_api testing gcov_test
	rm -f *.tar.gz
	rm -rf *.dSYM
