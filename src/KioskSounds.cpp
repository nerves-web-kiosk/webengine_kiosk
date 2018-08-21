#include "KioskSounds.h"

KioskSounds::KioskSounds(QObject *parent) :
    QObject(parent),
    player(nullptr)
{
}

void KioskSounds::play(const QUrl &soundFile)
{
    if (player == nullptr) {
        player = new QMediaPlayer(this);
        connect(player, SIGNAL(error(QMediaPlayer::Error)), SLOT(player_error(QMediaPlayer::Error)));
    }

    if (!soundFile.isEmpty()) {
        player->stop();
        player->setMedia(soundFile);
        player->play();
    }
}

void KioskSounds::player_error(QMediaPlayer::Error error)
{
    switch (error) {
    case QMediaPlayer::NoError:
    default:
        qDebug("Got unexpected value for error: %d", error);
        break;

    case QMediaPlayer::ResourceError:
        qDebug("Player error: ResourceError");
        break;

    case QMediaPlayer::FormatError:
        qDebug("Player error: FormatError");
        break;

    case QMediaPlayer::NetworkError:
        qDebug("Player error: NetworkError");
        break;

    case QMediaPlayer::AccessDeniedError:
        qDebug("Player error: AccessDeniedError");
        break;

    case QMediaPlayer::ServiceMissingError:
        qDebug("Player error: ServiceMissingError");
        break;

    case QMediaPlayer::MediaIsPlaylist:
        qDebug("Player error: MediaIsPlaylist");
        break;

    }
}
