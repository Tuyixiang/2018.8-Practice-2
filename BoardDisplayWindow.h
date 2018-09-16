//
// Created by Yixiang Tu on 04/09/2018.
//

#ifndef CHINESE_CHESS_BOARDDISPLAY_H
#define CHINESE_CHESS_BOARDDISPLAY_H


#include <Qt3DWindow>
#include <QOpenGLFunctions>
#include "declarations.h"
#include "PieceEntity.h"

class Tile;

struct Piece;

class BoardDisplayWindow : public Qt3DExtras::Qt3DWindow {

Q_OBJECT
  Q_PROPERTY(float gameStartFactor
               WRITE
               setGameStartFactor)

public:
  BoardDisplayWindow();

  bool gameStarted = false;

signals:

  void movePiece(int, int, int, int);

  void ok();

public slots:

  void movePieceEntity(PieceEntity* target, int x, int y);

  void destroyPieceEntity(PieceEntity* target);

  void gameReady();

  void startGame();

  void moveReady();

  void readPiecePositions();

  void stop() {
    state = stopped;
  }

protected:
  Camera* cameraEntity = nullptr;

  Tile* tiles[9][10];
  /// the tile at which cursor points, recorded even when not displayed (e.g. once selected)
  Tile* hoverTile = nullptr;
  Piece* selectedPiece = nullptr;
  enum State {
    stopped,
    selecting,
    selected
  } state = stopped;

  void mouseMoveEvent(QMouseEvent* ev) override;

  void mousePressEvent(QMouseEvent* ev) override;

  void mouseReleaseEvent(QMouseEvent* ev) override;

  void wheelEvent(QWheelEvent* ev) override;

  void resetTiles();

  Tile* getTileFromPos(const QPoint& position);

  void setGameStartFactor(float factor);

  void updateCamera();

  /// record mouse position when right button pressed
  QPoint mousePosition;

  float cameraRotate = 0;

  float cameraLift = 60.0f;

  float cameraDistance = 12;

  float cameraDistanceTarget = 12;

  bool rotatingCamera = false;

  bool cameraChanged = true;

  float gameStartFactor = 0;

  Entity* rootEntity = nullptr;

public:
  void initialize();


};


#endif //CHINESE_CHESS_BOARDDISPLAY_H
