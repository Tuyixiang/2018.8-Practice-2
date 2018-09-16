//
// Created by Yixiang Tu on 04/09/2018.
//

#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DInput/QInputAspect>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QPointLight>
#include <Qt3DRender/QRenderSettings>
#include <QDirectionalLight>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer>
#include <cmath>
#include <QVector4D>
#include "BoardDisplayWindow.h"
#include "PieceEntity.h"
#include "BoardEntity.h"
#include "Tile.h"
#include "Game.h"

float degSin(float x) {
  return std::sin(x * 3.14159f / 180);
}

float degCos(float x) {
  return std::cos(x * 3.14159f / 180);
}

BoardDisplayWindow::BoardDisplayWindow() : Qt3DWindow() {

  auto input = new Qt3DInput::QInputAspect();
  registerAspect(input);
  defaultFrameGraph()->setClearColor(QColor(17, 8, 3));

  cameraEntity = camera();
  cameraEntity->lens()->setPerspectiveProjection(45.0f, 4.0f / 3.0f, 1.0f, 50.0f);
  cameraEntity->setUpVector(QVector3D(0, 1, 0));

  initialize();

  auto timer = new QTimer(this);
  timer->setInterval(16);
  connect(timer, &QTimer::timeout, this, &BoardDisplayWindow::updateCamera);
  timer->start();
}

void BoardDisplayWindow::mouseMoveEvent(QMouseEvent* ev) {
  if (rotatingCamera) {
    float dx = ev->pos().x() - mousePosition.x();
    float dy = ev->pos().y() - mousePosition.y();
    cameraLift += dy / 20;
    if (cameraLift > 60) {
      cameraLift = 60;
    } else if (cameraLift < 45) {
      cameraLift = 45;
    }
    cameraRotate -= dx / 20;
    if (cameraRotate < 0) {
      cameraRotate += 360;
    } else if (cameraRotate >= 360) {
      cameraRotate -= 360;
    }
    cursor().setPos(mapToGlobal(mousePosition));
    cameraChanged = true;
  } else {
    auto currentTile = getTileFromPos(ev->pos());
    switch (state) {
      case selecting: {
        if (currentTile) {
          currentTile->setEnabled(true);
          if (hoverTile and hoverTile != currentTile) {
            hoverTile->setEnabled(false);
          }
        } else {
          if (hoverTile) {
            hoverTile->setEnabled(false);
          }
        }
        hoverTile = currentTile;
        break;
      }
      case selected: {
        if (currentTile) {
          currentTile->hoverOnAction();
          if (hoverTile and hoverTile != currentTile) {
            hoverTile->loseHoverOnAction();
          }
        } else {
          if (hoverTile) {
            hoverTile->loseHoverOnAction();
          }
        }
        hoverTile = currentTile;
        break;
      }
      default: {
        hoverTile = currentTile;
      }
    }

  }
}

void BoardDisplayWindow::mousePressEvent(QMouseEvent* ev) {
  if (gameStarted) {
    if (ev->button() == Qt::RightButton) {
      mousePosition = ev->pos();
      setCursor(Qt::BlankCursor);
      rotatingCamera = true;
    } else if (ev->button() == Qt::LeftButton) {
      switch (state) {
        case selecting: {
          if (hoverTile) {
            selectedPiece = GAME.positions[hoverTile->x][hoverTile->y];
            if (selectedPiece and !selectedPiece->side) {
              state = selected;
            }
            hoverTile->setEnabled(false);
          }
          break;
        }
        case selected: {
          if (!hoverTile or !hoverTile->state or hoverTile->state == Actions::origin) {
            resetTiles();
            state = selecting;
          } else {
            GAME.turn = false;
            emit movePiece(selectedPiece->x, selectedPiece->y, hoverTile->x, hoverTile->y);
            resetTiles();
            state = stopped;
          }
        }
        default: {
        }
      }
    }
  }
}

