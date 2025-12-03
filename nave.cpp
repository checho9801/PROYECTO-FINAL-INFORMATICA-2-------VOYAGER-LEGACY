#include "nave.h"
#include "constants.h"
#include <QGraphicsScene>

Nave::Nave(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent),
    velocidad(GameConstants::NAVE_SPEED),
    vidas(GameConstants::VIDAS_INICIALES)
{
    // Cargar imagen de la nave
    QPixmap navePixmap(GameConstants::Resources::NAVE);
    setPixmap(navePixmap.scaled(
        GameConstants::NAVE_SIZE,
        GameConstants::NAVE_SIZE,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));

    // Posición inicial
    setPos(GameConstants::NAVE_INITIAL_X, GameConstants::NAVE_INITIAL_Y);

    // Permitir que reciba eventos de teclado
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
}

void Nave::restarVida()
{
    if (vidas > 0) {
        vidas--;
        emit colisionConMeteorito();
        emit vidasCambiadas(vidas);

        if (vidas <= 0) {
            emit naveDestruida();
        }
    }
}

int Nave::getVidas() const
{
    return vidas;
}

void Nave::reiniciar()
{
    vidas = GameConstants::VIDAS_INICIALES;
    setPos(GameConstants::NAVE_INITIAL_X, GameConstants::NAVE_INITIAL_Y);
    emit vidasCambiadas(vidas);
}

bool Nave::esMovimientoValido(qreal newX, qreal newY) const
{
    if (!scene()) return false;

    return (newX >= 0 &&
            newX + pixmap().width() <= scene()->width() &&
            newY >= 0 &&
            newY + pixmap().height() <= scene()->height());
}

void Nave::keyPressEvent(QKeyEvent *event)
{
    qreal newX = x();
    qreal newY = y();

    switch (event->key()) {
    case Qt::Key_A:
    case Qt::Key_Left:
        newX = x() - velocidad;
        break;

    case Qt::Key_D:
    case Qt::Key_Right:
        newX = x() + velocidad;
        break;

    case Qt::Key_W:
    case Qt::Key_Up:
        newY = y() - velocidad;
        break;

    case Qt::Key_S:
    case Qt::Key_Down:
        newY = y() + velocidad;
        break;

    default:
        QGraphicsPixmapItem::keyPressEvent(event);
        return;
    }

    // Solo mover si la posición es válida
    if (esMovimientoValido(newX, newY)) {
        setPos(newX, newY);
    }
}
