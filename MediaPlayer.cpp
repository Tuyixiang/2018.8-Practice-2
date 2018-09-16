//
// Created by Yixiang Tu on 08/09/2018.
//

#include "MediaPlayer.h"
#include <QMediaPlaylist>

MediaPlayer::MediaPlayer() {

  for (auto& player : players) {
    player = new QMediaPlayer();
    player->setVolume(50);
  }
  players[0]->setMedia(QUrl("qrc:/res/check.wav"));
  players[1]->setMedia(QUrl("qrc:/res/checked.wav"));
  players[2]->setMedia(QUrl("qrc:/res/kill.wav"));
  players[3]->setMedia(QUrl("qrc:/res/killed.wav"));
  players[4]->setMedia(QUrl("qrc:/res/lose.wav"));
  players[5]->setMedia(QUrl("qrc:/res/move.wav"));
  players[6]->setMedia(QUrl("qrc:/res/win.wav"));
}
