//
// Created by Yixiang Tu on 04/09/2018.
//

#ifndef CHINESE_CHESS_MAINWINDOW_H
#define CHINESE_CHESS_MAINWINDOW_H

#include <QDebug>
#include <QMainWindow>
#include <QOpenGLFunctions>
#include <QThread>
#include <QMessageBox>

class BoardDisplayWindow;

class ConnectionThread;

class Panel;

class MainWindow : public QMainWindow {

Q_OBJECT

public:
  MainWindow();

signals:

  void gameReady();

  void wakeThread();

protected slots:

  void startHost();

  void startClient();

  void requireGameReady(const QString& address = QString(), const QString& message = QString(),
                        ConnectionThread* ct = nullptr);

  void gameStart(bool first);

  void showTimeout();

  void disconnected();

  void receiveMove(int x1, int y1, int x2, int y2);

  void win();

  void lose();

  void loadGame(const QByteArray& data);

protected:
  bool connected = false;

  ConnectionThread* connectionThread = nullptr;

  BoardDisplayWindow* boardDisplayWindow;

  Panel* panel;

  enum Status {
    successful,
    userCanceled,
    failed,
    error,
    timeout
  };

};


#endif //CHINESE_CHESS_MAINWINDOW_H
