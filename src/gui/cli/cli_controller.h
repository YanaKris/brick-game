#ifndef S21_GUI_CLI_CLI_CONTROLLER_H_
#define S21_GUI_CLI_CLI_CONTROLLER_H_

#include <memory>
#include <string>

#include "../../brick_game/common/game_base.h"
#include "cli_input.h"
#include "cli_view.h"
#include "tick_timer.h"

namespace s21 {

class CliController {
 public:
  CliController(std::unique_ptr<IGame> game, std::string high_score_path);
  void Run();

 private:
  bool WaitForStart();
  GameInfo_t Advance(); 
  void Finish(const GameInfo_t& info);

  std::unique_ptr<IGame> game_;
  std::string high_score_path_;
  CliView view_;
  CliInput input_;
  TickTimer timer_;
  int record_ = 0;
};

}  // namespace s21

#endif  // S21_GUI_CLI_CLI_CONTROLLER_H_
