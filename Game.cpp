//
// Created by Yixiang Tu on 05/09/2018.
//

#include <QFile>
#include "Game.h"
#include "Tile.h"
#include "MediaPlayer.h"

void Game::reset() {
  turn = false;
  gameStatus = 0;
  hasLoadGame = false;
  for (auto& col : positions) {
    for (auto& p: col) {
      p = nullptr;
    }
  }
  for (auto& side : pieces) {
    for (auto& type : side) {
      for (auto& p : type) {
        delete p;
      }
      type.clear();
    }
  }
  pieces[0][0].push_back(new Piece(PieceType::jiang, 4, 0, 0));
  pieces[0][1].push_back(new Piece(PieceType::shi, 3, 0, 0));
  pieces[0][1].push_back(new Piece(PieceType::shi, 5, 0, 0));
  pieces[0][2].push_back(new Piece(PieceType::xiang, 2, 0, 0));
  pieces[0][2].push_back(new Piece(PieceType::xiang, 6, 0, 0));
  pieces[0][3].push_back(new Piece(PieceType::ma, 1, 0, 0));
  pieces[0][3].push_back(new Piece(PieceType::ma, 7, 0, 0));
  pieces[0][4].push_back(new Piece(PieceType::ju, 0, 0, 0));
  pieces[0][4].push_back(new Piece(PieceType::ju, 8, 0, 0));
  pieces[0][5].push_back(new Piece(PieceType::pao, 1, 2, 0));
  pieces[0][5].push_back(new Piece(PieceType::pao, 7, 2, 0));
  pieces[0][6].push_back(new Piece(PieceType::zu, 0, 3, 0));
  pieces[0][6].push_back(new Piece(PieceType::zu, 2, 3, 0));
  pieces[0][6].push_back(new Piece(PieceType::zu, 4, 3, 0));
  pieces[0][6].push_back(new Piece(PieceType::zu, 6, 3, 0));
  pieces[0][6].push_back(new Piece(PieceType::zu, 8, 3, 0));
  positions[4][0] = pieces[0][0][0];
  positions[3][0] = pieces[0][1][0];
  positions[5][0] = pieces[0][1][1];
  positions[2][0] = pieces[0][2][0];
  positions[6][0] = pieces[0][2][1];
  positions[1][0] = pieces[0][3][0];
  positions[7][0] = pieces[0][3][1];
  positions[0][0] = pieces[0][4][0];
  positions[8][0] = pieces[0][4][1];
  positions[1][2] = pieces[0][5][0];
  positions[7][2] = pieces[0][5][1];
  positions[0][3] = pieces[0][6][0];
  positions[2][3] = pieces[0][6][1];
  positions[4][3] = pieces[0][6][2];
  positions[6][3] = pieces[0][6][3];
  positions[8][3] = pieces[0][6][4];

  pieces[1][0].push_back(new Piece(PieceType::jiang, 4, 9, 1));
  pieces[1][1].push_back(new Piece(PieceType::shi, 3, 9, 1));
  pieces[1][1].push_back(new Piece(PieceType::shi, 5, 9, 1));
  pieces[1][2].push_back(new Piece(PieceType::xiang, 2, 9, 1));
  pieces[1][2].push_back(new Piece(PieceType::xiang, 6, 9, 1));
  pieces[1][3].push_back(new Piece(PieceType::ma, 1, 9, 1));
  pieces[1][3].push_back(new Piece(PieceType::ma, 7, 9, 1));
  pieces[1][4].push_back(new Piece(PieceType::ju, 0, 9, 1));
  pieces[1][4].push_back(new Piece(PieceType::ju, 8, 9, 1));
  pieces[1][5].push_back(new Piece(PieceType::pao, 1, 7, 1));
  pieces[1][5].push_back(new Piece(PieceType::pao, 7, 7, 1));
  pieces[1][6].push_back(new Piece(PieceType::zu, 0, 6, 1));
  pieces[1][6].push_back(new Piece(PieceType::zu, 2, 6, 1));
  pieces[1][6].push_back(new Piece(PieceType::zu, 4, 6, 1));
  pieces[1][6].push_back(new Piece(PieceType::zu, 6, 6, 1));
  pieces[1][6].push_back(new Piece(PieceType::zu, 8, 6, 1));
  positions[4][9] = pieces[1][0][0];
  positions[3][9] = pieces[1][1][0];
  positions[5][9] = pieces[1][1][1];
  positions[2][9] = pieces[1][2][0];
  positions[6][9] = pieces[1][2][1];
  positions[1][9] = pieces[1][3][0];
  positions[7][9] = pieces[1][3][1];
  positions[0][9] = pieces[1][4][0];
  positions[8][9] = pieces[1][4][1];
  positions[1][7] = pieces[1][5][0];
  positions[7][7] = pieces[1][5][1];
  positions[0][6] = pieces[1][6][0];
  positions[2][6] = pieces[1][6][1];
  positions[4][6] = pieces[1][6][2];
  positions[6][6] = pieces[1][6][3];
  positions[8][6] = pieces[1][6][4];
}

