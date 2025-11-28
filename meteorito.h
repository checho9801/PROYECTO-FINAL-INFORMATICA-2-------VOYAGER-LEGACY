#ifndef METEORITO_H
#define METEORITO_H

#include "nave.h"
#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>

/**
 * @brief Clase que representa un meteorito enemigo
 *
 * Los meteoritos caen desde la parte superior de la pantalla
 * y causan daño al colisionar con la nave del jugador.
 */
class Meteorito : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    /**
     * @brief Constructor del meteorito
     * @param naveRef Referencia a la nave para detectar colisiones
     * @param parent Item padre en la jerarquía gráfica
     */
    explicit Meteorito(Nave *naveRef, QGraphicsItem *parent = nullptr);

    /**
     * @brief Destructor del meteorito
     */
    ~Meteorito();

    /**
     * @brief Establece la velocidad del meteorito
     * @param nuevaVelocidad Nueva velocidad en píxeles por frame
     */
    void setVelocidad(int nuevaVelocidad);

    /**
     * @brief Obtiene la velocidad actual del meteorito
     * @return Velocidad en píxeles por frame
     */
    int getVelocidad() const;

signals:
    /**
     * @brief Señal emitida cuando el meteorito colisiona con la nave
     */
    void colisionConNave();

    /**
     * @brief Señal emitida cuando el meteorito sale de la escena
     */
    void salioDePantalla();

private slots:
    /**
     * @brief Actualiza la posición del meteorito
     */
    void mover();

private:
    int velocidad;      ///< Velocidad de caída
    QTimer *timer;      ///< Timer para actualizar posición
    Nave *nave;         ///< Referencia a la nave del jugador

    /**
     * @brief Genera una posición X aleatoria válida
     * @return Posición X dentro de los límites de la escena
     */
    int generarPosicionXAleatoria() const;

    /**
     * @brief Verifica si hay colisión con la nave
     * @return true si hay colisión
     */
    bool verificarColision() const;

    /**
     * @brief Verifica si el meteorito salió de la escena
     * @return true si está fuera de los límites
     */
    bool estaDePantalla() const;
};

#endif // METEORITO_H
