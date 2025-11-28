#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "nave.h"
#include "gamemanager.h"
#include "nivelmanager.h"
#include "nivel2.h"
#include "nivel1.h"
#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>
#include <QPixmap>
#include <QPushButton>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @brief Clase principal de la ventana del juego
 *
 * Gestiona la interfaz, el menú, el HUD y coordina
 * el GameManager con el NivelManager
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Slots del fondo y HUD
    void actualizarFondo();
    void actualizarHUD();

    // Slots del GameManager
    void manejarEstadoJuego(GameManager::GameState nuevoEstado);

    // Slots del NivelManager
    void onNivelCargado(NivelManager::NivelID nivelID);
    void onNivelCompletado(NivelManager::NivelID nivelID);
    void onNivelFallado(NivelManager::NivelID nivelID);
    void onTodosLosNivelesCompletados();

    // Pantallas finales
    void mostrarPantallaVictoria(NivelManager::NivelID nivelCompletado);
    void mostrarPantallaGameOver();

    // Pantallas de transición
    void mostrarIntroNivel(NivelManager::NivelID nivelID);
    void iniciarNivelDespuesDeIntro(NivelManager::NivelID nivelID);

private:
    // UI generada desde el archivo .ui
    Ui::MainWindow *ui;

    // Managers
    GameManager *gameManager;       ///< Gestor del estado general del juego
    NivelManager *nivelManager;     ///< Gestor de niveles

    // Escena y elementos visuales
    QGraphicsScene *scene;
    QGraphicsPixmapItem *fondoItem; ///< Fondo de la escena
    QGraphicsTextItem *textoVidas;  ///< Texto "Vidas:"
    QList<QGraphicsPixmapItem*> corazones; ///< Corazones del HUD

    // Fondo scrolleable
    QPixmap fondoEscalado;
    int desplazamientoY;
    int velocidadFondo;
    QTimer *timerFondo;    //< Timer para scroll del fondo
    QTimer *timerTransicion;  //timmer para transiciones

    // Elementos del menú
    QPushButton *btnComenzar;
    QPushButton *btnOpciones;
    QPushButton *btnContinuar;

    // Métodos de inicialización
    void inicializarEscena();
    void inicializarFondo();
    void inicializarHUD();
    void inicializarManagers();
    void setupMenu();

    // Control de visibilidad
    void mostrarElementosJuego(bool mostrar);
    void limpiarEscena();
};

#endif // MAINWINDOW_H
