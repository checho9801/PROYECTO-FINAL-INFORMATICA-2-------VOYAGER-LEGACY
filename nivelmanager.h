#ifndef NIVELMANAGER_H
#define NIVELMANAGER_H

#include <QObject>
#include <QGraphicsScene>
#include "nivelbase.h" // Solo necesitamos la definición de la clase base

// Declaraciones anticipadas (Forward declarations) de los niveles.
// Esto evita problemas de compilación y acelera el tiempo de compilación.
class Nivel1;
class Nivel2;

/**
 * @brief Gestor de niveles del juego
 *
 * Controla la carga, transición y limpieza entre niveles
 */
class NivelManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Identificadores de niveles
     */
    enum NivelID {
        Ninguno = 0,
        Nivel1 = 1,
        Nivel2 = 2,
        NivelFinal = 3
    };

    /**
     * @brief Constructor del NivelManager
     * @param escena Escena gráfica compartida por todos los niveles
     * @param parent Objeto padre
     */
    explicit NivelManager(QGraphicsScene *escena, QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~NivelManager();

    /**
     * @brief Carga y inicia un nivel específico
     * @param nivelID ID del nivel a cargar
     * @return true si el nivel se cargó correctamente
     */
    bool cargarNivel(NivelID nivelID);

    /**
     * @brief Obtiene el nivel actual activo
     * @return Puntero al nivel actual o nullptr
     */
    NivelBase* getNivelActual() const { return nivelActual; }

    /**
     * @brief Obtiene el ID del nivel actual
     * @return ID del nivel actual
     */
    NivelID getNivelActualID() const { return nivelActualID; }

    /**
     * @brief Pausa el nivel actual
     */
    void pausarNivel();

    /**
     * @brief Reanuda el nivel actual
     */
    void reanudarNivel();

    /**
     * @brief Verifica si hay un siguiente nivel disponible
     * @return true si existe un siguiente nivel
     */
    bool haySiguienteNivel() const;

    /**
     * @brief Carga el siguiente nivel en secuencia
     * @return true si se cargó el siguiente nivel
     */
    bool cargarSiguienteNivel();

    /**
     * @brief Limpia el nivel actual de la memoria
     */
    void limpiarNivelActual();

signals:
    /**
     * @brief Se emite cuando se carga un nuevo nivel
     * @param nivelID ID del nivel cargado
     */
    void nivelCargado(NivelID nivelID);

    /**
     * @brief Se emite cuando el nivel actual es completado
     * @param nivelID ID del nivel completado
     */
    void nivelCompletado(NivelID nivelID);

    /**
     * @brief Se emite cuando el jugador falla el nivel
     * @param nivelID ID del nivel fallado
     */
    void nivelFallado(NivelID nivelID);

    /**
     * @brief Se emite cuando se completan todos los niveles
     */
    void todosLosNivelesCompletados();

private slots:
    /**
     * @brief Maneja la señal de nivel completado
     */
    void onNivelCompletado();

    /**
     * @brief Maneja la señal de nivel fallado
     */
    void onNivelFallado();

private:
    QGraphicsScene *scene;           ///< Escena gráfica compartida
    NivelBase *nivelActual;          ///< Nivel actualmente activo
    NivelID nivelActualID;           ///< ID del nivel actual



    /**
     * @brief Crea una instancia del nivel solicitado
     * @param nivelID ID del nivel a crear
     * @return Puntero al nivel creado o nullptr si falla
     */
    NivelBase* crearNivel(NivelID nivelID);
};

#endif // NIVELMANAGER_H
