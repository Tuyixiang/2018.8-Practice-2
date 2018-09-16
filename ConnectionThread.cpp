//
// Created by Yixiang Tu on 06/09/2018.
//

#include <QTimer>
#include <chrono>
#include <QThreadPool>
#include <QMessageBox>
#include <QProgressDialog>
#include <QRandomGenerator>
#include "Server.h"
#include "ConnectionThread.h"
#include "Game.h"

using namespace std::chrono;

/**
 *  Procedures:
 *  1. HOST start listening
 *  2. CLIENT connect to port
 *  3. HOST found connection and send version message
 *  4. CLIENT verify message and send version message and personal info
 *      CLIENT may then choose to ready for game
 *  5. HOST verify message
 *      HOST may then choose to ready for game
 */

QVector<ConnectionThread*> threadList;

void ConnectionThread::run() {

  connect(this, &QThread::finished, this, &QObject::deleteLater);

  switch (script) {
    case hostSetup: {
      setupHost();
      break;
    }

    case hostConfirmConnection: {
      verifyClientConnection();
      break;
    }
    case clientSetup: {
      setupClient();
      break;
    }
  }
}

void ConnectionThread::startPhase(ConnectionThread::Script p) {
  script = p;
  start();
}

void ConnectionThread::startPhase(ConnectionThread::Script p, const QString& address, quint16 port) {
  script = p;
  _address = address;
  _port = port;
  start();
}

void ConnectionThread::setupHost() {

  eventLoop = new QEventLoop();
  connect(this, &QThread::finished, [this]() {
    if (eventLoop) {
      eventLoop->deleteLater();
      eventLoop = nullptr;
    }
  });

  /// create server and start listening
  server = new Server();
  connect(this, &QThread::finished, [this]() mutable {
    server->close();
    server->deleteLater();
  });
  if (!server->listen(QHostAddress(_address), _port)) {
    qDebug() << "Unable to listen on that port";
    emit error("Unable to listen on that port");
    return;
  }
  qDebug() << tr("Listening on %1:%2").arg(_address).arg(_port);

  /// set a 10min timeout
  auto timer = new QTimer();
  timer->setSingleShot(true);
  connect(timer, &QTimer::timeout, [this]() {
    emit timeoutSignal();
    terminate();
  });
  connect(this, &QThread::finished, timer, &QObject::deleteLater);
  timer->start(600000);

  /// upon incoming connection
  connect(server, &Server::newSocket, [this](qintptr socketDescriptor) {
    /// start a new thread to handle
    qDebug() << "host received a new connection";
    auto newThread = new ConnectionThread();
    threadList.push_back(newThread);
    newThread->startPhase(hostConfirmConnection, socketDescriptor);
    connect(newThread, &ConnectionThread::clientFound, [this, newThread](const QString& a, const QString& m) mutable {
      emit clientConnected(a, m, newThread);
      for (auto t: threadList) {
        if (t != newThread) {
          t->terminate();
        }
      }
      disconnect(newThread, nullptr, this, nullptr);
      terminate();
    });
    connect(newThread, &QThread::finished, [newThread]() {
      threadList.removeAll(newThread);
    });
  });

  /// hold until a valid connection
  qDebug() << "waiting for a valid connection";
  eventLoop->exec(QEventLoop::WaitForMoreEvents);


  qDebug() << "eventLoop quited, found =" << successful;
}

