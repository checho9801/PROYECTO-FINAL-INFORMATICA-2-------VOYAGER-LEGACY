#ifndef ENEMIGO_H
#define ENEMIGO_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>              // ← AGREGADO
#include <QRandomGenerator>    // ← AGREGADO
#include "BalaEnemiga.h"
#include "BarraVida.h"
#include "naveN3.h"

class Enemigo : public QObject, public QGraphicsPixmapItem  // ← QObject AGREGADO
{
    Q_OBJECT

public:
    Enemigo(QGraphicsItem *parent = nullptr);
    int getVida() const;
    void recibirDanio(int dmg);
    void limpiarTotal();

signals:
    void enemigoDerrotado();

private slots:
    void disparar();
    void mover();
    void activarDash();

private:
    qreal velocidad = 6;
    int vida = 5;
    int vidaMax = 5;
    bool enDash = false;
    bool moviendoDerecha = true;
    qreal direccionDash = 0;
    QTimer *timerDisparo;
    QTimer *timerMovimiento;
    QTimer *timerDash;
    BarraVida *barraVida;
};

#endif // ENEMIGO_H
