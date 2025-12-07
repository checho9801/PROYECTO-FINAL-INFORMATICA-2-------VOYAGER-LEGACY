#include "Bala.h"
#include <QGraphicsScene>
#include <QPixmap>
#include "enemigo.h"

Bala::Bala(QGraphicsItem *parent)
    : QObject(), QGraphicsPixmapItem(parent)
{
    // Cargar sprite de la bala
    setPixmap(QPixmap(":/new/imagen/images/PA.png").scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Timer de movimiento
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Bala::mover);
    timer->start(60);  // más rápido que antes (suave)
}

void Bala::mover()
{
    // Colisiones con enemigo ---
    QList<QGraphicsItem*> col = collidingItems();
    for (QGraphicsItem *item : col)
    {
        Enemigo *e = dynamic_cast<Enemigo*>(item);
        if (e) {
            e->recibirDanio(1);  // restar vida al enemigo

            // eliminar bala
            scene()->removeItem(this);
            delete this;
            return;
        }
    }

    // Mover hacia arriba
    setPos(x(), y() - 15);

    // Si sale de escena, eliminarse
    if (y() + pixmap().height() < 0) {
        if (scene()) scene()->removeItem(this);
        deleteLater();
    }
}

void Bala::limpiarTotal()
{
    if (timer) {
        timer->stop();
        timer->disconnect();
        timer->deleteLater();
        timer = nullptr;
    }
}
