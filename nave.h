#ifndef NAVE_H
#define NAVE_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QKeyEvent>

/**
 * @brief Clase que representa la nave del jugador
 *
 * Hereda de QObject para señales/slots y de QGraphicsPixmapItem
 * para la representación gráfica en la escena.
 */
class Nave : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    /**
     * @brief Constructor de la nave
     * @param parent Item padre en la jerarquía gráfica
     */
    explicit Nave(QGraphicsItem *parent = nullptr);

    /**
     * @brief Reduce las vidas de la nave en 1
     */
    void restarVida();

    /**
     * @brief Obtiene las vidas actuales
     * @return Número de vidas restantes
     */
    int getVidas() const;

    /**
     * @brief Reinicia el estado de la nave
     */
    void reiniciar();

signals:
    /**
     * @brief Señal emitida cuando las vidas cambian
     * @param vidasRestantes Número de vidas después del cambio
     */
    void vidasCambiadas(int vidasRestantes);

    void colisionConMeteorito();

    /**
     * @brief Señal emitida cuando la nave es destruida
     */
    void naveDestruida();

protected:
    /**
     * @brief Maneja los eventos de teclado para mover la nave
     * @param event Evento de teclado recibido
     */
    void keyPressEvent(QKeyEvent *event) override;

private:
    int velocidad;      ///< Velocidad de movimiento (píxeles por frame)
    int vidas;          ///< Vidas actuales de la nave

    /**
     * @brief Verifica que el movimiento esté dentro de los límites
     * @param newX Nueva posición X
     * @param newY Nueva posición Y
     * @return true si la posición es válida
     */
    bool esMovimientoValido(qreal newX, qreal newY) const;
};

#endif // NAVE_H
