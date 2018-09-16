//
// Created by Yixiang Tu on 05/09/2018.
//

#include "BoardEntity.h"
#include <cmath>

BoardEntity::BoardEntity(Entity* parent) : Entity(parent) {

  auto boardPlane = new Entity(this);
  {
    auto boardPlaneMesh = new CuboidMesh();
    boardPlaneMesh->setXExtent(9.0f);
    boardPlaneMesh->setYExtent(10.0f);
    boardPlaneMesh->setZExtent(1.0f);

    auto boardPlaneTransform = new Transform();
    boardPlaneTransform->setTranslation(QVector3D(0, 0, -0.5f));

    auto textureLoader = new TextureLoader();
    textureLoader->setSource(QUrl("qrc:/res/wood.jpg"));
    auto boardMaterial = new TextureMaterial();
    boardMaterial->setTexture(textureLoader);

    boardPlane->addComponent(boardPlaneMesh);
    boardPlane->addComponent(boardPlaneTransform);
    boardPlane->addComponent(boardMaterial);
  }

  /// Grids
  {
    {
      /// Boarders
      addGrid(-4, -4.5f, -4, 4.5f, 0.1f, 0.1f);
      addGrid(4, -4.5f, 4, 4.5f, 0.1f, 0.1f);
      addGrid(-4, 4.5f, 4, 4.5f, 0.1f, 0.1f);
      addGrid(-4, -4.5f, 4, -4.5f, 0.1f, 0.1f);

      /// Horizontals
      addGrid(-4, 3.5f, 4, 3.5f, 0.03f, 0.03f);
      addGrid(-4, 2.5f, 4, 2.5f, 0.03f, 0.03f);
      addGrid(-4, 1.5f, 4, 1.5f, 0.03f, 0.03f);
      addGrid(-4, 0.5f, 4, 0.5f, 0.05f, 0.05f);
      addGrid(-4, -0.5f, 4, -0.5f, 0.05f, 0.05f);
      addGrid(-4, -1.5f, 4, -1.5f, 0.03f, 0.03f);
      addGrid(-4, -2.5f, 4, -2.5f, 0.03f, 0.03f);
      addGrid(-4, -3.5f, 4, -3.5f, 0.03f, 0.03f);

      /// Verticals
      addGrid(-3, -4.5f, -3, -0.5f, 0.03f, 0.03f);
      addGrid(-2, -4.5f, -2, -0.5f, 0.03f, 0.03f);
      addGrid(-1, -4.5f, -1, -0.5f, 0.03f, 0.03f);
      addGrid(0, -4.5f, 0, -0.5f, 0.03f, 0.03f);
      addGrid(1, -4.5f, 1, -0.5f, 0.03f, 0.03f);
      addGrid(2, -4.5f, 2, -0.5f, 0.03f, 0.03f);
      addGrid(3, -4.5f, 3, -0.5f, 0.03f, 0.03f);
      addGrid(-3, 0.5f, -3, 4.5f, 0.03f, 0.03f);
      addGrid(-2, 0.5f, -2, 4.5f, 0.03f, 0.03f);
      addGrid(-1, 0.5f, -1, 4.5f, 0.03f, 0.03f);
      addGrid(0, 0.5f, 0, 4.5f, 0.03f, 0.03f);
      addGrid(1, 0.5f, 1, 4.5f, 0.03f, 0.03f);
      addGrid(2, 0.5f, 2, 4.5f, 0.03f, 0.03f);
      addGrid(3, 0.5f, 3, 4.5f, 0.03f, 0.03f);

      /// Cross
      addGrid(-1, -2.5f, 1, -4.5f, 0.03f, 0.03f);
      addGrid(1, -2.5f, -1, -4.5f, 0.03f, 0.03f);
      addGrid(-1, 2.5f, 1, 4.5f, 0.03f, 0.03f);
      addGrid(1, 2.5f, -1, 4.5f, 0.03f, 0.03f);
    }
  }

}

void BoardEntity::addGrid(float x1, float y1, float x2, float y2, float width, float depth) {

  auto grid = new Entity(this);

  if (x1 == x2) {
    auto mesh = new CuboidMesh();
    mesh->setXExtent(width);
    mesh->setYExtent(y2 - y1 + width);
    mesh->setZExtent(depth);
    auto transform = new Transform();
    transform->setTranslation(QVector3D(x1, (y1 + y2) / 2, 0));
    auto material = new PhongMaterial();
    material->setDiffuse(QColor(39, 16, 8));
    material->setShininess(0);
    grid->addComponent(mesh);
    grid->addComponent(transform);
    grid->addComponent(material);
  } else if (y1 == y2) {
    auto mesh = new CuboidMesh();
    mesh->setXExtent(x2 - x1 + width);
    mesh->setYExtent(width);
    mesh->setZExtent(depth);
    auto transform = new Transform();
    transform->setTranslation(QVector3D((x1 + x2) / 2, y1, 0));
    auto material = new PhongMaterial();
    material->setDiffuse(QColor(39, 16, 8));
    material->setShininess(0);
    grid->addComponent(mesh);
    grid->addComponent(transform);
    grid->addComponent(material);
  } else {
    auto length = std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
    auto mesh = new CuboidMesh();
    mesh->setXExtent(length);
    mesh->setYExtent(width);
    mesh->setZExtent(depth);
    auto transform = new Transform();
    transform->setRotation(QQuaternion::fromAxisAndAngle(0, 0, 1, 180 * std::atan((y2 - y1) / (x2 - x1)) / 3.14159f));
    transform->setTranslation(QVector3D((x1 + x2) / 2, (y1 + y2) / 2, 0));
    auto material = new PhongMaterial();
    material->setDiffuse(QColor(39, 16, 8));
    material->setShininess(0);
    grid->addComponent(mesh);
    grid->addComponent(transform);
    grid->addComponent(material);
  }

}
