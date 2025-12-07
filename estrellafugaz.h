#ifndef ESTRELLAFUGAZ_H
#define ESTRELLAFUGAZ_H

#include "nave.h"
#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>

/**
 * @brief Clase que representa una estrella fugaz
 *
 * Las estrellas fugaces NO causan daño, pero demuestran
 * diferentes tipos de colisiones físicas al interactuar con la nave.
 * Son objetos educativos y visuales.
 */
class EstrellaFugaz : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    /**
     * @brief Tipos de colisión física
     */
    enum TipoColision {
        Elastica,                    ///< Conserva energía cinética (rebote perfecto)
        Inelastica,                  ///< Pierde energía cinética (rebote parcial)
        PerfectamenteInelastica     ///< Se pega a la nave temporalmente
    };

    /**
     * @brief Constructor de la estrella fugaz
     * @param naveRef Referencia a la nave para detectar colisiones
     * @param tipo Tipo de colisión física
     * @param parent Item padre en la jerarquía gráfica
     */
    explicit EstrellaFugaz(Nave *naveRef, TipoColision tipo, QGraphicsItem *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~EstrellaFugaz();

    /**
     * @brief Obtiene el tipo de colisión
     * @return Tipo de colisión
     */
    TipoColision getTipoColision() const { return tipoColision; }

    /**
     * @brief Verifica si está pegada a la nave
     * @return true si está pegada
     */
    bool estaPegada() const { return pegadaANave; }

signals:
    /**
     * @brief Se emite cuando colisiona con la nave (sin causar daño)
     * @param tipo Tipo de colisión que ocurrió
     */
    void colisionSinDanio(TipoColision tipo);

    /**
     * @brief Se emite cuando sale de pantalla
     */
    void salioDePantalla();

private slots:
    /**
     * @brief Actualiza la posición
     */
    void mover();

    /**
     * @brief Despegar de la nave después de colisión perfectamente inelástica
     */
    void despegar();

private:
    int velocidad;              ///< Velocidad vertical
    int velocidadX;             ///< Velocidad horizontal
    QTimer *timer;              ///< Timer de movimiento
    Nave *nave;                 ///< Referencia a la nave
    TipoColision tipoColision;  ///< Tipo de colisión física

    // Para colisiones perfectamente inelásticas
    bool pegadaANave;           ///< Indica si está pegada
    bool haColisionado;         ///< Flag anti-múltiples colisiones
    QTimer *timerDespegue;      ///< Timer para despegarse
    QPointF offsetPegado;       ///< Offset relativo cuando está pegada

    // Coeficientes de restitución
    static constexpr double COEF_ELASTICA = 1.0;
    static constexpr double COEF_INELASTICA = 0.5;
    static constexpr double COEF_PERF_INELASTICA = 0.0;
    static constexpr int TIEMPO_PEGADO_MS = 2000;

    int generarPosicionXAleatoria() const;
    bool verificarColision() const;
    bool estaDePantalla() const;
    void aplicarColision();
    void aplicarEfectoVisual();
};

#endif // ESTRELLAFUGAZ_H
