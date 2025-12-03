#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "constants.h"
#include "meteorito.h"
#include "gema.h"
#include "collisionmanager.h"
#include "nivelbase.h"
#include <QBrush>
#include <QTransform>
#include <QMessageBox>
#include <QDebug>
#include <QLabel>
#include <QUrl>
#include <QFile>
#include <QFileInfo>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    desplazamientoY(0),
    velocidadFondo(GameConstants::BACKGROUND_SPEED),
    textoVidas(nullptr),
    musicaMenu(nullptr),
    musicaNivel1(nullptr),
    musicaNivel2(nullptr),
    sfxColision(nullptr),
    sfxGema(nullptr),
    sfxVictoria(nullptr),
    sfxGameOver(nullptr)
{
    ui->setupUi(this);

    // Configurar ventana
    setWindowTitle("Voyager Legacy");

    // Inicializar componentes
    inicializarEscena();
    inicializarFondo();
    inicializarHUD();
    inicializarManagers();
    inicializarSonidos();
    setupMenu();

    timerGameOver = nullptr;

    // Conectar señales del GameManager
    connect(gameManager, &GameManager::estadoCambiado, this, &MainWindow::manejarEstadoJuego);

    // Conectar señales del NivelManager
    connect(nivelManager, &NivelManager::nivelCargado, this, &MainWindow::onNivelCargado);
    connect(nivelManager, &NivelManager::nivelCompletado, this, &MainWindow::onNivelCompletado);
    connect(nivelManager, &NivelManager::nivelFallado, this, &MainWindow::onNivelFallado);
    connect(nivelManager, &NivelManager::todosLosNivelesCompletados, this, &MainWindow::onTodosLosNivelesCompletados);

    // Timer para el fondo scrolleable
    timerFondo = new QTimer(this);
    connect(timerFondo, &QTimer::timeout, this, &MainWindow::actualizarFondo);
    connect(timerFondo, &QTimer::timeout, this, &MainWindow::actualizarHUD);

    // Timer para transiciones
    timerTransicion = new QTimer(this);
    timerTransicion->setSingleShot(true); // Solo se ejecuta una vez

    // Iniciar en el menú
    manejarEstadoJuego(gameManager->getEstado());
}

MainWindow::~MainWindow()
{
    // Limpiar objetos de audio
    delete musicaMenu;
    delete musicaNivel1;
    delete musicaNivel2;
    delete sfxColision;
    delete sfxGema;
    delete sfxVictoria;
    delete sfxGameOver;

    delete ui;
}

