#ifndef NIVEL1_H
#define NIVEL1_H

#include "nivelbase.h"
#include <QTimer>

class Meteorito;

/**
 * @brief Nivel 1 - Sobrevivir 60 segundos esquivando meteoritos
 *
 * El jugador debe mantener las 3 vidas durante 60 segundos
 * para completar el nivel y llegar a la Luna
 */
class Nivel1 : public NivelBase
{
    Q_OBJECT

public:
    /**
     * @brief Constructor del Nivel 1
     * @param escena Escena gráfica
     * @param parent Objeto padre
     */
    explicit Nivel1(QGraphicsScene *escena, QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~Nivel1() override;

    // Implementación de métodos virtuales
    void iniciar() override;
    void detener() override;
    void pausar() override;
    void reanudar() override;
    void limpiar() override;
    void actualizar() override;
    bool estaCompletado() const override;

signals:
    /**
     * @brief Se emite cuando cambia el intervalo de spawn (para debug)
     * @param nuevoIntervalo Nuevo intervalo en milisegundos
     */
    void intervaloSpawnCambiado(int nuevoIntervalo);

private slots:
    /**
     * @brief Genera un nuevo meteorito
     */
    void spawnMeteorito();

    /**
     * @brief Acelera gradualmente la aparición de meteoritos
     */
    void acelerarSpawn();

    /**
     * @brief Verifica si se alcanzó el tiempo objetivo (60 segundos)
     */
    void verificarMeta();

    void verificarVidas(); //verifica las vidas constantemente

private:
    // Timers
    QTimer *timerMeteorito;     ///< Timer para generar meteoritos
    QTimer *timerAcelerar;      ///< Timer para acelerar spawn
    QTimer *timerMeta;          ///< Timer para verificar meta (60s)
    QTimer *timerVidas;          /// Timer para verificar vidas constantemente

    // Estado del nivel
    int tiempoTranscurrido;     ///< Tiempo en milisegundos desde inicio
    int intervaloSpawn;         ///< Intervalo actual entre meteoritos

    /**
     * @brief Inicializa los timers del nivel
     */
    void inicializarTimers();

    /**
     * @brief Limpia todos los meteoritos de la escena
     */
    void limpiarMeteoritos();
};

#endif // NIVEL1_H
