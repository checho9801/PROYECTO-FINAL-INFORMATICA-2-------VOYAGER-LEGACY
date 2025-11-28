#include "meteorito.h"
#include "constants.h"
#include <QGraphicsScene>
#include <cstdlib>
#include <ctime>

Meteorito::Meteorito(Nave *naveRef, QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent),
    velocidad(GameConstants::METEORITO_SPEED),
    nave(naveRef)
{
    // Inicializar generador aleatorio (solo una vez)
    static bool randomInitialized = false;
    if (!randomInitialized) {
        srand(static_cast<unsigned>(time(nullptr)));
        randomInitialized = true;
    }

    // Cargar imagen del meteorito
    QPixmap meteoritoPixmap(GameConstants::Resources::METEORITO);
    setPixmap(meteoritoPixmap.scaled(
        GameConstants::METEORITO_SIZE,
        GameConstants::METEORITO_SIZE,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));

    // Posición inicial: arriba en una X aleatoria
    setPos(generarPosicionXAleatoria(), 0);

    // Timer para mover el meteorito
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Meteorito::mover);
    timer->start(GameConstants::METEORITO_MOVE_MS);
}

Meteorito::~Meteorito()
{
    if (timer) {
        timer->stop();
    }
}

void Meteorito::setVelocidad(int nuevaVelocidad)
{
    if (nuevaVelocidad > 0) {
        velocidad = nuevaVelocidad;
    }
}

int Meteorito::getVelocidad() const
{
    return velocidad;
}

int Meteorito::generarPosicionXAleatoria() const
{
    // Generar posición X dentro de los límites de la escena
    int maxX = GameConstants::SCENE_WIDTH - GameConstants::METEORITO_SIZE;
    return rand() % maxX;
}

bool Meteorito::verificarColision() const
{
    return nave && collidesWithItem(nave);
}

bool Meteorito::estaDePantalla() const
{
    return scene() && y() > scene()->height();
}

void Meteorito::mover()
{
    // Actualizar posición
    setPos(x(), y() + velocidad);

    // Verificar colisión con la nave
    if (verificarColision()) {
        nave->restarVida();
        emit colisionConNave();

        // Eliminar el meteorito
        if (scene()) {
            scene()->removeItem(this);
        }
        deleteLater();
        return;
    }

    // Verificar si salió de la pantalla
    if (estaDePantalla()) {
        emit salioDePantalla();

        if (scene()) {
            scene()->removeItem(this);
        }
        deleteLater();
    }
}