void MainWindow::inicializarEscena()
{
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    scene->setSceneRect(0, 0, GameConstants::SCENE_WIDTH, GameConstants::SCENE_HEIGHT);

    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

void MainWindow::inicializarFondo()
{
    // Cargar fondo de estrellas (doble altura para scroll continuo)
    QPixmap fondo(GameConstants::Resources::BACKGROUND);

    if (fondo.isNull()) {
        qDebug() << "ERROR: No se pudo cargar el fondo:" << GameConstants::Resources::BACKGROUND;
        fondoEscalado = QPixmap(GameConstants::SCENE_WIDTH, GameConstants::SCENE_HEIGHT * 2);
        fondoEscalado.fill(Qt::darkBlue);
    } else {
        qDebug() << "Fondo cargado correctamente";
        fondoEscalado = fondo.scaled(
            GameConstants::SCENE_WIDTH,
            GameConstants::SCENE_HEIGHT * 2,
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation
            );
    }

    // Crear item de fondo
    fondoItem = new QGraphicsPixmapItem();
    scene->addItem(fondoItem);
    fondoItem->setZValue(-1);
}

void MainWindow::inicializarHUD()
{
    // Limpiar elementos previos
    for (auto c : corazones) {
        scene->removeItem(c);
        delete c;
    }
    corazones.clear();

    if (textoVidas) {
        scene->removeItem(textoVidas);
        delete textoVidas;
        textoVidas = nullptr;
    }

    // Crear texto "Vidas:"
    textoVidas = new QGraphicsTextItem("Vidas:");
    textoVidas->setDefaultTextColor(Qt::white);
    QFont font = textoVidas->font();
    font.setBold(true);
    font.setPointSize(14);
    textoVidas->setFont(font);
    textoVidas->setPos(GameConstants::HUD_MARGIN_X, GameConstants::HUD_MARGIN_Y - 5);
    scene->addItem(textoVidas);
    textoVidas->hide();

    // Cargar imagen del corazón
    QPixmap corazonPixmap(GameConstants::Resources::CORAZON);

    if (corazonPixmap.isNull()) {
        qDebug() << "ERROR: No se pudo cargar el corazón";
        return;
    }

    QPixmap corazonEscalado = corazonPixmap.scaled(
        GameConstants::CORAZON_SIZE,
        GameConstants::CORAZON_SIZE,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    // Crear corazones
    int offsetX = GameConstants::HUD_MARGIN_X + 60;
    for (int i = 0; i < GameConstants::VIDAS_INICIALES; i++) {
        QGraphicsPixmapItem *corazon = new QGraphicsPixmapItem(corazonEscalado);
        int x = offsetX + i * (GameConstants::CORAZON_SIZE + GameConstants::CORAZON_SPACING);
        corazon->setPos(x, GameConstants::HUD_MARGIN_Y);
        scene->addItem(corazon);
        corazones.append(corazon);
        corazon->hide();
    }

    qDebug() << "HUD inicializado con" << corazones.size() << "corazones";
}

void MainWindow::inicializarManagers()
{
    gameManager = new GameManager(this);
    nivelManager = new NivelManager(scene, this);
}

// ============== SISTEMA DE AUDIO ==============

void MainWindow::inicializarSonidos()
{
    qDebug() << "Inicializando sistema de audio...";

    // Lambda para copiar recurso a archivo temporal
    auto copyResourceToTemp = [](const char* qrcPath) -> QString {
        QFile resourceFile(qrcPath);
        if (!resourceFile.open(QIODevice::ReadOnly)) {
            qWarning() << "No se pudo abrir recurso:" << qrcPath;
            return QString();
        }

        // Crear archivo temporal
        QString fileName = QFileInfo(qrcPath).fileName();
        QString tempPath = QDir::temp().filePath(fileName);

        QFile tempFile(tempPath);
        if (tempFile.exists()) {
            tempFile.remove(); // Eliminar si ya existe
        }

        if (!tempFile.open(QIODevice::WriteOnly)) {
            qWarning() << "No se pudo crear archivo temporal:" << tempPath;
            return QString();
        }

        // Copiar datos
        tempFile.write(resourceFile.readAll());
        tempFile.close();
        resourceFile.close();

        qDebug() << "Copiado a:" << tempPath;
        return tempPath;
    };

    // Lambda para inicializar música (loop infinito)
    auto initMusic = [this, &copyResourceToTemp](QSoundEffect*& effect, const char* resourcePath) {
        QString tempPath = copyResourceToTemp(resourcePath);
        if (tempPath.isEmpty()) {
            qWarning() << "ERROR: No se pudo copiar música:" << resourcePath;
            return;
        }

        effect = new QSoundEffect(this);
        effect->setSource(QUrl::fromLocalFile(tempPath));
        effect->setLoopCount(QSoundEffect::Infinite);
        effect->setVolume(0.3f);

        if (effect->status() == QSoundEffect::Error) {
            qWarning() << "ERROR: No se pudo cargar música:" << resourcePath;
        } else {
            qDebug() << "Música cargada:" << resourcePath;
        }
    };

    // Inicializar música
    initMusic(musicaMenu, GameConstants::Resources::MUSICA_MENU);
    initMusic(musicaNivel1, GameConstants::Resources::MUSICA_NIVEL1);
    initMusic(musicaNivel2, GameConstants::Resources::MUSICA_NIVEL2);

    // Lambda para inicializar efectos de sonido
    auto initSfx = [this, &copyResourceToTemp](QSoundEffect*& effect, const char* resourcePath) {
        QString tempPath = copyResourceToTemp(resourcePath);
        if (tempPath.isEmpty()) {
            qWarning() << "ERROR: No se pudo copiar SFX:" << resourcePath;
            return;
        }

        effect = new QSoundEffect(this);
        effect->setSource(QUrl::fromLocalFile(tempPath));
        effect->setVolume(0.7f);

        if (effect->status() == QSoundEffect::Error) {
            qWarning() << "ERROR: No se pudo cargar SFX:" << resourcePath;
        } else {
            qDebug() << "SFX cargado:" << resourcePath;
        }
    };

    // Inicializar efectos de sonido
    initSfx(sfxColision, GameConstants::Resources::SFX_COLISION);
    initSfx(sfxGema, GameConstants::Resources::SFX_GEMA);
    initSfx(sfxVictoria, GameConstants::Resources::SFX_VICTORIA);
    initSfx(sfxGameOver, GameConstants::Resources::SFX_GAMEOVER);

    qDebug() << "Sistema de audio inicializado";
}

void MainWindow::stopAllMusic()
{
    if (musicaMenu && musicaMenu->isPlaying()) musicaMenu->stop();
    if (musicaNivel1 && musicaNivel1->isPlaying()) musicaNivel1->stop();
    if (musicaNivel2 && musicaNivel2->isPlaying()) musicaNivel2->stop();
}

void MainWindow::playMusicaMenu()
{
    stopAllMusic();
    if (musicaMenu && musicaMenu->status() == QSoundEffect::Ready) {
        musicaMenu->play();
        qDebug() << "Reproduciendo música del menú";
    }
}

void MainWindow::playMusicaNivel1()
{
    stopAllMusic();
    if (musicaNivel1 && musicaNivel1->status() == QSoundEffect::Ready) {
        musicaNivel1->play();
        qDebug() << "Reproduciendo música del Nivel 1";
    }
}

void MainWindow::playMusicaNivel2()
{
    stopAllMusic();
    if (musicaNivel2 && musicaNivel2->status() == QSoundEffect::Ready) {
        musicaNivel2->play();
        qDebug() << "Reproduciendo música del Nivel 2";
    }
}

void MainWindow::playColisionSound()
{
    if (sfxColision && sfxColision->status() == QSoundEffect::Ready) {
        sfxColision->play();
    }
}

void MainWindow::playGemaSound()
{
    if (sfxGema && sfxGema->status() == QSoundEffect::Ready) {
        sfxGema->play();
    }
}

void MainWindow::playVictorySound()
{
    stopAllMusic();
    if (sfxVictoria && sfxVictoria->status() == QSoundEffect::Ready) {
        sfxVictoria->play();
        qDebug() << "Reproduciendo sonido de victoria";
    }
}

void MainWindow::playGameOverSound()
{
    stopAllMusic();
    if (sfxGameOver && sfxGameOver->status() == QSoundEffect::Ready) {
        sfxGameOver->play();
        qDebug() << "Reproduciendo sonido de Game Over";
    }
}

// ============== FIN SISTEMA DE AUDIO ==============

void MainWindow::setupMenu()
{
    // Crear botón "Comenzar el juego"
    btnComenzar = new QPushButton("Comenzar el juego", this);
    btnComenzar->setFont(QFont("Arial", 20, QFont::Bold));
    btnComenzar->setStyleSheet(
        "QPushButton { background-color: rgba(60, 99, 130, 200); color: white; "
        "border: 2px solid #63B8FF; border-radius: 10px; padding: 10px 20px; }"
        "QPushButton:hover { background-color: rgba(74, 122, 159, 230); }"
        );

    // Crear botón "Opciones"
    btnOpciones = new QPushButton("Opciones", this);
    btnOpciones->setFont(QFont("Arial", 20, QFont::Bold));
    btnOpciones->setStyleSheet(
        "QPushButton { background-color: rgba(60, 99, 130, 200); color: white; "
        "border: 2px solid #63B8FF; border-radius: 10px; padding: 10px 20px; }"
        "QPushButton:hover { background-color: rgba(74, 122, 159, 230); }"
        );

    // Posicionar botones
    int btnWidth = 300;
    int btnHeight = 60;
    int centerX = (GameConstants::SCENE_WIDTH - btnWidth) / 2;
    int startY = GameConstants::SCENE_HEIGHT / 2 - 50;

    btnComenzar->setGeometry(centerX, startY, btnWidth, btnHeight);
    btnOpciones->setGeometry(centerX, startY + btnHeight + 20, btnWidth, btnHeight);

    // Conectar botón al inicio del juego
    connect(btnComenzar, &QPushButton::clicked, [this]() {
        gameManager->iniciarJuego();
        mostrarIntroNivel(NivelManager::Nivel1);
    });

    btnComenzar->hide();
    btnOpciones->hide();
}

void MainWindow::manejarEstadoJuego(GameManager::GameState nuevoEstado)
{
    bool isMenu = (nuevoEstado == GameManager::Menu);
    bool isPlaying = (nuevoEstado == GameManager::Playing);
    bool isGameOver = (nuevoEstado == GameManager::GameOver);

    NivelBase *nivelActual = nivelManager->getNivelActual();

    // **Ajuste para corregir el error:** Determinar la escena correcta a usar.
    // Dado que 'NivelBase' no tiene 'getScene()', usaremos la escena principal de MainWindow (scene).
    QGraphicsScene *targetScene = scene;

    qDebug() << "Estado del juego cambió a:" << nuevoEstado;

    // Control del fondo scrolleable
    if (isPlaying) {
        timerFondo->start(GameConstants::FRAME_UPDATE_MS);
        QBrush brush(fondoEscalado);
        brush.setTransform(QTransform::fromTranslate(0, desplazamientoY));
        targetScene->setBackgroundBrush(brush); // Usamos targetScene
    } else {
        timerFondo->stop();
    }

    // Visibilidad del menú
    btnComenzar->setVisible(isMenu);
    btnOpciones->setVisible(isMenu);

    // Visibilidad del HUD
    mostrarElementosJuego(isPlaying);

    // Fondo del menú
    if (isMenu) {
        QPixmap portada(GameConstants::Resources::MENU_BACKGROUND);
        if (portada.isNull()) {
            qDebug() << "ERROR: No se pudo cargar MENU_BACKGROUND";
            portada = QPixmap(GameConstants::SCENE_WIDTH, GameConstants::SCENE_HEIGHT);
            portada.fill(Qt::darkBlue);
        } else {
            portada = portada.scaled(
                GameConstants::SCENE_WIDTH,
                GameConstants::SCENE_HEIGHT,
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
                );
        }

        QBrush brush(portada);
        targetScene->setBackgroundBrush(brush); // Usamos targetScene
        limpiarEscena();

        // Reproducir música del menú
        playMusicaMenu();
    } else {
        // Detener música cuando no estamos en el menú
        // (la música de niveles se inicia en onNivelCargado)
        if (!isPlaying) {
            stopAllMusic();
        }
    }

    // Manejar estado de Game Over
    if (isGameOver) {
        qDebug() << "Estado Game Over: deteniendo todo y mostrando pantalla final.";

        // 1. Detener todos los timers relevantes del juego
        timerFondo->stop();
        // ** (Añade aquí la detención de otros timers si los hay, p.ej., timerGeneradorEnemigos->stop()) **

        // 2. Detener música
        stopAllMusic();

        // 3. Ocultar HUD
        mostrarElementosJuego(false);

        // 4. Mostrar la imagen GAME_OVER como fondo
        QPixmap gameOverImage(GameConstants::Resources::GAME_OVER);
        if (gameOverImage.isNull()) {
            qDebug() << "ERROR: No se pudo cargar GAME_OVER. Usando fondo rojo.";
            gameOverImage = QPixmap(GameConstants::SCENE_WIDTH, GameConstants::SCENE_HEIGHT);
            gameOverImage.fill(Qt::red);
        } else {
            // Escalar la imagen para que cubra la escena
            gameOverImage = gameOverImage.scaled(
                GameConstants::SCENE_WIDTH,
                GameConstants::SCENE_HEIGHT,
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
                );
        }

        QBrush gameOverBrush(gameOverImage);

        // Usamos targetScene (que ahora es MainWindow::scene)
        targetScene->setBackgroundBrush(gameOverBrush);

        // 5. Configurar e iniciar el temporizador de cierre (5 segundos = 5000 ms)
        if (!timerGameOver) {
            // Inicializar el timer si es la primera vez (asumiendo que es un puntero a QTimer* de MainWindow)
            timerGameOver = new QTimer(this);

            // **ESTA CONEXIÓN ES LA CORRECTA PARA CERRAR LA APP:**
            connect(timerGameOver, &QTimer::timeout, QCoreApplication::instance(), &QCoreApplication::quit);
        }

        // Iniciar el temporizador para que se dispare una vez después de 5 segundos.
        timerGameOver->setSingleShot(true);
        timerGameOver->start(5000);

    }

    // Actualizamos la escena que esté activa
    targetScene->update();
}

void MainWindow::onNivelCargado(NivelManager::NivelID nivelID)
{
    qDebug() << "MainWindow: Nivel" << nivelID << "cargado";
    setWindowTitle(QString("Voyager Legacy - Nivel %1").arg(nivelID));

    // Reproducir música del nivel correspondiente
    if (nivelID == NivelManager::Nivel1) {
        playMusicaNivel1();
    } else if (nivelID == NivelManager::Nivel2) {
        playMusicaNivel2();

        // Si es el Nivel 2, configurar el label de gemas
        Nivel2 *nivel2 = dynamic_cast<Nivel2*>(nivelManager->getNivelActual());
        if (nivel2 && nivel2->getLabelGemas()) {
            QLabel *labelGemas = nivel2->getLabelGemas();
            labelGemas->setParent(this);
            labelGemas->move(GameConstants::SCENE_WIDTH - 200, GameConstants::HUD_MARGIN_Y);
            labelGemas->show();
        }
    }

    // Conectar sonidos de colisiones y gemas
    NivelBase *nivelActual = nivelManager->getNivelActual();
    if (!nivelActual) return;

    // Conectar sonido de colisión con la nave
       if (nivelActual && nivelActual->getNave()) {
        Nave *nave = nivelActual->getNave();
        connect(nave, &Nave::colisionConMeteorito, this, &MainWindow::playColisionSound, Qt::UniqueConnection);
    }

    // Para el Nivel 2, conectar sonido de gemas
    if (nivelID == NivelManager::Nivel2) {
        Nivel2 *nivel2 = dynamic_cast<Nivel2*>(nivelActual);
        if (nivel2) {
            connect(nivel2, &Nivel2::gemaRecolectadaSignal, this, &MainWindow::playGemaSound, Qt::UniqueConnection);
        }
    }
}

void MainWindow::onNivelCompletado(NivelManager::NivelID nivelCompletado)
{
    qDebug() << "MainWindow: Nivel" << nivelCompletado << "completado";

    // Reproducir sonido de victoria
    playVictorySound();

    mostrarPantallaVictoria(nivelCompletado);
}

void MainWindow::onNivelFallado(NivelManager::NivelID nivelID)
{
    qDebug() << "MainWindow: Nivel" << nivelID << "fallado - Activando Game Over";

    // Detener el nivel completamente
    nivelManager->pausarNivel();

    // Deshabilitar la nave para que no se pueda mover
    NivelBase *nivelActual = nivelManager->getNivelActual();
    if (nivelActual && nivelActual->getNave()) {
        Nave *nave = nivelActual->getNave();
        nave->setEnabled(false);
        nave->clearFocus();
    }

    // Cambiar el estado del juego a Game Over
    gameManager->terminarJuego();

    // Mostrar pantalla de Game Over
    mostrarPantallaGameOver();
}

void MainWindow::onTodosLosNivelesCompletados()
{
    qDebug() << "¡Todos los niveles completados!";

    QMessageBox::information(
        this,
        "¡VICTORIA TOTAL!",
        "¡Felicidades!\n\nHas completado todos los niveles de Voyager Legacy.\n\n"
        "¡Eres un verdadero explorador espacial! 🚀🌟"
        );

    qApp->quit();
}

void MainWindow::mostrarIntroNivel(NivelManager::NivelID nivelID)
{
    qDebug() << "Mostrando intro del Nivel" << nivelID;

    // Detener el fondo scrolleable
    timerFondo->stop();

    // Ocultar elementos del HUD
    mostrarElementosJuego(false);

    // Ocultar botones del menú
    btnComenzar->hide();
    btnOpciones->hide();

    // Cargar la imagen de intro correspondiente
    QString rutaIntro;
    if (nivelID == NivelManager::Nivel1) {
        rutaIntro = GameConstants::Resources::NIVEL1_INTRO;
    } else if (nivelID == NivelManager::Nivel2) {
        rutaIntro = GameConstants::Resources::NIVEL2_INTRO;
    }

    QPixmap intro(rutaIntro);
    if (intro.isNull()) {
        qDebug() << "ERROR: No se pudo cargar intro:" << rutaIntro;
        // Si falla, cargar el nivel directamente
        iniciarNivelDespuesDeIntro(nivelID);
        return;
    }

    intro = intro.scaled(
        GameConstants::SCENE_WIDTH,
        GameConstants::SCENE_HEIGHT,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );

    // Mostrar la intro como fondo
    QBrush brush(intro);
    scene->setBackgroundBrush(brush);
    scene->update();

    // Configurar timer para cargar el nivel después de X segundos
    disconnect(timerTransicion, nullptr, nullptr, nullptr); // Limpiar conexiones previas
    connect(timerTransicion, &QTimer::timeout, [this, nivelID]() {
        iniciarNivelDespuesDeIntro(nivelID);
    });

    timerTransicion->start(GameConstants::TIEMPO_INTRO_NIVEL);
}

void MainWindow::iniciarNivelDespuesDeIntro(NivelManager::NivelID nivelID)
{
    qDebug() << "Iniciando Nivel" << nivelID << "después de intro";

    // Restaurar el fondo scrolleable
    timerFondo->start(GameConstants::FRAME_UPDATE_MS);
    QBrush brush(fondoEscalado);
    brush.setTransform(QTransform::fromTranslate(0, desplazamientoY));
    scene->setBackgroundBrush(brush);

    // Mostrar elementos del HUD
    mostrarElementosJuego(true);

    // Cargar el nivel
    nivelManager->cargarNivel(nivelID);

    scene->update();
}

void MainWindow::mostrarPantallaVictoria(NivelManager::NivelID nivelCompletado)
{
    if (nivelCompletado == NivelManager::Nivel1) {
        qDebug() << "¡Nivel 1 completado! Mostrando la Luna...";

        // Detener el fondo scrolleable
        timerFondo->stop();

        // Ocultar elementos del HUD
        mostrarElementosJuego(false);

        // Cargar imagen de la Luna
        QPixmap luna(GameConstants::Resources::LUNA);
        if (luna.isNull()) {
            qDebug() << "ERROR: No se pudo cargar la imagen de la Luna";
            // Continuar sin imagen
        } else {
            QPixmap lunaEscalada = luna.scaled(
                scene->width(),
                scene->height(),
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
                );

            QBrush brush(lunaEscalada);
            scene->setBackgroundBrush(brush);
            scene->update();
        }

        // Configurar timer para mostrar mensaje y cargar Nivel 2
        disconnect(timerTransicion, nullptr, nullptr, nullptr);
        connect(timerTransicion, &QTimer::timeout, [this]() {
            QMessageBox::information(
                this,
                "¡Nivel 1 Completado!",
                "¡Has llegado a la Luna! 🌕\n\nAhora viene el Nivel 2:\n"
                "Recolecta 5 gemas mientras esquivas meteoritos."
                );

            // Mostrar intro del Nivel 2
            mostrarIntroNivel(NivelManager::Nivel2);
        });

        timerTransicion->start(GameConstants::TIEMPO_PANTALLA_VICTORIA);

    } else if (nivelCompletado == NivelManager::Nivel2) {
        qDebug() << "¡Nivel 2 completado!";

        QMessageBox::information(
            this,
            "¡Nivel 2 Completado!",
            "¡Has recolectado todas las gemas! 💎\n\n"
            "¡Felicidades, explorador espacial!"
            );

        // Terminar el juego
        onTodosLosNivelesCompletados();
    }
}

void MainWindow::mostrarPantallaGameOver()
{
    timerFondo->stop();

    // Reproducir sonido de Game Over
    playGameOverSound();

    QMessageBox::information(
        this,
        "Fin del juego",
        "¡Has perdido todas las vidas!\n\nLa misión ha fallado. 💥"
        );

    qApp->quit();
}

void MainWindow::actualizarFondo()
{
    desplazamientoY += velocidadFondo;

    if (desplazamientoY >= GameConstants::SCENE_HEIGHT) {
        desplazamientoY = 0;
    }

    QBrush brush(fondoEscalado);
    brush.setTransform(QTransform::fromTranslate(0, desplazamientoY));
    scene->setBackgroundBrush(brush);
}

void MainWindow::actualizarHUD()
{
    if (!nivelManager->getNivelActual()) return;

    Nave *nave = nivelManager->getNivelActual()->getNave();
    if (!nave) return;

    int vidasActuales = nave->getVidas();

    // Actualizar visibilidad de corazones
    for (int i = 0; i < corazones.size(); i++) {
        corazones[i]->setVisible(i < vidasActuales);
    }

    // Verificar game over
    if (vidasActuales <= 0) {
        gameManager->terminarJuego();
    }
}

void MainWindow::mostrarElementosJuego(bool mostrar)
{
    if (textoVidas) textoVidas->setVisible(mostrar);

    for (auto corazon : corazones) {
        corazon->setVisible(false); // Se actualizarán en actualizarHUD
    }
}

void MainWindow::limpiarEscena()
{
    // El NivelManager ya limpia sus propios elementos
    // Aquí solo limpiamos elementos globales si es necesario
}
