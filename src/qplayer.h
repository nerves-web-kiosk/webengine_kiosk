#ifndef QPLAYER_MULTIMEDIA_H
#define QPLAYER_MULTIMEDIA_H

#include <QtCore>
#include <QtMultimedia/QMediaPlayer>

class QPlayer : public QObject
{
    Q_OBJECT

public:
    explicit QPlayer(QObject *parent = nullptr);

    void play(const QUrl &soundFile);

public slots:
    void player_error(QMediaPlayer::Error error);

private:
    QMediaPlayer *player;
};

#endif // QPLAYER_MULTIMEDIA_H
