#include "BarraVida.h"
#include <QPen>
#include <QBrush>

BarraVida::BarraVida(QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
{
    // Marco de la barra
    setRect(0, 0, 60, 8);
    setPen(QPen(Qt::black));
    setBrush(Qt::NoBrush);

    // Parte verde (relleno)
    relleno = new QGraphicsRectItem(this);
    relleno->setBrush(QBrush(Qt::green));
    relleno->setRect(0, 0, 60, 8);
}

void BarraVida::actualizar(int vida, int vidaMax)
{
    if (vida < 0) vida = 0;
    qreal porcentaje = (qreal)vida / vidaMax;

    relleno->setRect(0, 0, 60 * porcentaje, 8);

    if (porcentaje > 0.6)
        relleno->setBrush(QBrush(Qt::green));
    else if (porcentaje > 0.3)
        relleno->setBrush(QBrush(Qt::yellow));
    else
        relleno->setBrush(QBrush(Qt::red));
}

void BarraVida::limpiarTotal() {
    if (parentItem()) {
        delete relleno;
        relleno = nullptr;
    }
}
