//
// Created by Yixiang Tu on 05/09/2018.
//

#ifndef CHINESE_CHESS_GAME_H
#define CHINESE_CHESS_GAME_H

#include "declarations.h"
#include <QObject>

#define GAME Game::instance()

class PieceEntity;

struct Piece {

  Piece(PieceType t, int xx, int yy, int s) :
    type(t), x(xx), y(yy), side(s) {}

  Piece() = default;

  PieceType type = PieceType::jiang;

  /// USING BOTTOM LEFT AS ZERO!!
  int x = -1;
  int y = -1;

  /// black: 0; red: 1
  int side = -1;

  PieceEntity* entity = nullptr;

};

struct Actions {
  Actions() {
    memset(pos, 0, sizeof(pos));
  }

  void canMove(int x, int y) {
    if (x >= 0 and x <= 8 and y >= 0 and y <= 9) {
      pos[x][y] = move;
    }
  }

  void canAttack(int x, int y) {
    if (x >= 0 and x <= 8 and y >= 0 and y <= 9) {
      pos[x][y] = attack;
    }
  }

  enum ActionType : int {
    null = 0,
    move = 1,
    attack = 2,
    origin = 3
  } pos[9][10];
};

class Game : public QObject {

Q_OBJECT

public:
  static Game& instance() {
    static Game game;
    return game;
  }

  void reset();

  Actions availableActions(Piece* target);

  Actions currentActions;

  QVector<Piece*> attackList;

  QVector<Piece*> pieces[2][7];
  Piece* positions[9][10];

  /*
   * after emitting a signal which requires time,
   * this will wait until each emission sends back its signal
   *
   * WARNING:
   * while okCount is on count down, nothing more should happen
   */
  int okCount;

  bool turn = false;

  int gameStatus = 0;

  bool hasLoadGame = false;

  QByteArray gameLoad;

  inline bool inRange(int x, int y);

  void getAttackList(int side);

  bool readFile();

  bool readData(const QByteArray& data);

  QByteArray generateLine(int type, int x, int y) {
    QByteArray line("0 <0,0>\n", 8);
    line[0] = type + '0';
    line[3] = x + '0';
    line[5] = y + '0';
    return line;
  }

  void readLine(const QByteArray& line, int side) {
    auto& type = pieces[side][line[0] - '0'];
    for (auto& piece: type) {
      if (piece->x == -1) {
        int x = line[3] - '0';
        int y = line[5] - '0';
        piece->x = x;
        piece->y = y;
        positions[x][y] = piece;
        break;
      }
    }
  }

  QByteArray dumpMirroredData();

signals:

  void movePieceEntity(PieceEntity* target, int, int);

  void destroyPieceEntity(PieceEntity*);

  void ready();

  void lose();

public slots:

  void movePiece(int x1, int y1, int x2, int y2);

  void ok();

  void dumpFile();

protected:
  Game() : QObject() {
    reset();
  }

public:
  Game(const Game& e) = delete;

  Game& operator=(const Game& e) = delete;
};


#endif //CHINESE_CHESS_GAME_H