void BoardDisplayWindow::mouseReleaseEvent(QMouseEvent* ev) {
  if (ev->button() == Qt::RightButton) {
    rotatingCamera = false;
    unsetCursor();
  } else if (ev->button() == Qt::LeftButton) {
    switch (state) {
      case selecting: {
        // clicked on some irrelevent place
        if (hoverTile) {
          hoverTile->setEnabled(true);
        }
        break;
      }
      case selected: {
        Q_ASSERT(selectedPiece);
        hoverTile->setEnabled(true);
        auto result = GAME.availableActions(selectedPiece);
        for (int i = 0; i < 9; ++i) {
          for (int j = 0; j < 10; ++j) {
            tiles[i][j]->setState(result.pos[i][j]);
          }
        }
        break;
      }
      default: {
      }
    }
  }
  QWindow::mouseReleaseEvent(ev);
}

void BoardDisplayWindow::wheelEvent(QWheelEvent* ev) {
  if (ev->angleDelta().y() > 0) {
    cameraDistanceTarget = std::max(cameraDistanceTarget - 1, 5.0f);
  } else if (ev->angleDelta().y() < 0) {
    cameraDistanceTarget = std::min(cameraDistanceTarget + 1, 13.0f);
  }
}

void BoardDisplayWindow::updateCamera() {
  if (gameStarted) {
    if (cameraDistanceTarget != cameraDistance) {
      if (std::abs(cameraDistance - cameraDistanceTarget) > 0.001f) {
        cameraDistance += (cameraDistanceTarget - cameraDistance) / 12;
      } else {
        cameraDistance = cameraDistanceTarget;
      }
      cameraChanged = true;
    }
    if (cameraChanged) {
      auto position = QVector3D(cameraDistance * degCos(cameraLift) * degSin(cameraRotate),
                                -cameraDistance * degCos(cameraLift) * degCos(cameraRotate),
                                cameraDistance * degSin(cameraLift));
      cameraEntity->setPosition(position);
      cameraEntity->setViewCenter(QVector3D(-0.2f * degSin(cameraRotate), 0.2f * degCos(cameraRotate), -2.3f));

      cameraChanged = false;
    }
  } else {
    if (cameraDistance != 20 - gameStartFactor * gameStartFactor * 8) {
      cameraDistance = 20 - gameStartFactor * gameStartFactor * 8;
      auto position = QVector3D(cameraDistance * degCos(30 * gameStartFactor) * degSin(180 * gameStartFactor),
                                -cameraDistance * degSin(30 * gameStartFactor),
                                -cameraDistance * degCos(30 * gameStartFactor) * degCos(180 * gameStartFactor));
      cameraEntity->setPosition(position);
      cameraEntity->setViewCenter(QVector3D(0, 0.2f * gameStartFactor, -2.3f * gameStartFactor));
    }
  }
}

Tile* BoardDisplayWindow::getTileFromPos(const QPoint& position) {
  auto invert = cameraEntity->transform()->matrix().inverted();
  auto ray = QVector3D((position.x() * 2.0f - width()) / height() * 0.4142135f,
                       (height() - position.y() * 2.0f) / height() * 0.4142135f,
                       -1);
  auto boardNormal = invert * QVector4D(0, 0, 1, 0); //normal vector of board
  auto boardDistance = cameraDistance * degSin(cameraLift);
  // equation:
  // boardDistance / (ray . boardNormal) * ray = origin + ? * xAxis + ? * yAxis
  // projMat * (boardDistance / (ray . boardNormal) * ray) = (x, y)
  auto coord = cameraEntity->transform()->matrix() *
               QVector4D(-boardDistance / (ray.x() * boardNormal.x() + ray.y() * boardNormal.y() +
                                           ray.z() * boardNormal.z()) * ray, 1);
  if (coord.x() > -4.5f and coord.x() < 4.5f and coord.y() > -5 and coord.y() < 5) {
    auto x = static_cast<int>(coord.x() + 4.5f);
    auto y = static_cast<int>(coord.y() + 5.0f);
    x = std::max(0, x);
    x = std::min(8, x);
    y = std::max(0, y);
    y = std::min(9, y);
    return tiles[x][y];
  } else {
    return nullptr;
  }
}

