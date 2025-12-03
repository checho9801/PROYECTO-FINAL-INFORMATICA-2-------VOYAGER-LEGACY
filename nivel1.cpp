#include "nivel1.h"
#include "meteorito.h"
#include "constants.h"
#include <QDebug>

Nivel1::Nivel1(QGraphicsScene *escena, QObject *parent)
    : NivelBase(escena, parent),
    tiempoTranscurrido(0),
    intervaloSpawn(GameConstants::METEORITO_SPAWN_MS)
{
}

Nivel1::~Nivel1()
{
    detener();
    limpiar();
}

void Nivel1::iniciar()
{
    qDebug() << "=== NIVEL 1 INICIADO ===";
    qDebug() << "Objetivo: Sobrevivir 60 segundos";

    completado = false;
    pausado = false;
    tiempoTranscurrido = 0;
    intervaloSpawn = GameConstants::METEORITO_SPAWN_MS;

    // Inicializar nave
    inicializarNave();

    // CONECTAR señal de nave destruida directamente
    if (nave) {
        connect(nave, &Nave::naveDestruida, this, [this]() {
            qDebug() << "Nivel 1: Nave destruida - Emitiendo nivelFallado";
            detener(); // Detener todos los timers
            emit nivelFallado();
        });
    }

    // Inicializar timers
    inicializarTimers();

    // Iniciar generación de meteoritos
    timerMeteorito->start(intervaloSpawn);
    timerAcelerar->start(GameConstants::ACCELERATION_MS);
    timerMeta->start(GameConstants::GOAL_TIME_MS);
}

void Nivel1::detener()
{
    qDebug() << "Nivel 1 detenido";

    if (timerMeteorito) timerMeteorito->stop();
    if (timerAcelerar) timerAcelerar->stop();
    if (timerMeta) timerMeta->stop();
}

void Nivel1::pausar()
{
    if (!pausado) {
        pausado = true;
        detener();
        qDebug() << "Nivel 1 pausado";
    }
}

void Nivel1::reanudar()
{
    if (pausado) {
        pausado = false;
        timerMeteorito->start(intervaloSpawn);
        timerAcelerar->start(GameConstants::ACCELERATION_MS);
        timerMeta->start();
        qDebug() << "Nivel 1 reanudado";
    }
}

void Nivel1::limpiar()
{
    limpiarMeteoritos();

    if (nave && scene) {
        scene->removeItem(nave);
        delete nave;
        nave = nullptr;
    }
}

void Nivel1::actualizar()
{
    // El Nivel 1 no necesita actualización constante
    // La lógica está manejada por los timers
}

bool Nivel1::estaCompletado() const
{
    return completado;
}

void Nivel1::inicializarTimers()
{
    // Timer para meteoritos
    timerMeteorito = new QTimer(this);
    connect(timerMeteorito, &QTimer::timeout, this, &Nivel1::spawnMeteorito);

    // Timer para acelerar spawn
    timerAcelerar = new QTimer(this);
    connect(timerAcelerar, &QTimer::timeout, this, &Nivel1::acelerarSpawn);

    // Timer para meta (60 segundos)
    timerMeta = new QTimer(this);
    timerMeta->setSingleShot(true); // Solo se ejecuta una vez
    connect(timerMeta, &QTimer::timeout, this, &Nivel1::verificarMeta);

    //Timer para verificar vidas
    timerVidas = new QTimer(this);
    connect(timerVidas, &QTimer::timeout, this, &Nivel1::verificarVidas);
}

void Nivel1::verificarVidas()
{
    if (nave && nave->getVidas() <= 0) {
        qDebug() << "Nave sin vidas en Nivel 1. ¡GAME OVER!";
        detener(); // Detiene todos los timers, incluyendo este
        emit nivelFallado();
    }
}

void Nivel1::spawnMeteorito()
{
    if (!pausado && scene && nave) {
        Meteorito *m = new Meteorito(nave);
        scene->addItem(m);
    }
}

void Nivel1::acelerarSpawn()
{
    if (pausado) return;

    // Reducir el intervalo poco a poco
    if (intervaloSpawn > GameConstants::METEORITO_SPAWN_MIN) {
        intervaloSpawn -= GameConstants::METEORITO_SPAWN_REDUCTION;

        // Asegurar que no baje del mínimo
        if (intervaloSpawn < GameConstants::METEORITO_SPAWN_MIN) {
            intervaloSpawn = GameConstants::METEORITO_SPAWN_MIN;
            timerAcelerar->stop(); // Detener aceleración al alcanzar el mínimo
        }

        timerMeteorito->setInterval(intervaloSpawn);
        emit intervaloSpawnCambiado(intervaloSpawn);

        qDebug() << "Dificultad aumentada. Nuevo intervalo:" << intervaloSpawn << "ms";
    }
}

void Nivel1::verificarMeta()
{
    if (nave && nave->getVidas() > 0) {
        qDebug() << "=== ¡NIVEL 1 COMPLETADO! ===";
        completado = true;
        detener();
        emit nivelCompletado();
    } else {
        qDebug() << "Tiempo alcanzado pero sin vidas";
        emit nivelFallado();
    }
}

void Nivel1::limpiarMeteoritos()
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
