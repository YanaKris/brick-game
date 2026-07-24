#ifndef S21_GUI_WEB_WEB_BRIDGE_H_
#define S21_GUI_WEB_WEB_BRIDGE_H_

#include "../../interface.h"

void WebFlattenField(int** field, int* out);

#ifdef __cplusplus
extern "C" {
#endif

void web_start(int game);
void web_input(int action, int hold);
void web_tick(void);

int* web_field_ptr(void);
int* web_next_ptr(void);

int web_score(void);
int web_high_score(void);
int web_level(void);
int web_speed(void);
int web_pause(void);

void web_set_high_score(int value);

#ifdef __cplusplus
}
#endif

#endif  // S21_GUI_WEB_WEB_BRIDGE_H_
