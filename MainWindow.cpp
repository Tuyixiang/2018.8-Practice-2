//
// Created by Yixiang Tu on 04/09/2018.
//

#include "MainWindow.h"
#include "BoardDisplayWindow.h"
#include "Game.h"
#include "ConnectionThread.h"
#include "Panel.h"
#include "MediaPlayer.h"
#include <QKeyEvent>
#include <QMenuBar>
#include <QNetworkInterface>
#include <QInputDialog>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QTimer>
#include <QPushButton>

MainWindow::MainWindow() : QMainWindow() {
  setWindowTitle("Chinese Chess");

  boardDisplayWindow = new BoardDisplayWindow();

  auto boardContainer = createWindowContainer(boardDisplayWindow, this);
  boardContainer->setGeometry(0, 0, 960, 720);

  panel = new Panel(this);
  panel->setGeometry(960, 0, 320, 720);

  setFixedSize(1280, 720);

  setMouseTracking(true);

  connect(boardDisplayWindow, &BoardDisplayWindow::movePiece, &GAME, &Game::movePiece);
  connect(&GAME, &Game::movePieceEntity, boardDisplayWindow, &BoardDisplayWindow::movePieceEntity);
  connect(&GAME, &Game::destroyPieceEntity, boardDisplayWindow, &BoardDisplayWindow::destroyPieceEntity);
  connect(boardDisplayWindow, &BoardDisplayWindow::ok, &GAME, &Game::ok);
  connect(&GAME, &Game::ready, boardDisplayWindow, &BoardDisplayWindow::gameReady);

  auto gameMenu = menuBar()->addMenu("Game");
  connect(gameMenu->addAction("Start as host"), &QAction::triggered,
          this, &MainWindow::startHost);
  connect(gameMenu->addAction("Start as client"), &QAction::triggered,
          this, &MainWindow::startClient);
  connect(gameMenu->addAction("Save game"), &QAction::triggered,
          &GAME, &Game::dumpFile);
  connect(gameMenu->addAction("Start saved game"), &QAction::triggered,
          [this]() {
            GAME.readFile();
            boardDisplayWindow->readPiecePositions();
            startHost();
          });
  connect(gameMenu->addAction("test"), &QAction::triggered, [this]() {
    gameStart(true);
  });

  auto audioMenu = menuBar()->addMenu("Sound");
  auto soundAction = audioMenu->addAction("Play sound");
  soundAction->setCheckable(true);
  soundAction->setChecked(true);
  connect(soundAction, &QAction::toggled, [](bool s) {
    MediaPlayer::instance().setMute(!s);
  });

  connect(menuBar()->addMenu("Help")->addAction("Redraw"), &QAction::triggered,
          [&]() { boardDisplayWindow->initialize(); });

}

