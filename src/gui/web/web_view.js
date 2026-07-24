// Dumb web-View поверх WASM-моста (web_bridge). Логики игры здесь нет: цикл на
// requestAnimationFrame тикает мост по его же speed, клавиши уходят в web_input,
// поле читается из плоского буфера через Module.getValue и рисуется на canvas.
(function () {
  'use strict';

  var W = 10;
  var H = 20;
  var CELLS = W * H;
  var CELL = 24; // px

  var TETRIS = 0;
  var SNAKE = 1; // enum CurrentGame

  // enum UserAction_t (порядок из interface.h)
  var A = {
    Start: 0, Pause: 1, Terminate: 2,
    Left: 3, Right: 4, Up: 5, Down: 6, Action: 7
  };
  // enum GameState — из конечных нужны только эти
  var GAME_OVER = 6;
  var WIN = 7;

  // 0 — пусто, 1 — блок/тело, 2 — яблоко
  var COLORS = ['#0d1117', '#39d353', '#ff4d4d'];

  var canvas = document.getElementById('field');
  var ctx = canvas.getContext('2d');
  var elScore = document.getElementById('score');
  var elHigh = document.getElementById('high');
  var elLevel = document.getElementById('level');
  var elStatus = document.getElementById('status');

  var Mod = null;
  var api = null;
  var current = TETRIS;
  var running = false;
  var lastTs = 0;
  var raf = 0;

  function hiKey(game) {
    return game === SNAKE ? 'brickgame.hi.snake' : 'brickgame.hi.tetris';
  }
  function loadHi(game) {
    return parseInt(localStorage.getItem(hiKey(game)) || '0', 10) || 0;
  }
  function saveHi(game, value) {
    try { localStorage.setItem(hiKey(game), String(value)); } catch (e) { /* no-op */ }
  }

  function bindApi(Module) {
    Mod = Module;
    api = {
      start: Module.cwrap('web_start', null, ['number']),
      input: Module.cwrap('web_input', null, ['number', 'number']),
      tick: Module.cwrap('web_tick', null, []),
      fieldPtr: Module.cwrap('web_field_ptr', 'number', []),
      score: Module.cwrap('web_score', 'number', []),
      high: Module.cwrap('web_high_score', 'number', []),
      level: Module.cwrap('web_level', 'number', []),
      speed: Module.cwrap('web_speed', 'number', []),
      paused: Module.cwrap('web_pause', 'number', []),
      finished: Module.cwrap('web_finished', 'number', []),
      state: Module.cwrap('web_state', 'number', []),
      setHigh: Module.cwrap('web_set_high_score', null, ['number'])
    };
  }

  function label(game) {
    return game === SNAKE ? 'Змейка' : 'Тетрис';
  }

  function setStatus(text) {
    elStatus.textContent = text;
  }

  function draw() {
    var ptr = api.fieldPtr();
    for (var i = 0; i < CELLS; i++) {
      var v = Mod.getValue(ptr + i * 4, 'i32');
      var x = (i % W) * CELL;
      var y = ((i / W) | 0) * CELL;
      ctx.fillStyle = COLORS[v] || COLORS[1];
      ctx.fillRect(x + 1, y + 1, CELL - 2, CELL - 2);
    }
    elScore.textContent = api.score();
    elHigh.textContent = api.high();
    elLevel.textContent = api.level();
  }

  function persistHi() {
    var h = api.high();
    if (h > loadHi(current)) saveHi(current, h);
  }

  function stopLoop() {
    running = false;
    if (raf) { cancelAnimationFrame(raf); raf = 0; }
  }

  function loop(ts) {
    if (!running) return;
    if (!lastTs) lastTs = ts;
    var interval = api.speed();
    if (interval < 30) interval = 30; // защита от нулевой/крошечной задержки
    if (ts - lastTs >= interval) {
      lastTs = ts;
      api.tick();
      persistHi();
      if (api.finished()) {
        running = false;
        raf = 0;
        draw();
        setStatus(api.state() === WIN ? 'Победа! 🎉' : 'Игра окончена');
        return;
      }
    }
    draw();
    raf = requestAnimationFrame(loop);
  }

  function startGame(game) {
    stopLoop();
    current = game;
    api.start(game); // selectGame — чистое состояние
    api.setHigh(loadHi(game)); // рекорд из localStorage
    api.input(A.Start, 0);
    api.tick(); // первый кадр
    running = true;
    lastTs = 0;
    setStatus(label(game));
    raf = requestAnimationFrame(loop);
  }

  var KEYS = {
    ArrowLeft: A.Left, ArrowRight: A.Right, ArrowUp: A.Up, ArrowDown: A.Down,
    ' ': A.Action, Spacebar: A.Action, p: A.Pause, P: A.Pause,
    Escape: A.Terminate, q: A.Terminate
  };

  function onKey(e) {
    if (!api) return;
    if (e.key === 'Enter') { // старт/рестарт текущей игры
      e.preventDefault();
      startGame(current);
      return;
    }
    var action = KEYS[e.key];
    if (action === undefined) return;
    e.preventDefault();
    api.input(action, 0); // ввод не двигает игру — перерисуется на ближайшем тике
    if (action === A.Pause) {
      setStatus(api.paused() ? 'Пауза' : label(current));
    }
  }

  function init(Module) {
    bindApi(Module);
    document.getElementById('btn-tetris')
      .addEventListener('click', function () { startGame(TETRIS); });
    document.getElementById('btn-snake')
      .addEventListener('click', function () { startGame(SNAKE); });
    document.addEventListener('keydown', onKey);

    // Превью пустого/стартового поля до выбора игры.
    api.start(current);
    api.setHigh(loadHi(current));
    api.tick();
    draw();
    setStatus('Выберите игру или нажмите Enter');
  }

  if (typeof BrickGameModule !== 'function') {
    setStatus('WASM не собран — выполните: make web');
    return;
  }
  BrickGameModule().then(init);
})();
