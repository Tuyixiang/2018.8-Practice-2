//
// Created by Yixiang Tu on 05/09/2018.
//

#ifndef CHINESE_CHESS_BOARD_H
#define CHINESE_CHESS_BOARD_H

#include "declarations.h"

class BoardEntity : public Entity {

Q_OBJECT

public:
  BoardEntity(Entity* parent);

protected:
  void addGrid(float x1, float y1, float x2, float y2, float width, float depth);

};


#endif //CHINESE_CHESS_BOARD_H
