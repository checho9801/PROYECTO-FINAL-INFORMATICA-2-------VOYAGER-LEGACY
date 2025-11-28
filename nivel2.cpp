#include "nivel2.h"
#include "gema.h"
#include "meteorito.h"
#include "constants.h"
#include <QDebug>
#include <QLabel>

Nivel2::Nivel2(QGraphicsScene *escena, QObject *parent)
    : NivelBase(escena, parent),
    gemasRecolectadas(0),
    gemasNecesarias(GameConstants::GEMAS_PARA_GANAR_NIVEL2),
    labelGemas(nullptr)
{
    collisionManager = new CollisionManager(this);
}

Nivel2::~Nivel2()
{
    detener();
    limpiar();
}

void Nivel2::iniciar()
{
    qDebug() << "=== NIVEL 2 INICIADO ===";
    qDebug() << "Objetivo: Recolectar" << gemasNecesarias << "gemas";

    completado = false;
    pausado = false;
    gemasRecolectadas = 0;

    // Inicializar nave
    inicializarNave();

    // Inicializar HUD
    inicializarHUD();

    // Inicializar timers
    inicializarTimers();

    // Iniciar generación de enemigos y gemas
    timerMeteorito->start();
    timerGema->start();
    timerColisiones->start();
}

void Nivel2::detener()
{
    qDebug() << "Nivel 2 detenido";

    if (timerMeteorito) timerMeteorito->stop();
    if (timerGema) timerGema->stop();
    if (timerColisiones) timerColisiones->stop();
}

void Nivel2::pausar()
{
    if (!pausado) {
        pausado = true;
        detener();
        qDebug() << "Nivel 2 pausado";
    }
}

void Nivel2::reanudar()
{
    if (pausado) {
        pausado = false;
        timerMeteorito->start();
        timerGema->start();
        timerColisiones->start();
        qDebug() << "Nivel 2 reanudado";
    }
}

void Nivel2::limpiar()
{
    limpiarMeteoritos();
    limpiarGemas();

    if (labelGemas) {
        delete labelGemas;
        labelGemas = nullptr;
    }

    if (nave && scene) {
        scene->removeItem(nave);
        delete nave;
        nave = nullptr;
    }
}

void Nivel2::actualizar()
{
    // Método llamado periódicamente si es necesario
    actualizarHUD();
}

bool Nivel2::estaCompletado() const
{
    return gemasRecolectadas >= gemasNecesarias;
}

void Nivel2::inicializarTimers()
{
    // Timer para meteoritos
    timerMeteorito = new QTimer(this);
    connect(timerMeteorito, &QTimer::timeout, this, &Nivel2::spawnMeteorito);
    timerMeteorito->setInterval(GameConstants::METEORITO_SPAWN_MS);

    // Timer para gemas
    timerGema = new QTimer(this);
    connect(timerGema, &QTimer::timeout, this, &Nivel2::spawnGema);
    timerGema->setInterval(GameConstants::GEMA_SPAWN_MS);

    // Timer para colisiones
    timerColisiones = new QTimer(this);
    connect(timerColisiones, &QTimer::timeout, this, &Nivel2::verificarColisiones);
    timerColisiones->setInterval(50); // Verificar cada 50ms
}

void Nivel2::inicializarHUD()
{
    // Crear label para mostrar progreso de gemas
    labelGemas = new QLabel();
    labelGemas->setStyleSheet(
        "color: gold; "
        "font-size: 24px; "
        "font-weight: bold; "
        "background-color: rgba(0, 0, 0, 180); "
        "padding: 10px; "
        "border-radius: 8px;"
        );
    actualizarHUD();
    labelGemas->hide(); // Se mostrará desde MainWindow
}

void Nivel2::actualizarHUD()
{
    if (labelGemas) {
        labelGemas->setText(QString("💎 Gemas: %1/%2")
                                .arg(gemasRecolectadas)
                                .arg(gemasNecesarias));
    }
}

// SLOT para la verificación de Game Over inmediato
void Nivel2::verificarVidas()
{
    if (nave && nave->getVidas() <= 0) {
        qDebug() << "Nave sin vidas en Nivel 2. ¡GAME OVER!";
        detener(); // Detiene todos los timers, incluyendo el de colisiones
        emit nivelFallado();
    }
}

void Nivel2::spawnMeteorito()
{
    if (!pausado && scene && nave) {
        Meteorito *m = new Meteorito(nave);
        scene->addItem(m);
    }
}

void Nivel2::spawnGema()
{
    if (!pausado && scene && nave) {
        Gema *g = new Gema(nave);

        // Conectar señal de recolección
        connect(g, &Gema::gemaRecolectada, this, &Nivel2::onGemaRecolectada);

        scene->addItem(g);
        qDebug() << "Gema generada";
    }
}

void Nivel2::onGemaRecolectada()
{
    gemasRecolectadas++;
    qDebug() << "¡Gema recolectada!" << gemasRecolectadas << "/" << gemasNecesarias;

    emit gemaRecolectadaSignal(gemasRecolectadas);
    actualizarHUD();

    // Verificar si se completó el nivel
    if (estaCompletado()) {
        qDebug() << "=== ¡NIVEL 2 COMPLETADO! ===";
        completado = true;
        detener();
        emit nivelCompletado();
    }
}

void Nivel2::verificarColisiones()
{
    if (!nave || !scene || pausado) return;

    // Verificar colisiones con meteoritos (daño)
    QList<Meteorito*> meteoritos = collisionManager->verificarColisionesNave(nave, scene->items());

    // Verificar colisiones con gemas (recolección)
    QList<Gema*> gemas = collisionManager->verificarColisionesGemas(nave, scene->items());

    // Las gemas ya emiten su propia señal en Gema::mover()
    // pero aquí puedes agregar lógica adicional si es necesario
}

void Nivel2::limpiarMeteoritos()
{
    if (!scene) return;

    for (QGraphicsItem *item : scene->items()) {
        Meteorito *m = dynamic_cast<Meteorito*>(item);
        if (m) {
            scene->removeItem(m);
            m->deleteLater();
        }
    }
}

void Nivel2::limpiarGemas()
{
    if (!scene) return;

    for (QGraphicsItem *item : scene->items()) {
        Gema *g = dynamic_cast<Gema*>(item);
        if (g) {
            scene->removeItem(g);
            g->deleteLater();
        }
    }
}
