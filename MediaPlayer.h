//
// Created by Yixiang Tu on 08/09/2018.
//

#ifndef CHINESE_CHESS_MEDIAPLAYER_H
#define CHINESE_CHESS_MEDIAPLAYER_H

#include <QMediaPlayer>

#define PLAY(x) MediaPlayer::instance().play(MediaPlayer::x);

class MediaPlayer {

public:
  enum Playlist : int {
    check = 0,
    checked = 1,
    kill = 2,
    killed = 3,
    lose = 4,
    move = 5,
    win = 6
  };

  static MediaPlayer& instance() {
    static MediaPlayer mp;
    return mp;
  }

  void play(Playlist x) {
    if (!muted) {
      players[x]->stop();
      players[x]->play();
    }
  }

  void setMute(bool m) {
    muted = m;
  }

protected:
  MediaPlayer();

  bool muted = false;

  QMediaPlayer* players[7] = {nullptr};

};


#endif //CHINESE_CHESS_MEDIAPLAYER_H
