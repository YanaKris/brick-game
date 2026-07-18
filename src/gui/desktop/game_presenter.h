#ifndef S21_GUI_DESKTOP_GAME_PRESENTER_H_
#define S21_GUI_DESKTOP_GAME_PRESENTER_H_

#include <memory>
#include <string>

#include "../../brick_game/common/game_base.h"
#include "../../interface.h"

namespace s21 {

// MVP-Presenter: Qt-free оркестратор над IGame. Владеет игрой и рекордом,
// прокидывает ввод, продвигает игру по запросу контроллера и накладывает
// (а при завершении — сохраняет) рекорд. View получает готовый GameInfo_t
// и только рисует — вся логика тестируется без Qt.
class GamePresenter {
 public:
  GamePresenter(std::unique_ptr<IGame> game, std::string high_score_path);

  // Загрузить рекорд из файла и перевести игру в рабочее состояние.
  void Begin();

  // Продвинуть игру на один шаг; вернуть кадр с наложенным рекордом.
  // После завершения игры — no-op, возвращает последний кадр.
  GameInfo_t Tick();

  // Прокинуть ввод в игру; вернуть актуальный кадр (для немедленной
  // перерисовки). После завершения игры ввод игнорируется.
  GameInfo_t Input(UserAction_t action);

  // Последний известный кадр без продвижения игры.
  GameInfo_t Snapshot() const;

  bool Finished() const;
  GameState State() const;
  int record() const { return record_; }

 private:
  GameInfo_t Advance();  // updateCurrentState + наложение рекорда

  std::unique_ptr<IGame> game_;
  std::string high_score_path_;
  int record_ = 0;
  GameInfo_t last_info_{};
  bool saved_ = false;  // рекорд сохраняется однократно при завершении
};

}  // namespace s21

#endif  // S21_GUI_DESKTOP_GAME_PRESENTER_H_
