#include "BalaEnemiga.h"
#include "naveN3.h"
#include <QGraphicsScene>

BalaEnemiga::BalaEnemiga(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
{
    setPixmap(QPixmap(":/new/imagen/images/PE.png").scaled(50,50));

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &BalaEnemiga::mover);
    timer->start(40);

    musicPlayer = new QMediaPlayer();
    audioOutput = new QAudioOutput();

    musicPlayer->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.5);

}

void BalaEnemiga::reproducirSonidoGolpe()
{
    musicPlayer->setSource(QUrl("qrc:/new/audios/sounds/danio.mp3"));
    musicPlayer->play();
}

void BalaEnemiga::mover()
{
    setPos(x(), y() + 10);

    // Colisión con la nave del jugador
    QList<QGraphicsItem*> col = collidingItems();
    for (QGraphicsItem *item : col)
    {
        NaveN3 *n = dynamic_cast<NaveN3*>(item);
        if (n) {
            n->restarVida();
            reproducirSonidoGolpe();
            scene()->removeItem(this);
            delete this;
            return;
        }
    }

    // si sale de la pantalla
    if (y() > scene()->height()) {
        scene()->removeItem(this);
        delete this;
    }
}

void BalaEnemiga::limpiarTotal()
{
    if (timer) {
        timer->stop();
        timer->disconnect();
        timer->deleteLater();
        timer = nullptr;
    }

    if (musicPlayer) {
        musicPlayer->stop();
        musicPlayer->deleteLater();
        musicPlayer = nullptr;
    }

    if (audioOutput) {
        audioOutput->deleteLater();
        audioOutput = nullptr;
    }
}
