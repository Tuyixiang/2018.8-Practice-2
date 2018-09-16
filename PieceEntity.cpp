//
// Created by Yixiang Tu on 05/09/2018.
//

#include "PieceEntity.h"
#include <QFontDatabase>

QString PieceEntity::pieceNames[2][7] = {
  {"将", "士", "象", "马", "车", "炮", "卒"},
  {"帅", "仕", "相", "马", "车", "炮", "兵"}
};

QFont* PieceEntity::font = nullptr;

PieceEntity::PieceEntity(Entity* parent, PieceType type, bool isRed) :
  Entity(parent),
  pieceType(type),
  red(isRed) {

  if (!font) {
    font = new QFont("HYCuSongF");
    //font = new QFont(QFontDatabase::applicationFontFamilies(0)[0]);
    //qDebug() << QFontDatabase::applicationFontFamilies(0)[0];
  }

  auto character = new Entity(this);
  {
    auto mesh = new TextMesh();
    mesh->setDepth(0.05f);
    mesh->setFont(*font);
    mesh->setText(pieceNames[red][type]);

    auto transform = new Transform();
    transform->setScale(3.0f);
    transform->setTranslation(QVector3D(-1.5f, -0.9f, 2.0f));

    auto material = new PhongMaterial();
    material->setDiffuse(red ? Qt::darkRed : QColor(25, 25, 25));
    material->setShininess(100.0f);

    character->addComponent(mesh);
    character->addComponent(transform);
    character->addComponent(material);
  }

  auto ring = new Entity(this);
  {
    auto mesh = new TorusMesh();
    mesh->setRadius(2.0f);
    mesh->setMinorRadius(0.15f);
    mesh->setSlices(100);
    mesh->setRings(100);

    auto transform = new Transform();
    transform->setTranslation(QVector3D(0, 0, 2));

    auto material = new PhongMaterial();
    material->setDiffuse(red ? Qt::darkRed : QColor(25, 25, 25));
    material->setShininess(70.0f);

    ring->addComponent(mesh);
    ring->addComponent(transform);
    ring->addComponent(material);
  }

  auto cylinder = new Entity(this);
  {
    auto mesh = new CylinderMesh();
    mesh->setRadius(2.0f);
    mesh->setLength(2.0f);
    mesh->setRings(100);
    mesh->setSlices(100);

    auto transform = new Transform();
    transform->setTranslation(QVector3D(0, 0, 1));
    transform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 90));

    auto material = new PhongMaterial();
    material->setDiffuse(QColor(243, 166, 99));
    material->setShininess(50);

    cylinder->addComponent(mesh);
    cylinder->addComponent(transform);
    cylinder->addComponent(material);
  }

  auto torus = new Entity(this);
  {
    auto mesh = new TorusMesh();
    mesh->setRadius(2.0f);
    mesh->setMinorRadius(1.0f);
    mesh->setRings(100);
    mesh->setSlices(100);

    auto transform = new Transform();
    transform->setTranslation(QVector3D(0, 0, 1));

    auto material = new PhongMaterial();
    material->setDiffuse(QColor(243, 166, 99));
    material->setShininess(50);

    torus->addComponent(mesh);
    torus->addComponent(transform);
    torus->addComponent(material);
  }

  transform = new Transform();
  transform->setScale(0.14f);
  addComponent(transform);

}

void PieceEntity::setPos(const QPointF& pos) {
  transform->setTranslation(QVector3D(static_cast<float>(pos.x()),
                                      static_cast<float>(pos.y()),
                                      transform->translation().z()));
}

void PieceEntity::setHeight(float height) {
  transform->setTranslation(QVector3D(transform->translation().x(),
                                      transform->translation().y(),
                                      height));
}

void PieceEntity::startAnimation(const QPointF& dest) {

  delete posAnimation;

  posAnimation = new QPropertyAnimation(this, "pos");
  posAnimation->setDuration(500);
  posAnimation->setStartValue(QPointF(transform->translation().x(), transform->translation().y()));
  posAnimation->setEndValue(dest);
  posAnimation->setEasingCurve(QEasingCurve::OutQuad);
  posAnimation->start();
  connect(posAnimation, &QPropertyAnimation::finished, [&]() {
    emit ok();
  });
}

void PieceEntity::destroy() {

  delete heightAnimation;

  heightAnimation = new QPropertyAnimation(this, "height");
  heightAnimation->setDuration(500);
  heightAnimation->setStartValue(0);
  heightAnimation->setEndValue(-0.5f);
  heightAnimation->start();
  connect(heightAnimation, &QPropertyAnimation::finished, [&]() {
    setEnabled(false);
    emit ok();
  });
}
