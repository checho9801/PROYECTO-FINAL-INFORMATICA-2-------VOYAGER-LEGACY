#include "Enemigo.h"
#include "BalaEnemiga.h"
#include <QGraphicsScene>
#include <QPixmap>
#include <QRandomGenerator>
#include "naveN3.h"
#include "barraVida.h"

Enemigo::Enemigo(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent),
    velocidad(6),
    moviendoDerecha(true)
{
    // Imagen del enemigo
    setPixmap(QPixmap(":/new/imagen/images/NaveEnemiga.png").scaled(100,100,Qt::KeepAspectRatio));

    // Posición inicial (arriba)
    int x = rand() % 500;
    setPos(x, 20);

    // Timer de disparo
    timerDisparo = new QTimer(this);
    connect(timerDisparo, &QTimer::timeout, this, &Enemigo::disparar);
    timerDisparo->start(700); // dispara cada 0,7 segundos

    // Timer de movimiento
    timerMovimiento = new QTimer(this);
    connect(timerMovimiento, &QTimer::timeout, this, &Enemigo::mover);
    timerMovimiento->start(50);

    // Timer que decide cuándo hacer dash
    timerDash = new QTimer(this);
    connect(timerDash, &QTimer::timeout, this, &Enemigo::activarDash);
    timerDash->start(2000);

    barraVida = new BarraVida(this);
    barraVida->setPos(pixmap().width()/2 - 30, -12);  // arriba de la nave
    barraVida->actualizar(vida, vidaMax);
}

void Enemigo::disparar()
{
    BalaEnemiga *b = new BalaEnemiga();
    b->setPos(x() + pixmap().width()/2 - 5, y() + pixmap().height());
    scene()->addItem(b);
}

void Enemigo::mover()
{
    if (!scene()) return;

    // Buscar al jugador
    QGraphicsItem *jugador = nullptr;

    for (auto *item : scene()->items()) {
        NaveN3 *n = dynamic_cast<NaveN3*>(item);
        if (n) {
            jugador = n;
            break;
        }
    }

    qreal nuevaX = x();

    // === 1. Tracking horizontal ===
    if (!enDash && jugador) {
        if (jugador->x() > x() + 10)
            nuevaX += velocidad;
        else if (jugador->x() < x() - 10)
            nuevaX -= velocidad;
    }

    // === 2. Dash evasivo ===
    if (enDash) {
        nuevaX += direccionDash * 12;
    }

    setPos(nuevaX, y());

    // === 3. Limites ===
    if (x() < 0) setPos(0, y());
    if (x() + pixmap().width() > scene()->width())
        setPos(scene()->width() - pixmap().width(), y());
}

void Enemigo::activarDash()
{
    int prob = QRandomGenerator::global()->bounded(0, 100);

    if (prob < 70) { // 70% de probabilidad
        enDash = true;

        // elegir dirección
        direccionDash = (QRandomGenerator::global()->bounded(0, 2) == 0) ? -1 : 1;

        // el dash dura 300ms
        QTimer::singleShot(500, [this]() {
            enDash = false;
            direccionDash = 0;
        });
    }
}

void Enemigo::recibirDanio(int dmg)
{
    vida -= dmg;
    barraVida->actualizar(vida, vidaMax);

    if (vida <= 0) {
        emit enemigoDerrotado();
        scene()->removeItem(this);
        delete this;
    }
}

int Enemigo::getVida() const
{
    return vida;
}

void Enemigo::limpiarTotal()
{
    if (timerDisparo) {
        timerDisparo->stop();
        timerDisparo->disconnect();
        timerDisparo->deleteLater();
        timerDisparo = nullptr;
    }

    if (timerMovimiento) {
        timerMovimiento->stop();
        timerMovimiento->disconnect();
        timerMovimiento->deleteLater();
        timerMovimiento = nullptr;
    }

    if (timerDash) {
        timerDash->stop();
        timerDash->disconnect();
        timerDash->deleteLater();
        timerDash = nullptr;
    }

    if (barraVida) {
        delete barraVida;
        barraVida = nullptr;
    }
}