void MainWindow::startHost() {

  /// get address and port
  QString ipAddress;
  int port;
  {
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (const auto& i : ipAddressesList) {
      if (i != QHostAddress::LocalHost &&
          i.toIPv4Address()) {
        ipAddress = i.toString();
        break;
      }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
      ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    bool ok;
    bool entered;
    port = QInputDialog::getText(this, "Select a port",
                                 ipAddress + ":",
                                 QLineEdit::Normal,
                                 QString::number(QRandomGenerator::global()->bounded(32768, 65536)),
                                 &entered, Qt::WindowFlags(), Qt::ImhDigitsOnly).toInt(&ok);
    if (!entered) {
      return;
    } else if (!ok or port <= 0 or port >= 65536) {
      QMessageBox::warning(this, "Error", "Port number must be within (0, 65536)");
      return;
    }
    qDebug() << "will listen on" << ipAddress << ":" << port;
  }

  /// start connection
  connectionThread = new ConnectionThread();
  auto connectingMessage = new QMessageBox(QMessageBox::Information,
                                           "Status",
                                           "Waiting for connection...",
                                           QMessageBox::Cancel, this);
  /// user may cancel connection
  connect(connectingMessage, &QMessageBox::buttonClicked, [this, connectingMessage]() {
    connectionThread->terminate();
    connectingMessage->deleteLater();
    QMessageBox::warning(this, "Error", "Connection canceled by user");
  });

  /// timeout after 10min
  connect(connectionThread, &ConnectionThread::timeoutSignal, [this, connectingMessage]() {
    connectingMessage->hide();
    connectingMessage->deleteLater();
  });
  connect(connectionThread, &ConnectionThread::timeoutSignal, this, &MainWindow::showTimeout);

  connect(connectionThread, &ConnectionThread::clientConnected,
          [this, connectingMessage](const QString& address, const QString& message, ConnectionThread* ct) mutable {
            connectingMessage->hide();
            connectingMessage->deleteLater();
          });
  connect(connectionThread, &ConnectionThread::clientConnected, this, &MainWindow::requireGameReady);

  connectingMessage->show();

  connectionThread->startPhase(ConnectionThread::hostSetup, ipAddress, static_cast<quint16>(port));


}

void MainWindow::startClient() {

  /// ask for address and port and connectToHost()
  QString address;
  int port;
  {
    bool entered;
    auto inputs = QInputDialog::getText(this, "Enter host address and port",
                                        "xxx.xxx.xxx.xxx:xxxxx", QLineEdit::Normal, QString(),
                                        &entered).split(":");
    if (inputs.size() != 2) {
      QMessageBox::warning(this, "Error", "Invalid input");
      return;
    }
    address = inputs[0];
    auto nums = address.split(".");
    if (nums.size() != 4) {
      QMessageBox::warning(this, "Error", "Invalid input");
      return;
    }
    for (int i = 0; i < 4; ++i) {
      bool ok;
      auto num = nums[i].toInt(&ok);
      if (num < 0 or num > 255 or !ok) {
        QMessageBox::warning(this, "Error", "Invalid input");
        return;
      }
    }
    bool ok;
    port = inputs[1].toInt(&ok);
    if (!ok or port <= 0 or port > 65535) {
      QMessageBox::warning(this, "Error", "Port number must be within (0, 65536)");
      return;
    }
  }
  connectionThread = new ConnectionThread();
  //thread->setConnection(socket);

  /// show MessageBox
  auto connectingMessage = new QMessageBox(QMessageBox::Information,
                                           "Status",
                                           "Connecting to host...",
                                           QMessageBox::Cancel, this);

  /// user may cancel connection by clicking "Cancel"
  connect(connectingMessage, &QMessageBox::buttonClicked, [this, connectingMessage]() {
    connectingMessage->deleteLater();
    QMessageBox::warning(this, "Error", "Connection canceled by user");
    connectionThread->terminate();
  });

  /// timeout after 3 seconds
  connect(connectionThread, &ConnectionThread::timeoutSignal, [this, connectingMessage]() {
    connectingMessage->hide();
    connectingMessage->deleteLater();
  });
  connect(connectionThread, &ConnectionThread::timeoutSignal, this, &MainWindow::showTimeout);

  /// if connected
  connect(connectionThread, &ConnectionThread::hostFound, [this, connectingMessage]() {
    connectingMessage->hide();
    connectingMessage->deleteLater();
  });
  connect(connectionThread, SIGNAL(hostFound(
                                     const QString&, const QString&)), this, SLOT(requireGameReady(
                                                                                    const QString&, const QString&)));

  connect(connectionThread, &ConnectionThread::useGameLoad, this, &MainWindow::loadGame);

  connectingMessage->show();

  connectionThread->startPhase(ConnectionThread::clientSetup, address, static_cast<quint16>(port));

}

void MainWindow::requireGameReady(const QString& address, const QString& message, ConnectionThread* ct) {
  qDebug() << "require game ready";
  if (ct) {
    qDebug() << "host receive new ct";
    connectionThread = ct;
  }
  connect(connectionThread, &ConnectionThread::closed, this, &MainWindow::disconnected);

  connect(this, &MainWindow::wakeThread, connectionThread->eventLoop, &QEventLoop::quit);

  connect(connectionThread, &ConnectionThread::gameStart, this, &MainWindow::gameStart);

  QString text;
  if (address.isEmpty()) {
    if (message.isEmpty()) {
      text = "You've connected to host. Start game?";
    } else {
      text = "You've connected to host. The host loaded a saved game. Continue that game?";
    }
  } else if (message.isEmpty()) {
    text = tr("You've connected to a player at %1. Start game?").arg(address);
  } else {
    text = tr("You've connected to <i>%1</i>. Start game?").arg(message);
  }

  auto msg = new QMessageBox(QMessageBox::NoIcon,
                             "Game",
                             text,
                             QMessageBox::Yes | QMessageBox::No, this);
  int result = msg->exec();
  msg->deleteLater();

  if (connectionThread) {
    if (result != QMessageBox::Yes) {
      connectionThread->syncMessage = ConnectionThread::SyncMessage{ConnectionThread::SyncMessage::terminate};
    }
    qDebug() << "wake thread";
    emit wakeThread();
  }


}

void MainWindow::showTimeout() {
  QMessageBox::warning(this, "Error", "Connection timeout");
}

void MainWindow::disconnected() {
  connectionThread = nullptr;
  qDebug() << (panel->state);
  if (panel->state != Panel::win and panel->state != Panel::lose) {
    panel->stop();
  }
  boardDisplayWindow->stop();
  if (boardDisplayWindow->gameStarted and !GAME.gameStatus) {
    auto msg = new QMessageBox(QMessageBox::Warning, "Error",
                               "Disconnected from game. Would you like to save the current progress?",
                               QMessageBox::No | QMessageBox::Yes, this);
    if (msg->exec() == QMessageBox::Yes) {
      GAME.dumpFile();
    }
  } else {
    auto msg = new QMessageBox(QMessageBox::Warning, "Error",
                               "Disconnected from game.",
                               QMessageBox::Close, this);
    msg->exec();
  }
}

void MainWindow::gameStart(bool first) {
  connect(connectionThread, &ConnectionThread::moveReceived, this, &MainWindow::receiveMove);
  connect(connectionThread, &ConnectionThread::moveReceived, panel, &Panel::yourMove);
  connect(connectionThread, &ConnectionThread::youWin, this, &MainWindow::win);
  connect(&GAME, &Game::lose, this, &MainWindow::lose);
  boardDisplayWindow->startGame();
  if (first) {
    boardDisplayWindow->moveReady();
    panel->yourMove();
    GAME.turn = true;
  } else {
    panel->opponentMove();
  }
  connect(boardDisplayWindow, &BoardDisplayWindow::movePiece, [this](int x1, int y1, int x2, int y2) {
    qDebug() << "creating move syncMessage";
    connectionThread->syncMessage = ConnectionThread::SyncMessage{
      ConnectionThread::SyncMessage::move,
      static_cast<char>(x1), static_cast<char>(y1),
      static_cast<char>(x2), static_cast<char>(y2)};
    panel->opponentMove();
    emit wakeThread();
  });
  connect(panel, &Panel::timeout, [this]() {
    qDebug() << "creating lose syncMessage";
    connectionThread->syncMessage = ConnectionThread::SyncMessage{
      ConnectionThread::SyncMessage::lose};
    panel->showLose();
    boardDisplayWindow->stop();
    connect(connectionThread, &ConnectionThread::packageSent, connectionThread, &QThread::terminate);
    emit wakeThread();
  });
  connect(connectionThread, &ConnectionThread::opponentConcede, this, &MainWindow::win);
}

void MainWindow::receiveMove(int x1, int y1, int x2, int y2) {
  GAME.movePiece(x1, y1, x2, y2);
  GAME.turn = true;
}

void MainWindow::win() {
  boardDisplayWindow->stop();
  panel->showWin();
  connectionThread->terminate();
  connectionThread = nullptr;
}

void MainWindow::lose() {
  qDebug() << "lose";
  boardDisplayWindow->stop();
  panel->showLose();
  connectionThread->terminate();
  connectionThread = nullptr;
}

void MainWindow::loadGame(const QByteArray& data) {
  GAME.readData(data);
  boardDisplayWindow->readPiecePositions();
}
