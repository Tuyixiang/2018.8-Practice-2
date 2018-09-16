//
// Created by Yixiang Tu on 05/09/2018.
//

#include "Tile.h"
#include "Game.h"

Tile::Tile(Entity* parent) : Entity(parent) {

  auto mesh = new CuboidMesh();
  mesh->setXExtent(1);
  mesh->setYExtent(1);
  mesh->setZExtent(0);

  transform = new Transform();

  material = new PhongAlphaMaterial();
  material->setDiffuse(Qt::red);
  material->setAlpha(0.3f);

  addComponent(mesh);
  addComponent(transform);
  addComponent(material);

  setEnabled(false);

}

void Tile::setCoordinate(int xx, int yy) {
  x = xx;
  y = yy;
  transform->setTranslation(QVector3D(x - 4, y - 4.5f, 0.001f));
}

void Tile::setState(int s) {
  state = s;
  switch (s) {
    case Actions::move: {
      setColor(Qt::green);
      setEnabled(true);
      break;
    }
    case Actions::attack: {
      setColor(Qt::yellow);
      setEnabled(true);
      break;
    }
    case Actions::origin: {
      setColor(Qt::red);
      setEnabled(true);
      break;
    }
    default: {
      reset();
    }
  }
}

void Tile::setColor(const QColor& color) {
  material->setDiffuse(color);
}

void Tile::hoverOnAction() {
  switch (state) {
    case Actions::move: {
      setAlpha(0.7f);
      break;
    }
    case Actions::attack: {
      setAlpha(0.7f);
      break;
    }
    default: {
    }
  }
}

void Tile::loseHoverOnAction() {
  switch (state) {
    case Actions::move: {
      setAlpha(0.3f);
      break;
    }
    case Actions::attack: {
      setAlpha(0.3f);
      break;
    }
    default: {
    }
  }
}

void Tile::reset() {
  setAlpha(0.3f);
  setColor(Qt::red);
  setEnabled(false);
}
