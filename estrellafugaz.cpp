#include "estrellafugaz.h"
#include "constants.h"
#include <QGraphicsScene>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsColorizeEffect>
#include <cstdlib>
#include <ctime>
#include <QDebug>

EstrellaFugaz::EstrellaFugaz(Nave *naveRef, TipoColision tipo, QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent),
    velocidad(8),  // Más rápida que meteoritos
    velocidadX(0),
    nave(naveRef),
    tipoColision(tipo),
    pegadaANave(false),
    haColisionado(false)
{
    static bool randomInitialized = false;
    if (!randomInitialized) {
        srand(static_cast<unsigned>(time(nullptr)));
        randomInitialized = true;
    }

    // Crear gráfico de estrella (círculo brillante)
    QPixmap estrella(60, 60);
    estrella.fill(Qt::transparent);

    QPainter painter(&estrella);
    painter.setRenderHint(QPainter::Antialiasing);

    // Dibujar estrella brillante
    QRadialGradient gradient(30, 30, 30);

    switch (tipoColision) {
    case Elastica:
        gradient.setColorAt(0, QColor(0, 255, 100, 255));    // Verde brillante
        gradient.setColorAt(0.5, QColor(0, 200, 50, 180));
        gradient.setColorAt(1, QColor(0, 100, 0, 0));
        break;
    case Inelastica:
        gradient.setColorAt(0, QColor(255, 255, 0, 255));    // Amarillo brillante
        gradient.setColorAt(0.5, QColor(255, 200, 0, 180));
        gradient.setColorAt(1, QColor(200, 100, 0, 0));
        break;
    case PerfectamenteInelastica:
        gradient.setColorAt(0, QColor(255, 100, 255, 255));  // Magenta brillante
        gradient.setColorAt(0.5, QColor(255, 50, 200, 180));
        gradient.setColorAt(1, QColor(150, 0, 100, 0));
        break;
    }

    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(10, 10, 40, 40);

    setPixmap(estrella);

    // Aplicar efecto de brillo
    aplicarEfectoVisual();

    // Posición inicial
    setPos(generarPosicionXAleatoria(), -60);

    // Velocidad horizontal aleatoria
    velocidadX = (rand() % 7) - 3; // Entre -3 y 3

    // Timer de movimiento
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &EstrellaFugaz::mover);
    timer->start(30); // Más fluido

    // Timer de despegue
    timerDespegue = new QTimer(this);
    timerDespegue->setSingleShot(true);
    connect(timerDespegue, &QTimer::timeout, this, &EstrellaFugaz::despegar);
}

EstrellaFugaz::~EstrellaFugaz()
{
    if (timer) timer->stop();
    if (timerDespegue) timerDespegue->stop();
}

int EstrellaFugaz::generarPosicionXAleatoria() const
{
    int maxX = GameConstants::SCENE_WIDTH - 60;
    return rand() % maxX;
}

bool EstrellaFugaz::verificarColision() const
{
    return nave && collidesWithItem(nave);
}

bool EstrellaFugaz::estaDePantalla() const
{
    return scene() && (y() > scene()->height() || x() < -60 || x() > scene()->width());
}

void EstrellaFugaz::aplicarEfectoVisual()
{
    QGraphicsDropShadowEffect *efecto = new QGraphicsDropShadowEffect();
    efecto->setBlurRadius(30);
    efecto->setOffset(0, 0);

    switch (tipoColision) {
    case Elastica:
        efecto->setColor(QColor(0, 255, 100, 200));
        break;
    case Inelastica:
        efecto->setColor(QColor(255, 255, 0, 200));
        break;
    case PerfectamenteInelastica:
        efecto->setColor(QColor(255, 100, 255, 200));
        break;
    }

    setGraphicsEffect(efecto);
}

void EstrellaFugaz::aplicarColision()
{
    if (!nave) return;

    // Emitir señal (NO causa daño)
    emit colisionSinDanio(tipoColision);

    QString tipoStr;
    switch (tipoColision) {
    case Elastica:
        tipoStr = "Elástica";
        velocidad = -velocidad * COEF_ELASTICA;
        velocidadX = -velocidadX * COEF_ELASTICA;
        setPos(x() + velocidadX * 4, y() - abs(velocidad) * 4);

        QTimer::singleShot(800, this, [this]() {
            haColisionado = false;
        });
        break;

    case Inelastica:
        tipoStr = "Inelástica";
        velocidad = -velocidad * COEF_INELASTICA;
        velocidadX = velocidadX * COEF_INELASTICA;
        setPos(x() + velocidadX * 3, y() - abs(velocidad) * 3);

        QTimer::singleShot(800, this, [this]() {
            haColisionado = false;
        });
        break;

    case PerfectamenteInelastica:
        tipoStr = "Perfectamente Inelástica";
        pegadaANave = true;
        offsetPegado = pos() - nave->pos();
        velocidad = 0;
        velocidadX = 0;
        timerDespegue->start(TIEMPO_PEGADO_MS);
        break;
    }

    qDebug() << "⭐ Colisión" << tipoStr << "- ¡SIN DAÑO!";
}

void EstrellaFugaz::despegar()
{
    if (!pegadaANave) return;

    pegadaANave = false;
    haColisionado = false;

    velocidad = 5;
    velocidadX = (rand() % 7) - 3;

    qDebug() << "⭐ Estrella despegada";
}

void EstrellaFugaz::mover()
{
    if (pegadaANave && nave) {
        setPos(nave->pos() + offsetPegado);
        return;
    }

    setPos(x() + velocidadX, y() + velocidad);

    if (!haColisionado && verificarColision()) {
        haColisionado = true;
        aplicarColision();
        return;
    }

    if (estaDePantalla()) {
        emit salioDePantalla();
        if (scene()) scene->removeItem(this);
        deleteLater();
    }
}
