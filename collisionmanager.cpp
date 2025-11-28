#include "collisionmanager.h"
#include "nave.h"
#include "meteorito.h"
#include "gema.h"
#include <QGraphicsPixmapItem>
#include <QtMath>
#include <QDebug>

CollisionManager::CollisionManager(QObject *parent)
    : QObject(parent)
{
}

bool CollisionManager::hayColision(QGraphicsItem *item1, QGraphicsItem *item2) const
{
    if (!item1 || !item2) {
        return false;
    }

    // Obtener los rectángulos de colisión con margen reducido
    QRectF rect1 = obtenerAreaColision(item1);
    QRectF rect2 = obtenerAreaColision(item2);

    // Verificar intersección
    return rect1.intersects(rect2);
}

QList<Meteorito*> CollisionManager::verificarColisionesNave(Nave *nave, const QList<QGraphicsItem*> &items)
{
    QList<Meteorito*> meteoritosColisionados;

    if (!nave) {
        return meteoritosColisionados;
    }

    // Buscar meteoritos que colisionen con la nave
    for (QGraphicsItem *item : items) {
        Meteorito *meteorito = dynamic_cast<Meteorito*>(item);

        if (meteorito && hayColision(nave, meteorito)) {
            meteoritosColisionados.append(meteorito);
            emit colisionDetectada(NaveMeteorito);
            emit naveGolpeada(meteorito);

            qDebug() << "Colisión detectada: Nave <-> Meteorito";
        }
    }

    return meteoritosColisionados;
}

// Método para verificar colisiones con gemas
QList<Gema*> CollisionManager::verificarColisionesGemas(Nave *nave, const QList<QGraphicsItem*> &items)
{
    QList<Gema*> gemasRecolectadas;

    if (!nave) {
        return gemasRecolectadas;
    }

    // Buscar gemas que colisionen con la nave
    for (QGraphicsItem *item : items) {
        Gema *gema = dynamic_cast<Gema*>(item);

        if (gema && hayColision(nave, gema)) {
            gemasRecolectadas.append(gema);
            emit colisionDetectada(NaveGema);
            emit gemaRecolectada(gema);

            qDebug() << "¡Gema recolectada!";
        }
    }

    return gemasRecolectadas;
}

QRectF CollisionManager::obtenerAreaColision(QGraphicsItem *item) const
{
    if (!item) {
        return QRectF();
    }

    // Obtener el rectángulo del item
    QRectF rect = item->boundingRect();

    // Aplicar transformación de posición
    rect.translate(item->pos());

    // Reducir el área con el margen para colisiones más precisas
    rect.adjust(MARGEN_COLISION, MARGEN_COLISION,
                -MARGEN_COLISION, -MARGEN_COLISION);

    return rect;
}

qreal CollisionManager::calcularDistancia(QGraphicsItem *item1, QGraphicsItem *item2) const
{
    if (!item1 || !item2) {
        return -1.0;
    }

    QPointF pos1 = item1->pos() + item1->boundingRect().center();
    QPointF pos2 = item2->pos() + item2->boundingRect().center();

    qreal dx = pos2.x() - pos1.x();
    qreal dy = pos2.y() - pos1.y();

    return qSqrt(dx * dx + dy * dy);
}
