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
HIGH_SCORE_CC = brick_game/common/high_score.cc
FACTORY_CC = brick_game/common/game_factory.cc
COMMON_CC = $(HIGH_SCORE_CC) $(FACTORY_CC)
TICK_TIMER_CC = gui/cli/tick_timer.cc
CLI_CC   = gui/cli/main.cc gui/cli/cli_view.cc gui/cli/cli_controller.cc \
	gui/cli/cli_input.cc $(TICK_TIMER_CC)
API_CC   = api/game_api.cc
WEB_BRIDGE_CC = gui/web/web_bridge.cc
# Desktop MVP core: Qt-free presenter (unit-tested without Qt).
DESKTOP_CORE_CC = gui/desktop/game_presenter.cc
BIN      = brickgame

# files checked/formatted by style/format
FORMAT_FILES = interface.h \
	api/*.h api/*.cc \
	brick_game/common/*.h brick_game/common/*.cc \
	brick_game/snake/*.cc brick_game/snake/*.h \
	brick_game/tetris/*.c brick_game/tetris/*.cc brick_game/tetris/*.h \
	gui/cli/*.cc gui/cli/*.h \
	gui/web/*.cc gui/web/*.h \
	gui/desktop/*.cc gui/desktop/*.h \
	gui/desktop/controller/*.cc gui/desktop/controller/*.h \
	tests/*.cc

.PHONY: all cli run web install uninstall clean dvi dist \
        test test_snake test_common test_tetris test_api test_desktop test_web \
        gcov_report style format valgrind

# Default: build only, do NOT run.
all: cli

# Build the unified CLI binary (menu -> Tetris + Snake). Build only.
# Pure C++: thin controller drives both games through the IGame facade,
# so the legacy tetris.c front-end is no longer linked here.
cli: clean
	$(CXX) $(CXXFLAGS) $(CLI_CC) $(SNAKE_CC) $(TETRIS_CC) $(COMMON_CC) \
		-o $(BIN) $(CURSES)

# Launch the CLI (separate from the build step).
run: cli
	./$(BIN)

install:
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(CLI_CC) $(SNAKE_CC) $(TETRIS_CC) $(COMMON_CC) \
		-o build/$(BIN) $(CURSES)
	echo 0 > build/high_score_snake.txt
	echo 0 > build/record.txt

uninstall:
	rm -rf build/

# --- web (WebAssembly via Emscripten) ---
# Требует emcc (emsdk) в PATH — локально может отсутствовать, сборку делает CI.
# Ядро игры + мост -> gui/web/brickgame.{js,wasm} рядом с index.html/web_view.js;
# всё это раздаётся как статика GitHub Pages.
EMCC         = emcc
WEB_DIR      = gui/web
WEB_SRC      = $(WEB_BRIDGE_CC) $(API_CC) $(FACTORY_CC) $(SNAKE_CC) $(TETRIS_CC)
EMCC_EXPORTS = _web_start,_web_input,_web_tick,_web_field_ptr,_web_next_ptr,_web_score,_web_high_score,_web_level,_web_speed,_web_pause,_web_finished,_web_state,_web_set_high_score

web:
	$(EMCC) -std=c++17 -O2 $(WEB_SRC) \
		-sMODULARIZE=1 -sEXPORT_NAME=BrickGameModule \
		-sEXPORTED_FUNCTIONS=$(EMCC_EXPORTS) \
		-sEXPORTED_RUNTIME_METHODS=ccall,cwrap,getValue \
		-sALLOW_MEMORY_GROWTH=1 -sENVIRONMENT=web \
		-o $(WEB_DIR)/brickgame.js

# --- tests (GoogleTest) ---
test: test_snake test_common test_tetris test_api test_desktop test_web

# Snake tests: model (pure logic) + SnakeGame facade/FSM. No ncurses linked.
test_snake:
	$(CXX) $(CXXFLAGS) tests/tests.cc tests/test_snake_fsm.cc $(SNAKE_CC) \
		$(LDGTEST) -o test_snake
	./test_snake

# Common-layer unit tests (FSM, Observer, IGame, HighScore, TickTimer).
test_common:
	$(CXX) $(CXXFLAGS) tests/test_common.cc $(HIGH_SCORE_CC) $(TICK_TIMER_CC) \
		$(LDGTEST) -o test_common
	./test_common

# Tetris: parity tests — legacy tetris.c (characterization) + s21::TetrisModel.
# Legacy C code is kept and compiled alongside until parity is confirmed.
test_tetris:
	$(CC) $(CFLAGS) -c $(TETRIS_C) -o tetris_legacy.o
	$(CXX) $(CXXFLAGS) tests/test_tetris.cc $(TETRIS_CC) tetris_legacy.o \
		$(LDGTEST) -o test_tetris
	rm -f tetris_legacy.o
	./test_tetris

test_api:
	$(CXX) $(CXXFLAGS) tests/test_game_api.cc $(API_CC) $(FACTORY_CC) \
		$(SNAKE_CC) $(TETRIS_CC) $(LDGTEST) -o test_api
	./test_api

# Desktop MVP presenter tests: Qt-free core over IGame. No Qt/qmake needed.
test_desktop:
	$(CXX) $(CXXFLAGS) tests/test_desktop_presenter.cc $(DESKTOP_CORE_CC) \
		$(SNAKE_CC) $(HIGH_SCORE_CC) $(LDGTEST) -o test_desktop
	./test_desktop

# Web bridge tests: int** field -> flat buffer + record passthrough over the
# public API. Native build (no Emscripten) — pure logic behind the WASM view.
test_web:
	$(CXX) $(CXXFLAGS) tests/test_web_bridge.cc $(WEB_BRIDGE_CC) $(API_CC) \
		$(FACTORY_CC) $(SNAKE_CC) $(TETRIS_CC) $(LDGTEST) -o test_web
	./test_web

# Coverage over the unit-tested translation units only (not the ncurses/Qt front-ends).
gcov_report:
	$(CC) $(CFLAGS) $(GCOV_FLAGS) -c $(TETRIS_C) -o tetris_legacy.o
	$(CXX) $(CXXFLAGS) $(GCOV_FLAGS) tests/tests.cc tests/test_snake_fsm.cc \
		tests/test_common.cc tests/test_tetris.cc tests/test_game_api.cc \
		tests/test_desktop_presenter.cc tests/test_web_bridge.cc \
		$(SNAKE_CC) $(TETRIS_CC) $(API_CC) $(COMMON_CC) $(TICK_TIMER_CC) \
		$(DESKTOP_CORE_CC) $(WEB_BRIDGE_CC) tetris_legacy.o $(LDGTEST) -o gcov_test
	./gcov_test
	lcov --capture --directory . --output-file coverage.info --no-external \
		--ignore-errors inconsistent,mismatch
	lcov --remove coverage.info '/usr/*' '*/tests/*' '*gtest*' \
		--output-file coverage.info --ignore-errors inconsistent,unused
	genhtml coverage.info --output-directory report --ignore-errors inconsistent
	@echo "Coverage report generated: report/index.html"

valgrind: test_snake test_common test_tetris test_api test_desktop test_web
	valgrind --leak-check=full --error-exitcode=1 ./test_snake
	valgrind --leak-check=full --error-exitcode=1 ./test_common
	valgrind --leak-check=full --error-exitcode=1 ./test_tetris
	valgrind --leak-check=full --error-exitcode=1 ./test_api
	valgrind --leak-check=full --error-exitcode=1 ./test_desktop
	valgrind --leak-check=full --error-exitcode=1 ./test_web

# --- code style (Google, config in src/.clang-format) ---
style:
	clang-format -style=$(STYLE) -n --Werror $(FORMAT_FILES)

format:
	clang-format -style=$(STYLE) -i $(FORMAT_FILES)

# --- documentation ---
dvi:
	open description.html 2>/dev/null || xdg-open description.html 2>/dev/null || echo "See description.html"

# --- source distribution ---
dist:
	mkdir -p dist
	cp -a brick_game gui api interface.h Makefile .clang-format description.html dist/
	tar -czf brickgame-2.0.tar.gz -C dist .
	rm -rf dist

clean:
	rm -rf *.o *.info *.gcda *.gcno *.gcov report/ build/ dist/
	rm -f $(BIN) main tetris snake test test_snake test_common test_tetris \
		test_api test_desktop test_web testing gcov_test
	rm -f *.tar.gz
	rm -f $(WEB_DIR)/brickgame.js $(WEB_DIR)/brickgame.wasm
	rm -rf *.dSYM
