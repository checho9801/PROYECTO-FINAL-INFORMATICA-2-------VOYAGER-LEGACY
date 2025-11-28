#ifndef NIVEL2_H
#define NIVEL2_H

#include "nivelbase.h"
#include "collisionmanager.h"
#include <QTimer>

class QLabel; // FIX: Declaración anticipada en lugar de incluir la cabecera completa
class Gema;
class Meteorito;

/**
 * @brief Nivel 2 - Recolectar gemas mientras se esquivan meteoritos
 *
 * El jugador debe recolectar 5 gemas para completar el nivel
 */
class Nivel2 : public NivelBase
{
    Q_OBJECT

public:
    /**
     * @brief Constructor del Nivel 2
     * @param escena Escena gráfica
     * @param parent Objeto padre
     */
    explicit Nivel2(QGraphicsScene *escena, QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~Nivel2() override;

    // Implementación de métodos virtuales
    void iniciar() override;
    void detener() override;
    void pausar() override;
    void reanudar() override;
    void limpiar() override;
    void actualizar() override;
    bool estaCompletado() const override;

    /**
     * @brief Obtiene el número de gemas recolectadas
     * @return Contador de gemas
     */
    int getGemasRecolectadas() const { return gemasRecolectadas; }

    /**
     * @brief Obtiene el label de gemas para mostrarlo en el HUD
     * @return Puntero al QLabel o nullptr
     */
    QLabel* getLabelGemas() const { return labelGemas; }

signals:
    /**
     * @brief Se emite cuando se recolecta una gema
     * @param total Total de gemas recolectadas
     */
    void gemaRecolectadaSignal(int total);

private slots:
    /**
     * @brief Genera un nuevo meteorito
     */
    void spawnMeteorito();

    /**
     * @brief Genera una nueva gema
     */
    void spawnGema();

    /**
     * @brief Maneja la recolección de una gema
     */
    void onGemaRecolectada();

    /**
     * @brief Actualiza las colisiones
     */
    void verificarColisiones();

    /**
     * @brief Verifica si la nave perdió todas las vidas
     */
    void verificarVidas(); // FIX: Slot para el Game Over inmediato

private:
    // Timers
    QTimer *timerMeteorito;      ///< Timer para generar meteoritos
    QTimer *timerGema;           ///< Timer para generar gemas
    QTimer *timerColisiones;     ///< Timer para verificar colisiones
    QTimer *timerVidas;          ///< FIX: Timer para verificar vidas

    // Managers
    CollisionManager *collisionManager;

    // Contadores
    int gemasRecolectadas;       ///< Gemas recolectadas en este nivel
    int gemasNecesarias;         ///< Gemas necesarias para ganar

    // HUD
    QLabel *labelGemas;          ///< Etiqueta que muestra el progreso

    /**
     * @brief Inicializa los timers del nivel
     */
    void inicializarTimers();

    /**
     * @brief Inicializa el HUD específico del nivel
     */
    void inicializarHUD();

    /**
     * @brief Actualiza el HUD con el progreso actual
     */
    void actualizarHUD();

    /**
     * @brief Limpia todos los meteoritos de la escena
     */
    void limpiarMeteoritos();

    /**
     * @brief Limpia todas las gemas de la escena
     */
    void limpiarGemas();
};

#endif // NIVEL2_H
