//
// Created by Yixiang Tu on 07/09/2018.
//

#ifndef CHINESE_CHESS_SERVER_H
#define CHINESE_CHESS_SERVER_H


#include <QTcpServer>

class Server : public QTcpServer {

Q_OBJECT

signals:

  void newSocket(qintptr);

public:
  Server() : QTcpServer() {}

protected:
  void incomingConnection(qintptr socketDescriptor) override {
    emit newSocket(socketDescriptor);
  }


};


#endif //CHINESE_CHESS_SERVER_H
