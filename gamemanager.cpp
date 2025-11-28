#include "gamemanager.h"
#include <QDebug>

GameManager::GameManager(QObject *parent)
    : QObject(parent),
    estado(Menu), // El juego comienza en estado Menu por defecto
    puntuacion(0),
    tiempoTranscurrido(0),
    dificultad(1)
{
    // Timer para contar tiempo de juego
    timerTiempo = new QTimer(this);
    connect(timerTiempo, &QTimer::timeout, this, &GameManager::actualizarTiempo);
}

void GameManager::iniciarJuego()
{
    // Esta funcion es llamada al presionar 'Comenzar el juego' desde el Menu
    qDebug() << "Iniciando juego...";
    puntuacion = 0;
    tiempoTranscurrido = 0;
    dificultad = 1;

    cambiarEstado(Playing);
    timerTiempo->start(1000); // Actualizar cada segundo

    emit puntuacionCambiada(puntuacion);
    emit tiempoActualizado(tiempoTranscurrido);
    emit dificultadCambiada(dificultad);
}

void GameManager::pausarJuego()
{
    if (estado == Playing) {
        qDebug() << "Juego pausado";
        timerTiempo->stop();
        cambiarEstado(Paused);
    }
}

void GameManager::reanudarJuego()
{
    if (estado == Paused) {
        qDebug() << "Juego reanudado";
        timerTiempo->start(1000);
        cambiarEstado(Playing);
    }
}

void GameManager::terminarJuego()
{
    qDebug() << "Game Over - Puntuación final:" << puntuacion;
    timerTiempo->stop();
    cambiarEstado(GameOver);
}

void GameManager::ganarJuego()
{
    qDebug() << "¡Victoria! - Puntuación final:" << puntuacion;
    timerTiempo->stop();
    cambiarEstado(Victory);
}

void GameManager::reiniciarJuego()
{
    qDebug() << "Reiniciando juego...";
    iniciarJuego();
}

void GameManager::agregarPuntos(int puntos)
{
    if (estado == Playing) {
        puntuacion += puntos;
        emit puntuacionCambiada(puntuacion);
        qDebug() << "Puntuación:" << puntuacion;
    }
}

void GameManager::aumentarDificultad()
{
    if (estado == Playing) {
        dificultad++;
        emit dificultadCambiada(dificultad);
        qDebug() << "Dificultad aumentada a nivel:" << dificultad;
    }
}

void GameManager::actualizarTiempo()
{
    if (estado == Playing) {
        tiempoTranscurrido++;
        emit tiempoActualizado(tiempoTranscurrido);
    }
}

void GameManager::cambiarEstado(GameState nuevoEstado)
{
    if (estado != nuevoEstado) {
        estado = nuevoEstado;
        emit estadoCambiado(nuevoEstado);
    }
}
