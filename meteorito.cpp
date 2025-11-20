#include "meteorito.h"
#include <QGraphicsScene>
#include <cstdlib>

Meteorito::Meteorito(Nave *naveRef, QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent),
    velocidad(10),  // píxeles por frame
    nave(naveRef)
{
    // Imagen del meteorito
    setPixmap(QPixmap(":/imagen/spriteMe.png").scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Posición inicial: arriba en una X aleatoria
    int x = rand() % 500; // ancho aproximado de la escena
    setPos(x, 0);

    // Timer para mover el meteorito
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Meteorito::mover);
    timer->start(50); // cada 50 ms
}

void Meteorito::mover()
{
    setPos(x(), y() + velocidad);

    // 🚨 Verificar colisión directamente con la nave
    if (nave && collidesWithItem(nave)) {
        nave->restarVida();

        scene()->removeItem(this);
        delete this;
        return;
    }

    // Si sale de la escena, lo eliminamos
    if (y() > scene()->height()) {
        scene()->removeItem(this);
        delete this;
    }
}
