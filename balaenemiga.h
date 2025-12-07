#ifndef BALA_ENEMIGA_H
#define BALA_ENEMIGA_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>

class BalaEnemiga : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    BalaEnemiga(QGraphicsItem *parent = nullptr);

    void limpiarTotal();

private slots:
    void mover();

private:
    QTimer *timer;
    QMediaPlayer *musicPlayer;
    QAudioOutput *audioOutput;

    void reproducirSonidoGolpe();
};

#endif // BALA_ENEMIGA_H
