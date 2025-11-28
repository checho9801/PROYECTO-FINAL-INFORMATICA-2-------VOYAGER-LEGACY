#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#include <QObject>
#include <QGraphicsItem>
#include <QList>

class Nave;
class Meteorito;
class Gema;  //

/**
 * @brief Gestiona todas las colisiones del juego
 *
 * Verifica colisiones entre diferentes tipos de objetos
 * y emite señales cuando ocurren
 */
class CollisionManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Tipos de colisión posibles
     */
    enum TipoColision {
        NaveMeteorito,      ///< Colisión entre nave y meteorito
        NaveGema,           ///< Colisión entre nave y gema
        //NavePowerUp,        ///< Colisión entre nave y power-up
       // ProyectilMeteorito  ///< Colisión entre proyectil y meteorito (futuro)
    };

    /**
     * @brief Constructor del CollisionManager
     * @param parent Objeto padre
     */
    explicit CollisionManager(QObject *parent = nullptr);

    /**
     * @brief Verifica si hay colisión entre dos items
     * @param item1 Primer item
     * @param item2 Segundo item
     * @return true si hay colisión
     */
    bool hayColision(QGraphicsItem *item1, QGraphicsItem *item2) const;

    /**
     * @brief Verifica colisiones de la nave con todos los meteoritos
     * @param nave Referencia a la nave
     * @param items Lista de items en escena
     * @return Lista de meteoritos que colisionan
     */
    QList<Meteorito*> verificarColisionesNave(Nave *nave, const QList<QGraphicsItem*> &items);

    /**
     * @brief Verifica colisiones de la nave con gemas (NUEVO)
     * @param nave Referencia a la nave
     * @param items Lista de items en escena
     * @return Lista de gemas recolectadas
     */
    QList<Gema*> verificarColisionesGemas(Nave *nave, const QList<QGraphicsItem*> &items);

    /**
     * @brief Obtiene el área de colisión de un item
     * @param item El item gráfico
     * @return Rectángulo de colisión
     */
    QRectF obtenerAreaColision(QGraphicsItem *item) const;

    /**
     * @brief Calcula la distancia entre dos items
     * @param item1 Primer item
     * @param item2 Segundo item
     * @return Distancia en píxeles
     */
    qreal calcularDistancia(QGraphicsItem *item1, QGraphicsItem *item2) const;

signals:
    /**
     * @brief Se emite cuando ocurre una colisión
     * @param tipo Tipo de colisión detectada
     */
    void colisionDetectada(TipoColision tipo);

    /**
     * @brief Se emite cuando la nave colisiona con un meteorito
     * @param meteorito El meteorito involucrado
     */
    void naveGolpeada(Meteorito *meteorito);

    /**
     * @brief Se emite cuando la nave recolecta una gema (NUEVO)
     * @param gema La gema recolectada
     */
    void gemaRecolectada(Gema *gema);

private:
    /**
     * @brief Margen de error en píxeles para colisiones
     */
    static constexpr int MARGEN_COLISION = 5;
};

#endif // COLLISIONMANAGER_H