void ConnectionThread::verifyClientConnection() {
  /// construct socket of incoming connection
  _socket = new QTcpSocket();
  _socket->setSocketDescriptor(_socketDescriptor);
  connect(this, &QThread::finished, _socket, &QTcpSocket::disconnectFromHost);
  connect(_socket, &QTcpSocket::disconnected, [this]() {
    emit closed();
    qDebug() << "socket closed";
  });
  connect(_socket, &QTcpSocket::disconnected, _socket, &QObject::deleteLater);

  /// set a 3s timeout for connection
  auto timer = new QTimer();
  timer->setSingleShot(true);
  connect(timer, &QTimer::timeout, [this]() mutable {
    emit timeoutSignal();
    terminate();
  });
  connect(this, &ConnectionThread::clientFound, timer, &QObject::deleteLater);
  connect(this, &QThread::finished, timer, &QObject::deleteLater);
  timer->start(3000);

  /// send hostVerificationMessage to client
  if (!GAME.hasLoadGame) {
    _socket->write(hostVerificationMessage);
  } else {
    auto data = GAME.dumpMirroredData();
    QByteArray header("\0\0\0\0", 4);
    header[2] = (data.size() + 4) / 256;
    header[3] = (data.size() + 4) % 256;
    auto package = header + data;
    Q_ASSERT(package.size() == data.size() + 4);
    _socket->write(package);
  }
  waitForBytesWritten(_socket);
  qDebug() << "host sent verification message";

  /// expect a reply
  QByteArray received;
  auto package = receiveData();
  if (package.header[3] == '\xAE') {
    qDebug() << "client found";
    emit clientFound(_socket->peerAddress().toString(), QString());
    hostHandleConnection();
  }
  /*while (true) {
    qDebug() << "host waiting for message";
    waitForReadyRead(_socket);
    received.append(_socket->readAll());
    qDebug() << "host receive message:" << received;
    if (received.size() >= 8) {
      if (received.left(4) == "\xfa\xce\x2a\xae") {
        if (received.size() == 8 and received.right(4) == "\xfa\xce\x2a\xae") {
          qDebug() << "client found";
          emit clientFound(_socket->peerAddress().toString(), QString());
          hostHandleConnection();
          return;
        } else {
          int size = (received[4] << 24) + (received[5] << 16) + (received[6] << 8) + received[7];
          if (size <= 0 or size >= 256) {
            break;
          }
          received.remove(0, 8);
          while (received.size() < size) {
            waitForReadyRead(_socket);
            received.append(_socket->readAll());
            qDebug() << "host receive message:" << received;
          }
          if (received.size() == size) {
            qDebug() << "client found" << received;
            emit clientFound(_socket->peerAddress().toString(), QString(received));
            hostHandleConnection();
            return;
          }
          break;
        }
      } else {
        break;
      }

    }
  }*/
}

void ConnectionThread::hostHandleConnection() {
  qDebug() << "hostHandleConnection";

  eventLoop = new QEventLoop();
  connect(this, &QThread::finished, [this]() {
    if (eventLoop) {
      eventLoop->deleteLater();
      eventLoop = nullptr;
    }
  });

  eventLoop->exec(QEventLoop::WaitForMoreEvents);

  readSyncMessage();

  qDebug() << "After read";


  if (!_socket->readAll().isEmpty()) {
    emit closed();
    terminated = true;
    terminate();
  }

  if (terminated) {
    return;
  }

  qDebug() << "send first signal";

  bool first;
  if (GAME.hasLoadGame) {
    first = GAME.turn;
  } else {
    first = QRandomGenerator::global()->bounded(2) > 0;
  }
  if (first) {
    _socket->write("\0\0\0\3I1st", 8);
  } else {
    _socket->write("\0\0\0\3U1st", 8);
  }
  waitForBytesWritten(_socket);

  qDebug() << "written";

  /// timeout 20s
  receiveData();

  qDebug() << "received";

  emit gameStart(first);

  if (first) {
    qDebug() << "enter loop";
    eventLoop->exec(QEventLoop::WaitForMoreEvents);
    qDebug() << "quit loop";
    readSyncMessage();
  }
  while (!terminated) {
    readPackage(receiveData());
    eventLoop->exec(QEventLoop::WaitForMoreEvents);
    readSyncMessage();
  }

}

