//
// Created by Yixiang Tu on 05/09/2018.
//

#ifndef CHINESE_CHESS_DECLARATIONS_H
#define CHINESE_CHESS_DECLARATIONS_H

#include <Qt3DCore/QEntity>

#include <Qt3DExtras/QExtrudedTextMesh>
#include <Qt3DExtras/QTorusMesh>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DRender/QDirectionalLight>
#include <Qt3DRender/QPointLight>
#include <QCuboidMesh>
#include <QTextureMaterial>
#include <QTexture>
#include <QCamera>
#include <QPhongAlphaMaterial>
#include <QDepthTest>

typedef Qt3DCore::QEntity Entity;
typedef Qt3DCore::QTransform Transform;
typedef Qt3DExtras::QPhongMaterial PhongMaterial;
typedef Qt3DExtras::QPhongAlphaMaterial PhongAlphaMaterial;
typedef Qt3DExtras::QTorusMesh TorusMesh;
typedef Qt3DExtras::QCylinderMesh CylinderMesh;
typedef Qt3DExtras::QExtrudedTextMesh TextMesh;
typedef Qt3DExtras::QCuboidMesh CuboidMesh;
typedef Qt3DExtras::QTextureMaterial TextureMaterial;
typedef Qt3DRender::QTextureLoader TextureLoader;
typedef Qt3DRender::QCamera Camera;
typedef Qt3DRender::QDirectionalLight DirectionalLight;
typedef Qt3DRender::QPointLight PointLight;
typedef Qt3DRender::QDepthTest DepthTest;


enum PieceType : int {
  jiang = 0,
  shi = 1,
  xiang = 2,
  ma = 3,
  ju = 4,
  pao = 5,
  zu = 6
};

#endif //CHINESE_CHESS_DECLARATIONS_H
