#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include <QTimer>

/**
 * @brief Gestiona el estado general del juego
 *
 * Controla puntuación, nivel de dificultad, tiempo transcurrido,
 * y el estado del juego (jugando, pausado, game over, victoria)
 */
class GameManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Estados posibles del juego
     */
    enum GameState {
        Menu,           ///< En el menú principal
        Playing,        ///< Jugando activamente
        Paused,         ///< Juego en pausa
        GameOver,       ///< El jugador perdió
        Victory         ///< El jugador ganó
    };

    /**
     * @brief Constructor del GameManager
     * @param parent Objeto padre
     */
    explicit GameManager(QObject *parent = nullptr);

    /**
     * @brief Inicia un nuevo juego (comienza el nivel 1)
     */
    void iniciarJuego();

    /**
     * @brief Pausa el juego
     */
    void pausarJuego();

    /**
     * @brief Reanuda el juego
     */
    void reanudarJuego();

    /**
     * @brief Termina el juego (Game Over)
     */
    void terminarJuego();

    /**
     * @brief Marca el juego como ganado
     */
    void ganarJuego();

    /**
     * @brief Reinicia el juego
     */
    void reiniciarJuego();

    // Getters
    GameState getEstado() const { return estado; }
    int getPuntuacion() const { return puntuacion; }
    int getTiempoTranscurrido() const { return tiempoTranscurrido; }
    int getDificultad() const { return dificultad; }
    bool estaPausado() const { return estado == Paused; }
    bool estaJugando() const { return estado == Playing; }
    bool estaEnMenu() const { return estado == Menu; } // Nuevo Getter

    /**
     * @brief Incrementa la puntuación
     * @param puntos Puntos a agregar
     */
    void agregarPuntos(int puntos);

    /**
     * @brief Incrementa la dificultad
     */
    void aumentarDificultad();

signals:
    /**
     * @brief Se emite cuando cambia el estado del juego
     * @param nuevoEstado El nuevo estado
     */
    void estadoCambiado(GameState nuevoEstado);

    /**
     * @brief Se emite cuando cambia la puntuación
     * @param nuevaPuntuacion La nueva puntuación
     */
    void puntuacionCambiada(int nuevaPuntuacion);

    /**
     * @brief Se emite cada segundo de juego
     * @param segundos Tiempo transcurrido en segundos
     */
    void tiempoActualizado(int segundos);

    /**
     * @brief Se emite cuando aumenta la dificultad
     * @param nivel Nuevo nivel de dificultad
     */
    void dificultadCambiada(int nivel);

private slots:
    /**
     * @brief Actualiza el contador de tiempo
     */
    void actualizarTiempo();

private:
    GameState estado;             ///< Estado actual del juego
    int puntuacion;               ///< Puntuación acumulada
    int tiempoTranscurrido;       ///< Tiempo en segundos desde inicio
    int dificultad;               ///< Nivel de dificultad actual
    QTimer *timerTiempo;          ///< Timer para contar tiempo

    /**
     * @brief Cambia el estado del juego
     * @param nuevoEstado Nuevo estado
     */
    void cambiarEstado(GameState nuevoEstado);
};

#endif // GAMEMANAGER_H
