#ifndef NIVEL3_H
#define NIVEL3_H

#include "nivelbase.h"
#include "naven3.h"
#include "balaenemiga.h"
#include <QObject>
#include <QTimer>
#include <QPixmap>

class Enemigo;

class Nivel3 : public NivelBase
{
    Q_OBJECT

public:
    explicit Nivel3(QGraphicsScene *escena, QObject *parent = nullptr);
    ~Nivel3() override;

    // Métodos VIRTUALES de NivelBase ← ¡OBLIGATORIOS!
    void iniciar() override;
    void detener() override;
    void pausar() override;
    void reanudar() override;
    void actualizar() override;
    bool estaCompletado() const override;
    void limpiar() override;

private slots:
    void mostrarGameOver();
    void nivelSuperado();
    void actualizarFondo();

private:
    NaveN3 *naveN3;
    Enemigo *enemigo;
    QTimer *timerFondo;
    QPixmap fondoEscalado;
    qreal desplazamientoY = 0;
    qreal velocidadFondo = 2.0;
};

#endif // NIVEL3_H
