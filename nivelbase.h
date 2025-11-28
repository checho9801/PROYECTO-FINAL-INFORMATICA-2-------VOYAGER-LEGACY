#ifndef NIVELBASE_H
#define NIVELBASE_H

#include <QObject>
#include <QGraphicsScene>
#include <QTimer>
#include "nave.h"

/**
 * @brief Clase base abstracta para todos los niveles
 *
 * Define la interfaz común que todos los niveles deben implementar
 */
class NivelBase : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor de la clase base
     * @param escena Escena gráfica donde se dibuja el nivel
     * @param parent Objeto padre
     */
    explicit NivelBase(QGraphicsScene *escena, QObject *parent = nullptr);

    /**
     * @brief Destructor virtual
     */
    virtual ~NivelBase();

    /**
     * @brief Inicia el nivel
     */
    virtual void iniciar() = 0;

    /**
     * @brief Detiene el nivel
     */
    virtual void detener() = 0;

    /**
     * @brief Pausa el nivel
     */
    virtual void pausar() = 0;

    /**
     * @brief Reanuda el nivel
     */
    virtual void reanudar() = 0;

    /**
     * @brief Limpia todos los elementos del nivel
     */
    virtual void limpiar() = 0;

    /**
     * @brief Actualiza la lógica del nivel
     */
    virtual void actualizar() = 0;

    /**
     * @brief Verifica si el nivel ha sido completado
     * @return true si se cumplieron las condiciones de victoria
     */
    virtual bool estaCompletado() const = 0;

    /**
     * @brief Obtiene la nave del jugador
     * @return Puntero a la nave
     */
    Nave* getNave() const { return nave; }

signals:
    /**
     * @brief Se emite cuando el nivel es completado
     */
    void nivelCompletado();

    /**
     * @brief Se emite cuando el jugador pierde el nivel
     */
    void nivelFallado();

protected:
    QGraphicsScene *scene;  ///< Escena gráfica del nivel
    Nave *nave;             ///< Nave del jugador
    bool pausado;           ///< Estado de pausa
    bool completado;        ///< Nivel completado

    /**
     * @brief Inicializa la nave del jugador
     */
    void inicializarNave();
};

#endif // NIVELBASE_H