Actions Game::availableActions(Piece* target) {
  currentActions = Actions();
  currentActions.pos[target->x][target->y] = Actions::origin;
  switch (target->type) {
    case PieceType::jiang: {
      if (target->side == 0) {
        for (int i = 3; i <= 5; ++i) {
          for (int j = 0; j <= 2; ++j) {
            if (abs(i - target->x) + abs(j - target->y) == 1) {
              if (positions[i][j]) {
                if (positions[i][j]->side) {
                  currentActions.pos[i][j] = Actions::attack;
                  attackList.push_back(positions[i][j]);
                }
              } else {
                currentActions.pos[i][j] = Actions::move;
              }
            }
          }
        }
        auto opp = GAME.pieces[1][0][0];
        if (target->x == opp->x) {
          bool block = false;
          for (int j = opp->y - 1; j > target->y; --j) {
            if (GAME.positions[target->x][j]) {
              block = true;
              break;
            }
          }
          if (!block) {
            currentActions.pos[opp->x][opp->y] = Actions::attack;
            attackList.push_back(positions[opp->x][opp->y]);
          }
        }
      } else {
        for (int i = 3; i <= 5; ++i) {
          for (int j = 7; j <= 9; ++j) {
            if (abs(i - target->x) + abs(j - target->y) == 1) {
              if (positions[i][j]) {
                if (!positions[i][j]->side) {
                  currentActions.pos[i][j] = Actions::attack;
                  attackList.push_back(positions[i][j]);
                }
              } else {
                currentActions.pos[i][j] = Actions::move;
              }
            }
          }
        }
        auto opp = GAME.pieces[0][0][0];
        if (target->x == opp->x) {
          bool block = false;
          for (int j = opp->y + 1; j < target->y; ++j) {
            if (GAME.positions[target->x][j]) {
              block = true;
              break;
            }
          }
          if (!block) {
            currentActions.pos[opp->x][opp->y] = Actions::attack;
            attackList.push_back(positions[opp->x][opp->y]);
          }
        }
      }
      break;
    }
    case PieceType::shi: {
      if (target->side == 0) {
        if (target->x == 4) {
          for (int i = 3; i <= 5; i += 2) {
            for (int j = 0; j <= 2; j += 2) {
              if (positions[i][j]) {
                if (positions[i][j]->side) {
                  currentActions.pos[i][j] = Actions::attack;
                  attackList.push_back(positions[i][j]);
                }
              } else {
                currentActions.pos[i][j] = Actions::move;
              }
            }
          }
        } else {
          if (positions[4][1]) {
            if (positions[4][1]->side) {
              currentActions.pos[4][1] = Actions::attack;
              attackList.push_back(positions[4][1]);
            }
          } else {
            currentActions.pos[4][1] = Actions::move;
          }
        }
      } else {
        if (target->x == 4) {
          for (int i = 3; i <= 5; i += 2) {
            for (int j = 7; j <= 9; j += 2) {
              if (positions[i][j]) {
                if (!positions[i][j]->side) {
                  currentActions.pos[i][j] = Actions::attack;
                  attackList.push_back(positions[i][j]);
                }
              } else {
                currentActions.pos[i][j] = Actions::move;
              }
            }
          }
        } else {
          if (positions[4][8]) {
            if (!positions[4][8]->side) {
              currentActions.pos[4][8] = Actions::attack;
              attackList.push_back(positions[4][8]);
            }
          } else {
            currentActions.pos[4][8] = Actions::move;
          }
        }
      }
      break;
    }
    case PieceType::xiang: {
      for (int dx = -2; dx <= 2; dx += 4) {
        for (int dy = -2; dy <= 2; dy += 4) {
          int x = target->x + dx;
          int y = target->y + dy;
          if (!inRange(x, y) or
              (target->side and y < 5) or
              (!target->side and y > 4) or
              positions[x - dx / 2][y - dy / 2] != nullptr) {
            continue;
          }
          if (positions[x][y]) {
            if (positions[x][y]->side != target->side) {
              currentActions.pos[x][y] = Actions::attack;
              attackList.push_back(positions[x][y]);
            }
          } else {
            currentActions.pos[x][y] = Actions::move;
          }
        }
      }
      break;
    }
    case PieceType::ma: {
      for (int i = -1; i <= 1; i += 2) {
        for (int j = -1; j <= 1; j += 2) {
          for (int k = 0; k <= 1; ++k) {
            int x, y;
            if (k) {
              x = target->x + i * 2;
              y = target->y + j;
            } else {
              x = target->x + i;
              y = target->y + j * 2;
            }
            if (!inRange(x, y) or
                positions[target->x + k * i][target->y + (1 - k) * j] != nullptr) {
              continue;
            }
            if (positions[x][y]) {
              if (positions[x][y]->side != target->side) {
                currentActions.pos[x][y] = Actions::attack;
                attackList.push_back(positions[x][y]);
              }
            } else {
              currentActions.pos[x][y] = Actions::move;
            }
          }
        }
      }
      break;
    }
    case PieceType::ju: {
      for (int vert = 0; vert <= 1; ++vert) {
        for (int dir = -1; dir <= 1; dir += 2) {
          for (int step = 1;; ++step) {
            int x = target->x + (1 - vert) * dir * step;
            int y = target->y + vert * dir * step;
            if (!inRange(x, y)) {
              break;
            }
            if (positions[x][y]) {
              if (positions[x][y]->side != target->side) {
                currentActions.pos[x][y] = Actions::attack;
                attackList.push_back(positions[x][y]);
              }
              break;
            }
            currentActions.pos[x][y] = Actions::move;
          }
        }
      }
      break;
    }
    case PieceType::pao: {
      for (int vert = 0; vert <= 1; ++vert) {
        for (int dir = -1; dir <= 1; dir += 2) {
          bool notBlocked = true;
          for (int step = 1;; ++step) {
            int x = target->x + (1 - vert) * dir * step;
            int y = target->y + vert * dir * step;
            if (!inRange(x, y)) {
              break;
            }
            if (positions[x][y]) {
              if (notBlocked) {
                notBlocked = false;
                continue;
              } else {
                if (positions[x][y]->side != target->side) {
                  currentActions.pos[x][y] = Actions::attack;
                  attackList.push_back(positions[x][y]);
                }
                break;
              }
            }
            if (notBlocked) {
              currentActions.pos[x][y] = Actions::move;
            }
          }
        }
      }
      break;
    }
    case PieceType::zu: {
      int horizontalRange = 1;
      if ((target->side and target->y >= 5) or
          (!target->side and target->y <= 4)) {
        horizontalRange = 0;
      }
      for (int dx = -horizontalRange; dx <= horizontalRange; ++dx) {
        int x = target->x + dx;
        int y = target->y + (dx ? 0 : 1) * (target->side ? -1 : 1);
        if (!inRange(x, y)) {
          continue;
        }
        if (positions[x][y]) {
          if (positions[x][y]->side != target->side) {
            currentActions.pos[x][y] = Actions::attack;
            attackList.push_back(positions[x][y]);
          }
        } else {
          currentActions.pos[x][y] = Actions::move;
        }
      }
      break;
    }
  }

  return currentActions;
}

