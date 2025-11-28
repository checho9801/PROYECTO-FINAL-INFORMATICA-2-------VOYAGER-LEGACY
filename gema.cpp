#include "gema.h"
#include "nave.h"
#include "constants.h"
#include <QGraphicsScene>
#include <QGraphicsEffect>
#include <cstdlib>
#include <ctime>

Gema::Gema(Nave *naveRef, QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent),
    velocidad(5),  // Más lenta que los meteoritos
    valor(10),     // Puntos que otorga
    nave(naveRef)
{
    // Inicializar generador aleatorio
    static bool randomInitialized = false;
    if (!randomInitialized) {
        srand(static_cast<unsigned>(time(nullptr)));
        randomInitialized = true;
    }

    // Cargar imagen de la gema
    QPixmap gemaPixmap(GameConstants::Resources::GEMA);
    setPixmap(gemaPixmap.scaled(
        GameConstants::GEMA_SIZE,
        GameConstants::GEMA_SIZE,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));

    // Posición inicial: arriba en una X aleatoria
    setPos(generarPosicionXAleatoria(), -GameConstants::GEMA_SIZE);

    // Aplicar efecto visual
    aplicarEfectoVisual();

    // Timer para mover la gema
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Gema::mover);
    timer->start(50); // Actualizar cada 50 ms
}

Gema::~Gema()
{
    if (timer) {
        timer->stop();
    }
}

void Gema::setVelocidad(int nuevaVelocidad)
{
    if (nuevaVelocidad > 0) {
        velocidad = nuevaVelocidad;
    }
}

int Gema::getVelocidad() const
{
    return velocidad;
}

int Gema::getValor() const
{
    return valor;
}

int Gema::generarPosicionXAleatoria() const
{
    int maxX = GameConstants::SCENE_WIDTH - GameConstants::GEMA_SIZE;
    return rand() % maxX;
}

bool Gema::verificarRecoleccion() const
{
    return nave && collidesWithItem(nave);
}

bool Gema::estaDePantalla() const
{
    return scene() && y() > scene()->height();
}

void Gema::aplicarEfectoVisual()
{
    // Agregar un efecto de brillo sutil
    QGraphicsDropShadowEffect *efecto = new QGraphicsDropShadowEffect();
    efecto->setBlurRadius(15);
    efecto->setColor(QColor(255, 215, 0, 180)); // Dorado
    efecto->setOffset(0, 0);
    setGraphicsEffect(efecto);
}

void Gema::mover()
{
    // Actualizar posición
    setPos(x(), y() + velocidad);

    // Verificar recolección
    if (verificarRecoleccion()) {
        emit gemaRecolectada();

        // Eliminar la gema
        if (scene()) {
            scene()->removeItem(this);
        }
        deleteLater();
        return;
    }

    // Verificar si salió de la pantalla
    if (estaDePantalla()) {
        emit gemaPerdida();

        if (scene()) {
            scene()->removeItem(this);
        }
        deleteLater();
    }
}
