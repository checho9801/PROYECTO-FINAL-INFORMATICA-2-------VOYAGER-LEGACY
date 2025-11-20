#include "nave.h"
#include <QGraphicsScene>

Nave::Nave(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent),
    velocidad(13) // píxeles por movimiento
{
    // Cargar imagen de la nave
    setPixmap(QPixmap(":/imagen/voyager.png").scaled(100, 100, Qt::KeepAspectRatio,Qt::SmoothTransformation));

    // Posición inicial
    setPos(260, 506);

    // Permitir que reciba eventos de teclado
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();

    vidas = 3;
}

void Nave::restarVida()
{
    vidas--;
}

int Nave::getVidas() const
{
    return vidas;
}

void Nave::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_A:
        if (x() > 0)
            setPos(x() - velocidad, y());
        break;
    case Qt::Key_D:
        if (x() + pixmap().width() < scene()->width())
            setPos(x() + velocidad, y());
        break;
    case Qt::Key_W:
        if (y() > 0)
            setPos(x(), y() - velocidad);
        break;
    case Qt::Key_S:
        if (y() + pixmap().height() < scene()->height())
            setPos(x(), y() + velocidad);
        break;
    default:
        break;
    }
}