bool Game::inRange(int x, int y) {
  return x >= 0 and x <= 8 and y >= 0 and y <= 9;
}

void Game::movePiece(int x1, int y1, int x2, int y2) {
  Q_ASSERT(positions[x1][y1]);
  Q_ASSERT(!positions[x2][y2] or positions[x2][y2]->side != positions[x1][y1]->side);
  auto side = positions[x1][y1]->side;

  if (positions[x2][y2]) {
    /// killer move
    auto killed = positions[x2][y2];
    auto killer = positions[x1][y1];
    if (killed->type == PieceType::jiang) {
      if (killer->side) {
        emit lose();
        gameStatus = -1;
      } else {
        gameStatus = 1;
      }
    } else {
      if (killer->side) {
        PLAY(killed)
      } else {
        PLAY(kill)
      }
    }
    /*
     * 1. move/destroy the entities according to positions
     * 2. change GAME data
     * however neither Piece nor PieceEntity are destroyed,
     * and they are still linked
     */
    emit destroyPieceEntity(killed->entity);
    emit movePieceEntity(killer->entity, x2, y2);
    okCount = 2;
    killed->x = killed->y = -1;
    positions[x2][y2] = killer;
    killer->x = x2;
    killer->y = y2;
    positions[x1][y1] = nullptr;
  } else {
    PLAY(move)
    /// normal move
    auto mover = positions[x1][y1];
    emit movePieceEntity(mover->entity, x2, y2);
    okCount = 1;
    positions[x2][y2] = mover;
    mover->x = x2;
    mover->y = y2;
    positions[x1][y1] = nullptr;
  }
  if (side) {
    getAttackList(1);
    for (auto& p: attackList) {
      if (p->type == PieceType::jiang) {
        PLAY(checked)
      }
    }
  } else {
    getAttackList(0);
    for (auto& p: attackList) {
      if (p->type == PieceType::jiang) {
        PLAY(check)
      }
    }
  }
}

