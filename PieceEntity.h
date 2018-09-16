//
// Created by Yixiang Tu on 05/09/2018.
//

#ifndef CHINESE_CHESS_PIECE_H
#define CHINESE_CHESS_PIECE_H

#include "declarations.h"
#include <QFont>
#include <QString>
#include <QPropertyAnimation>

class PieceEntity : public Entity {

Q_OBJECT
  Q_PROPERTY(QPointF pos
               WRITE
               setPos)
  Q_PROPERTY(float height
               WRITE
               setHeight);

public:
  PieceEntity(Entity* parent, PieceType type, bool isRed = false);

  Transform* transform;

  PieceType pieceType;

  void startAnimation(const QPointF& dest);

  void destroy();

  bool red;

signals:

  void ok();

protected:
  void setPos(const QPointF& pos);

  void setHeight(float height);

  static QFont* font;

  static QString pieceNames[2][7];

  QPropertyAnimation* posAnimation = nullptr;
  QPropertyAnimation* heightAnimation = nullptr;

};


#endif //CHINESE_CHESS_PIECE_H