void BoardDisplayWindow::movePieceEntity(PieceEntity* target, int x, int y) {
  target->startAnimation(QPointF(x - 4, y - 4.5));
  disconnect(target, nullptr, nullptr, nullptr);
  connect(target, &PieceEntity::ok, [&, target]() {
    emit ok();
    disconnect(target, nullptr, nullptr, nullptr);
  });
}

void BoardDisplayWindow::resetTiles() {
  for (auto& rol : tiles) {
    for (auto& tile : rol) {
      tile->setState(0);
    }
  }
  selectedPiece = nullptr;
}

void BoardDisplayWindow::gameReady() {
  resetTiles();
  if (GAME.turn) {
    state = selecting;
  }
}

void BoardDisplayWindow::destroyPieceEntity(PieceEntity* target) {
  target->destroy();
  disconnect(target, nullptr, nullptr, nullptr);
  connect(target, &PieceEntity::ok, [&, target]() {
    emit ok();
    disconnect(target, nullptr, nullptr, nullptr);
  });
}

void BoardDisplayWindow::initialize() {
  if (!rootEntity) {
    rootEntity = new Entity();
    setRootEntity(rootEntity);

    auto lightEntity = new Entity(rootEntity);
    {
      auto topLight = new DirectionalLight(lightEntity);
      topLight->setColor("white");
      topLight->setIntensity(0.8f);
      topLight->setWorldDirection(QVector3D(0, 0, -1));
      lightEntity->addComponent(topLight);
    }
    {
      auto bottomLight = new DirectionalLight(lightEntity);
      bottomLight->setColor("white");
      bottomLight->setIntensity(0.3f);
      bottomLight->setWorldDirection(QVector3D(0, 0, 1));
      lightEntity->addComponent(bottomLight);
    }
  } else {
    Entity another;
    for (auto* p: rootEntity->childNodes()) {
      p->setEnabled(false);
      p->setParent(&another);
    }
  }

  new BoardEntity(rootEntity);

  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 10; ++j) {
      tiles[i][j] = new Tile(rootEntity);
      tiles[i][j]->setCoordinate(i, j);
    }
  }

  for (auto& side: GAME.pieces) {
    for (auto& type: side) {
      for (auto& piece: type) {
        piece->entity = new PieceEntity(rootEntity, piece->type, static_cast<bool>(piece->side));
        piece->entity->transform->setTranslation(QVector3D(piece->x - 4, piece->y - 4.5f, 0));
      }
    }
  }

  hoverTile = nullptr;
  selectedPiece = nullptr;
}

void BoardDisplayWindow::startGame() {
  if (!gameStarted) {
    auto animation = new QPropertyAnimation(this, "gameStartFactor");
    animation->setDuration(3000);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
    connect(animation, &QPropertyAnimation::finished, [&]() {
      gameStarted = true;
      cameraEntity->setUpVector(QVector3D(0, 0, 1));
      //qDebug() << "Game Started";
    });
  }
}

void BoardDisplayWindow::setGameStartFactor(float factor) {
  gameStartFactor = factor;
}

void BoardDisplayWindow::moveReady() {
  state = selecting;
}

void BoardDisplayWindow::readPiecePositions() {
  for (auto& side: GAME.pieces) {
    for (auto& type: side) {
      for (auto& piece: type) {
        if (piece->x != -1) {
          piece->entity->transform->setTranslation(QVector3D(piece->x - 4, piece->y - 4.5f, 0));
        } else {
          piece->entity->setEnabled(false);
        }
      }
    }
  }
}