void Game::ok() {
  --okCount;
  if (!okCount) {
    emit ready();
  }
}

void Game::getAttackList(int side) {
  attackList.clear();
  for (auto& type: pieces[side]) {
    for (auto& piece: type) {
      availableActions(piece);
    }
  }
}

void Game::dumpFile() {
  QByteArray data;
  if (turn) {
    data += "you\n";
    for (int i = 0; i < 7; ++i) {
      for (auto& piece: pieces[0][i]) {
        if (piece->x != -1) {
          data += generateLine(i, piece->x, piece->y);
        }
      }
    }
    data += "opponent\n";
    for (int i = 0; i < 7; ++i) {
      for (auto& piece: pieces[1][i]) {
        if (piece->x != -1) {
          data += generateLine(i, piece->x, piece->y);
        }
      }
    }
    QFile out("save.txt");
    if (!out.open(QIODevice::WriteOnly | QIODevice::Text)) {
      qDebug() << "failed to open file";
    } else {
      out.write(data);
      out.close();
    }
  } else {
    data += "opponent\n";
    for (int i = 0; i < 7; ++i) {
      for (auto& piece: pieces[1][i]) {
        if (piece->x != -1) {
          data += generateLine(i, piece->x, piece->y);
        }
      }
    }
    data += "you\n";
    for (int i = 0; i < 7; ++i) {
      for (auto& piece: pieces[0][i]) {
        if (piece->x != -1) {
          data += generateLine(i, piece->x, piece->y);
        }
      }
    }
    QFile out("save.txt");
    if (!out.open(QIODevice::WriteOnly | QIODevice::Text)) {
      qDebug() << "failed to open file";
    } else {
      out.write(data);
      out.close();
    }
  }
}

bool Game::readFile() {
  hasLoadGame = true;
  QFile in("save.txt");
  if (!in.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "failed to open";
  } else {
    gameLoad = in.readAll();
    return readData(gameLoad);
  }
  return turn;
}

bool Game::readData(const QByteArray& data) {
  for (auto& side: pieces) {
    for (auto& type: side) {
      for (auto& piece: type) {
        piece->x = -1;
        piece->y = -1;
      }
    }
  }
  for (auto& col: positions) {
    for (auto& position: col) {
      position = nullptr;
    }
  }
  auto lines = data.split('\n');
  qDebug() << lines;
  int side = 0;
  for (auto& line : lines) {
    if (line.isEmpty()) {
      continue;
    }
    if (line[0] == 'o') {
      side = 1;
      continue;
    }
    if (line[0] == 'y') {
      side = 0;
      continue;
    }
    readLine(line, side);
  }
  return (turn = static_cast<bool>(side));
}

QByteArray Game::dumpMirroredData() {
  QByteArray data;
  if (turn) {
    data += "opponent\n";
    for (int i = 0; i < 7; ++i) {
      for (auto& piece: pieces[0][i]) {
        if (piece->x != -1) {
          data += generateLine(i, 8 - piece->x, 9 - piece->y);
        }
      }
    }
    data += "you\n";
    for (int i = 0; i < 7; ++i) {
      for (auto& piece: pieces[1][i]) {
        if (piece->x != -1) {
          data += generateLine(i, 8 - piece->x, 9 - piece->y);
        }
      }
    }
  } else {
    data += "you\n";
    for (int i = 0; i < 7; ++i) {
      for (auto& piece: pieces[1][i]) {
        if (piece->x != -1) {
          data += generateLine(i, 8 - piece->x, 9 - piece->y);
        }
      }
    }
    data += "opponent\n";
    for (int i = 0; i < 7; ++i) {
      for (auto& piece: pieces[0][i]) {
        if (piece->x != -1) {
          data += generateLine(i, 8 - piece->x, 9 - piece->y);
        }
      }
    }
  }
  return data;
}
