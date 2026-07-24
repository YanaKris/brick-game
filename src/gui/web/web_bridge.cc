#include "web_bridge.h"

#include "../../api/game_api.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#define WEB_EXPORT EMSCRIPTEN_KEEPALIVE
#else
#define WEB_EXPORT
#endif

namespace {

int g_field[FIELD_HEIGHT * FIELD_WIDTH] = {0};
int g_next[FIELD_HEIGHT * FIELD_WIDTH] = {0};

int g_score = 0;
int g_level = 0;
int g_speed = 0;
int g_pause = 0;

int g_record = 0;

}  // namespace

void WebFlattenField(int** field, int* out) {
  for (int r = 0; r < FIELD_HEIGHT; ++r)
    for (int c = 0; c < FIELD_WIDTH; ++c)
      out[r * FIELD_WIDTH + c] = field[r][c];
}

extern "C" {

WEB_EXPORT void web_start(int game) {
  selectGame(static_cast<CurrentGame>(game));
}

WEB_EXPORT void web_input(int action, int hold) {
  userInput(static_cast<UserAction_t>(action), hold != 0);
}

WEB_EXPORT void web_tick(void) {
  GameInfo_t info = updateCurrentState();
  WebFlattenField(info.field, g_field);
  if (info.next != nullptr) WebFlattenField(info.next, g_next);
  g_score = info.score;
  if (info.score > g_record) g_record = info.score;
  g_level = info.level;
  g_speed = info.speed;
  g_pause = info.pause;
}

WEB_EXPORT int* web_field_ptr(void) { return g_field; }
WEB_EXPORT int* web_next_ptr(void) { return g_next; }

WEB_EXPORT int web_score(void) { return g_score; }
WEB_EXPORT int web_high_score(void) { return g_record; }
WEB_EXPORT int web_level(void) { return g_level; }
WEB_EXPORT int web_speed(void) { return g_speed; }
WEB_EXPORT int web_pause(void) { return g_pause; }

WEB_EXPORT void web_set_high_score(int value) { g_record = value; }

}  // extern "C"