void ConnectionThread::setupClient() {
  /// create connection
  _socket = new QTcpSocket();
  connect(this, &QThread::finished, _socket, &QTcpSocket::disconnectFromHost);
  connect(_socket, &QTcpSocket::disconnected, [this]() {
    emit closed();
    qDebug() << "socket closed";
  });
  connect(_socket, &QTcpSocket::disconnected, _socket, &QObject::deleteLater);
  _socket->connectToHost(_address, _port);

  /// timeout after 3s
  auto timer = new QTimer();
  timer->setSingleShot(true);
  connect(this, &ConnectionThread::hostFound, timer, &QTimer::stop);
  connect(this, &QThread::finished, timer, &QObject::deleteLater);
  connect(timer, &QTimer::timeout, [this]() mutable {
    emit timeoutSignal();
    terminate();
  });
  timer->start(3000);

  QByteArray received;
  while (true) {
    received.append(_socket->readAll());
    qDebug() << "client received message:" << received;
    if (received.size() >= 4) {
      int size = (received[0] << 24) + (received[1] << 16) + (received[2] << 8) + received[3];
      if (size <= 0 or size >= 1024) {
        break;
      }
      received.append(_socket->readAll());
      while (received.size() < size) {
        waitForReadyRead(_socket);
        received.append(_socket->readAll());
      }
      if (received.size() == size and received == hostVerificationMessage) {
        emit hostFound(QString(), QString());
        qDebug() << "host found, sending game request";
        _socket->write(QByteArray("\xfa\xce\x2a\xae\xfa\xce\x2a\xae", 8));
        waitForBytesWritten(_socket);
        clientHandleConnection();
      } else {
        received.remove(0, 4);
        emit useGameLoad(received);
        emit hostFound(QString(), QString("123"));
        qDebug() << "host found, data received, sending game request";
        _socket->write(QByteArray("\xfa\xce\x2a\xae\xfa\xce\x2a\xae", 8));
        waitForBytesWritten(_socket);
        clientHandleConnection();
      }
      break;
    }
    waitForReadyRead(_socket);
  }
}

void ConnectionThread::clientHandleConnection() {
  qDebug() << "clientHandleConnection";
  eventLoop = new QEventLoop();
  connect(this, &QThread::finished, [this]() {
    if (eventLoop) {
      eventLoop->deleteLater();
      eventLoop = nullptr;
    }
  });
  eventLoop->exec(QEventLoop::WaitForMoreEvents);

  readSyncMessage();

  qDebug() << "wait for first signal";

  /// timeout 20s

  qDebug() << "ready read";
  auto package = receiveData();

  if (!package.header[3]) {
    emit closed();
    terminate();
  }

  auto first = package.x1 == 'U';

  _socket->write("\0\0\0\3ACCE", 8);
  waitForBytesWritten(_socket);
  emit gameStart(first);

  if (first) {
    qDebug() << "enter loop";
    eventLoop->exec(QEventLoop::WaitForMoreEvents);
    qDebug() << "quit loop";
    readSyncMessage();
  }
  while (true) {
    readPackage(receiveData());
    eventLoop->exec(QEventLoop::WaitForMoreEvents);
    readSyncMessage();
  }

}

Package ConnectionThread::receiveData() {
  QByteArray received;
  received.append(_socket->readAll());
  while (received.size() < 8) {
    waitForReadyRead(_socket);
    qDebug() << "ready read";
    received.append(_socket->readAll());
    qDebug() << received << received.size();
  }
  return Package(received);
}

void ConnectionThread::readPackage(const Package& p) {
  switch (p.header[3]) {
    case 1: {
      emit moveReceived(p.x1, p.y1, p.x2, p.y2);
      break;
    }
    case 2: {
      emit opponentConcede();
      break;
    }
    case 0: {
      emit closed();
      terminate();
      break;
    }
    default: {
    }
  }
}

void ConnectionThread::readSyncMessage() {
  switch (syncMessage.type) {
    case SyncMessage::lose: {
      qDebug() << "sending lose";
      _socket->write(syncMessage.toBA());
      waitForBytesWritten(_socket);
      emit packageSent();
      break;
    }
    case SyncMessage::move: {
      qDebug() << "sending move";
      _socket->write(syncMessage.toBA());
      waitForBytesWritten(_socket);
      emit packageSent();
      if (GAME.gameStatus == 1) {
        emit youWin();
      }
      break;
    }
    case SyncMessage::terminate: {
      qDebug() << "sending terminate";
      _socket->write(syncMessage.toBA());
      waitForBytesWritten(_socket);
      terminated = true;
      terminate();
      break;
    }
    default: {
    }
  }
}
