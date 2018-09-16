//
// Created by Yixiang Tu on 07/09/2018.
//

#ifndef CHINESE_CHESS_PANEL_H
#define CHINESE_CHESS_PANEL_H


#include <QWidget>
#include <QLabel>
#include <QLCDNumber>
#include <QPushButton>
#include <QPen>
#include <QPropertyAnimation>

class Panel : public QWidget {

Q_OBJECT
  Q_PROPERTY(qreal timeRemaining
               WRITE
               setTimeRemaining)

  friend class MainWindow;

signals:

  void timeout();

public slots:

  void yourMove();

  void opponentMove();

  void stop();

  void showWin();

  void showLose();

public:
  Panel(QWidget* parent);

  enum State {
    null,
    you,
    opponent,
    win,
    lose
  } state = null;

  qreal timeRemaining = 20;

protected:
  void setTimeRemaining(qreal t) {
    timeRemaining = t;
    update();
    if (state == you and t == 0) {
      emit timeout();
    }
  }

  void paintEvent(QPaintEvent* ev) override;

  void mouseMoveEvent(QMouseEvent* ev) override;

  void mousePressEvent(QMouseEvent* ev) override;

  QLCDNumber* timeDisplay = nullptr;
  QImage background;
  QImage yourTurn;
  QImage opponentTurn;
  QImage concede;
  QImage title;
  QImage winImage;
  QImage loseImage;
  QPen timerPen;
  QRectF timerRect;
  QRectF concedeRect;

  QPropertyAnimation* animation = nullptr;

};


#endif //CHINESE_CHESS_PANEL_H
