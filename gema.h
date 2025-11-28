#ifndef GEMA_H
#define GEMA_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>

class Nave;

/**
 * @brief Clase que representa una gema coleccionable
 *
 * Las gemas aparecen aleatoriamente y otorgan puntos
 * cuando la nave las recolecta
 */
class Gema : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    /**
     * @brief Constructor de la gema
     * @param naveRef Referencia a la nave para detectar colisiones
     * @param parent Item padre en la jerarquía gráfica
     */
    explicit Gema(Nave *naveRef, QGraphicsItem *parent = nullptr);

    /**
     * @brief Destructor de la gema
     */
    ~Gema();

    /**
     * @brief Establece la velocidad de caída de la gema
     * @param nuevaVelocidad Nueva velocidad en píxeles por frame
     */
    void setVelocidad(int nuevaVelocidad);

    /**
     * @brief Obtiene la velocidad actual
     * @return Velocidad en píxeles por frame
     */
    int getVelocidad() const;

    /**
     * @brief Obtiene el valor en puntos de la gema
     * @return Puntos que otorga al recolectarla
     */
    int getValor() const;

signals:
    /**
     * @brief Se emite cuando la gema es recolectada
     */
    void gemaRecolectada();

    /**
     * @brief Se emite cuando la gema sale de pantalla sin ser recolectada
     */
    void gemaPerdida();

private slots:
    /**
     * @brief Actualiza la posición de la gema
     */
    void mover();

private:
    int velocidad;      ///< Velocidad de caída
    int valor;          ///< Puntos que otorga
    QTimer *timer;      ///< Timer para actualizar posición
    Nave *nave;         ///< Referencia a la nave del jugador

    /**
     * @brief Genera una posición X aleatoria válida
     * @return Posición X dentro de los límites de la escena
     */
    int generarPosicionXAleatoria() const;

    /**
     * @brief Verifica si hay colisión con la nave
     * @return true si hay colisión (recolección)
     */
    bool verificarRecoleccion() const;

    /**
     * @brief Verifica si la gema salió de la escena
     * @return true si está fuera de los límites
     */
    bool estaDePantalla() const;

    /**
     * @brief Aplica efecto visual de brillo/rotación a la gema
     */
    void aplicarEfectoVisual();
};

#endif // GEMA_H
