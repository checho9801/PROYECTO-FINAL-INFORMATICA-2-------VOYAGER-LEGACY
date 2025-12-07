#ifndef NAVEN3_H
#define NAVEN3_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QKeyEvent>
#include "Bala.h"
#include "BarraVida.h"

class NaveN3 : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)  // ← AGREGADO para key events

public:
    NaveN3(QGraphicsItem *parent = nullptr);
    int getVidas() const;
    void activarDisparo(bool activar);
    void restarVida();
    void limpiarTotal();

signals:
    void gameOver();
    void nivelCompletado();

protected:
    void keyPressEvent(QKeyEvent *event);  // ← PROTEGIDO

private slots:
    void reproducirSonidoBala();

private:
    qreal velocidad = 13;
    int vidas = 3;
    int vidaMax = 3;
    bool puedeDisparar = true;
    BarraVida *barraVida;
    QMediaPlayer *musicPlayer;
    QAudioOutput *audioOutput;
};

#endif // NAVEN3_H
