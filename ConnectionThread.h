//
// Created by Yixiang Tu on 06/09/2018.
//

#ifndef CHINESE_CHESS_CONNECTIONTHREAD_H
#define CHINESE_CHESS_CONNECTIONTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QTcpServer>
#include <utility>
#include <QRunnable>
#include <QEventLoop>

class Server;

struct Package {
  enum GameMessage : int {
    null = 0,
    move = 1,
    concede = 2,
    message = 3
  };
  char header[4];
  char x1;
  char y1;
  char x2;
  char y2;

  Package(const QByteArray& ba) {
    memcpy(this, ba.constData(), 8);
  }

  void print() {
    qDebug() << "Package:" << header[0] << header[1] << header[2] << header[3] << x1 << y1 << x2 << y2;
  }
};

class ConnectionThread : public QThread {

Q_OBJECT
public:

  struct SyncMessage {
    enum Type {
      terminate,
      move,
      lose,
      win, null
    } type = null;
    char x1, y1, x2, y2;

    SyncMessage() = default;

    SyncMessage(Type t) {
      type = t;
    };

    SyncMessage(Type t, char a, char b, char c, char d) {
      type = t;
      x1 = a;
      y1 = b;
      x2 = c;
      y2 = d;
    };

    QByteArray toBA() {
      QByteArray package(8, '\0');
      switch (type) {
        case move: {
          package[3] = '\1';
          package[4] = 8 - x1;
          package[5] = 9 - y1;
          package[6] = 8 - x2;
          package[7] = 9 - y2;
          break;
        }
        case lose: {
          package[3] = '\2';
          break;
        }
        default: {
          break;
        }
      }
      return package;
    }
  } syncMessage;

  enum Script {
    hostSetup,
    hostConfirmConnection,
    clientSetup
  } script;

public:
  ConnectionThread(QObject* parent = nullptr) : QThread(parent) {}

signals:

  void moveReceived(int, int, int, int);

  void clientFound(const QString&, const QString&);

  void hostFound(const QString&, const QString&);

  void clientConnected(const QString&, const QString&, ConnectionThread*);

  void closed();

  void error(const QString&);

  void timeoutSignal();

  void finished();

  void userCancelled();

  void gameStart(bool);

  void opponentConcede();

  void packageSent();

  void youWin();

  void useGameLoad(const QByteArray&);

public slots:

  void startPhase(Script p);

  void startPhase(Script p, const QString& address, quint16 port);

  void startPhase(Script p, qintptr socketDescriptor) {
    script = p;
    _socketDescriptor = socketDescriptor;
    start();
  }

  void run();

  void sendTerminate() {
    _socket->write(QByteArray("\0\0\0\0\0\0\0\0", 8));
    waitForBytesWritten(_socket);
    terminate();
  }

public:
  QEventLoop* eventLoop = nullptr;

  bool terminated = false;

protected:
  void setupHost();

  void verifyClientConnection();

  void hostHandleConnection();

  void setupClient();

  void clientHandleConnection();

  void readPackage(const Package& p);

  void readSyncMessage();

  void waitForReadyRead(QTcpSocket* socket) {
    QEventLoop loop;
    connect(socket, &QTcpSocket::readyRead, &loop, &QEventLoop::quit);
    loop.exec(QEventLoop::WaitForMoreEvents);
  }

  void waitForBytesWritten(QTcpSocket* socket) {
    QEventLoop loop;
    connect(socket, &QTcpSocket::bytesWritten, &loop, &QEventLoop::quit);
    loop.exec(QEventLoop::WaitForMoreEvents);
  }

  Package receiveData();

  QTcpSocket* _socket = nullptr;
  Server* server = nullptr;

  QString _address;
  quint16 _port;
  qintptr _socketDescriptor;
  QString _message;


  QByteArray hostVerificationMessage = QByteArray("\0\0\1\0chinese chess host\n"
                                                    "package size: 8 bytes\n"
                                                    "uint32 header:\n"
                                                    "  0: terminate\n"
                                                    "  1: move\n"
                                                    "  2: concede\n"
                                                    "  3: message\n"
                                                    "  0xFACE2AAE: request\n"
                                                    "four uint8 represent a move:\n"
                                                    "  x1, y1, x2, y2, using board bottom-left as zero\n"
                                                    "move timeout: 60s\n"
                                                    "send \"request + request\" to join\n", 256);

  //QTimer* timer = nullptr;
  bool successful = false;

};


#endif //CHINESE_CHESS_CONNECTIONTHREAD_H
