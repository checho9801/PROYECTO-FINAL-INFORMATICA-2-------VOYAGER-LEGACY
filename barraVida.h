#ifndef BARRAVIDA_H
#define BARRAVIDA_H

#include <QGraphicsRectItem>

class BarraVida : public QGraphicsRectItem
{
public:
    BarraVida(QGraphicsItem *parent = nullptr);

    void actualizar(int vida, int vidaMax);
    void limpiarTotal();

private:
    QGraphicsRectItem *relleno;  // barra verde
};

#endif // BARRAVIDA_H
