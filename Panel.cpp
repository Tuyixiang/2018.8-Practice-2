//
// Created by Yixiang Tu on 07/09/2018.
//

#include "Panel.h"
#include "MediaPlayer.h"
#include <QLayout>
#include <QPainter>
#include <QTimer>
#include <QMouseEvent>

Panel::Panel(QWidget* parent) : QWidget(parent) {

  background = QImage(":/res/wood2.jpg");
  yourTurn = QImage(":/res/your.png");
  opponentTurn = QImage(":/res/opp.png");
  concede = QImage(":/res/concede.png");
  title = QImage(":/res/title.png");
  winImage = QImage(":/res/win.png");
  loseImage = QImage(":/res/lose.png");
  timerPen.setColor(QColor(255, 223, 205));
  timerPen.setWidthF(5);
  timerRect = QRectF(100, 256, 120, 120);
  animation = new QPropertyAnimation(this, "timeRemaining");
  animation->setStartValue(20);
  animation->setEndValue(0);
  animation->setDuration(20000);
  concedeRect = QRectF(69, 600, 182, 34);

  setMouseTracking(true);
}

void Panel::paintEvent(QPaintEvent* ev) {
  QPainter painter(this);
  painter.drawImage(QRect(0, 0, 320, 720), background);
  switch (state) {
    case null: {
      painter.drawImage(QRect(85, 171, 151, 378), title);
      break;
    }
    case you: {
      painter.drawImage(QRect(20, 50, 280, 98), yourTurn);
      painter.drawImage(QRect(69, 600, 182, 34), concede);
      painter.setPen(timerPen);
      painter.drawArc(timerRect, 1440, static_cast<int>(timeRemaining * 288));
      break;
    }
    case opponent: {
      painter.drawImage(QRect(20, 50, 280, 98), opponentTurn);
      painter.drawImage(QRect(69, 600, 182, 34), concede);
      painter.setPen(timerPen);
      painter.drawArc(timerRect, 1440, static_cast<int>(timeRemaining * 288));
      break;
    }
    case win: {
      painter.drawImage(QRect(56, 236, 207, 248), winImage);
      break;
    }
    case lose: {
      painter.drawImage(QRect(50, 235, 220, 249), loseImage);
      break;
    }
  }

}

void Panel::yourMove() {
  if (state != win and state != lose) {
    state = you;
    animation->stop();
    animation->start();
  }
}

void Panel::opponentMove() {
  state = opponent;
  animation->stop();
  animation->start();
}

void Panel::mouseMoveEvent(QMouseEvent* ev) {
  if (state != null and concedeRect.contains(ev->localPos())) {
    setCursor(Qt::PointingHandCursor);
  } else {
    setCursor(Qt::ArrowCursor);
  }
}

void Panel::mousePressEvent(QMouseEvent* ev) {
  if (state == you and concedeRect.contains(ev->localPos())) {
    emit timeout();
  }
}

void Panel::stop() {
  state = null;
  animation->stop();
  update();
}

void Panel::showLose() {
  PLAY(lose)
  state = lose;
  qDebug() << "show lose";
  animation->stop();
  repaint();
}

void Panel::showWin() {
  PLAY(win)
  state = win;
  animation->stop();
  repaint();
}

