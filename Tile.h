//
// Created by Yixiang Tu on 05/09/2018.
//

#ifndef CHINESE_CHESS_TILE_H
#define CHINESE_CHESS_TILE_H

#include "declarations.h"
#include <QPropertyAnimation>

class Tile : public Entity {

Q_OBJECT
  Q_PROPERTY(float alpha
               WRITE
               setAlpha)

public:
  Tile(Entity* parent);

  Transform* transform;

  void setCoordinate(int xx, int yy);

  void setColor(const QColor& color);

  void setState(int s);

  /// mouse over this tile while choosing an action for a piece
  void hoverOnAction();

  void loseHoverOnAction();

  int x;
  int y;
  int state = 0;

protected:
  void setAlpha(float alpha) {
    material->setAlpha(alpha);
  }

  void reset();

  PhongAlphaMaterial* material;

  QPropertyAnimation* animation;

};


#endif //CHINESE_CHESS_TILE_H
