#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "nave.h"
#include "gamemanager.h"
#include "nivelmanager.h"
#include "nivel2.h"
#include "naveN3.h"
#include "nivel2.h"
#include "nivel3.h"
#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>
#include <QPixmap>
#include <QPushButton>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QSoundEffect>  // NUEVO: Para el audio

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
public slots:
    void onNivelCargado(NivelManager::NivelID nivelID);
    void onNivelCompletado(NivelManager::NivelID nivelCompletado);
    void onNivelFallado(NivelManager::NivelID nivelID);
    void onTodosLosNivelesCompletados();

    // Pantallas finales
    void mostrarPantallaVictoria(NivelManager::NivelID nivelCompletado);
    void mostrarPantallaGameOver();

    // Pantallas de transición (NUEVOS)
    void mostrarIntroNivel(NivelManager::NivelID nivelID);
    void iniciarNivelDespuesDeIntro(NivelManager::NivelID nivelID);

    // Slots de audio (NUEVOS)
    void playColisionSound();
    void playGemaSound();

    //slots nivel 3
    void playMusicaNivel3();


private:
    // UI generada desde el archivo .ui
    Ui::MainWindow *ui;
    NaveN3 *naveN3Nivel3;
    QTimer *timerNivel3;

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
    QTimer *timerFondo;             ///< Timer para scroll del fondo
    QTimer *timerTransicion;        ///< Timer para transiciones entre pantallas
    QTimer *timerGameOver;         ///< Timer para el retardo de 5 segundos antes de cerrar la app

    // Elementos del menú
    QPushButton *btnComenzar;
    QPushButton *btnOpciones;

    // Sistema de audio
    QSoundEffect *musicaMenu;
    QMediaPlayer *musicaNivel1;
    QMediaPlayer *musicaNivel2;
    QMediaPlayer *musicaNivel3;

    QAudioOutput *audioOutputNivel3 = nullptr;
    QAudioOutput *audioOutputNivel2 = nullptr;
    QAudioOutput *audioOutputNivel1 = nullptr;

    //efectos
    QSoundEffect *sfxColision;
    QSoundEffect *sfxGema;
    QSoundEffect *sfxVictoria;
    QSoundEffect *sfxGameOver;

    // Métodos de inicialización
    void inicializarEscena();
    void inicializarFondo();
    void inicializarHUD();
    void inicializarManagers();
    void inicializarSonidos();  // NUEVO
    void setupMenu();

    // Control de audio (NUEVO)
    void stopAllMusic();
    void playMusicaMenu();
    void playMusicaNivel1();
    void playMusicaNivel2();
    void playVictorySound();
    void playGameOverSound();

    // Control de visibilidad
    void mostrarElementosJuego(bool mostrar);
    void limpiarEscena();
};

#endif // MAINWINDOW_H
