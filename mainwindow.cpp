#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "nivelmanager.h"
#include "constants.h"
#include "nave.h"
#include "nivel2.h" // Necesario para dynamic_cast en onNivelCargado
#include <QBrush>
#include <QTransform>
#include <QMessageBox>
#include <QDebug>
#include <QGraphicsPixmapItem> // Necesario para mostrar la luna/imagen de victoria
#include <QLabel>
#include <QApplication> // Necesario para qApp->quit()

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    desplazamientoY(0),
    velocidadFondo(GameConstants::BACKGROUND_SPEED),
    textoVidas(nullptr),
    btnContinuar(nullptr) // Inicializar el nuevo botón
{
    ui->setupUi(this);

    // Configurar ventana
    setWindowTitle("Voyager Legacy");

    // Inicializar componentes
    inicializarEscena();
    inicializarFondo();
    inicializarHUD();
    inicializarManagers();
    setupMenu();

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

void MainWindow::setupMenu()
{
    // Estilo común para botones
    const QString buttonStyle =
        "QPushButton { background-color: rgba(60, 99, 130, 200); color: white; "
        "border: 2px solid #63B8FF; border-radius: 10px; padding: 10px 20px; }"
        "QPushButton:hover { background-color: rgba(74, 122, 159, 230); }";

    int btnWidth = 300;
    int btnHeight = 60;
    int centerX = (GameConstants::SCENE_WIDTH - btnWidth) / 2;
    int startY = GameConstants::SCENE_HEIGHT / 2 - 50;

    // Crear botón "Comenzar el juego" (usado también para reiniciar)
    btnComenzar = new QPushButton("Comenzar el juego", this);
    btnComenzar->setFont(QFont("Arial", 20, QFont::Bold));
    btnComenzar->setStyleSheet(buttonStyle);
    btnComenzar->setGeometry(centerX, startY, btnWidth, btnHeight);

    // Crear botón "Opciones"
    btnOpciones = new QPushButton("Opciones", this);
    btnOpciones->setFont(QFont("Arial", 20, QFont::Bold));
    btnOpciones->setStyleSheet(buttonStyle);
    btnOpciones->setGeometry(centerX, startY + btnHeight + 20, btnWidth, btnHeight);

    // ** BOTÓN DE CONTINUAR**
    btnContinuar = new QPushButton("Continuar", this);
    btnContinuar->setFont(QFont("Arial", 20, QFont::Bold));
    btnContinuar->setStyleSheet(buttonStyle);
    btnContinuar->setGeometry(centerX, startY + GameConstants::SCENE_HEIGHT/4, btnWidth, btnHeight);

    // Conectar botón al inicio del juego (conexión base para el menú)
    connect(btnComenzar, &QPushButton::clicked, [this]() {
        gameManager->iniciarJuego();
        mostrarIntroNivel(NivelManager::Nivel1);
    });

    btnComenzar->hide();
    btnOpciones->hide();
    btnContinuar->hide(); // Ocultar por defecto
}

void MainWindow::manejarEstadoJuego(GameManager::GameState nuevoEstado)
{
    qDebug() << "Estado del juego cambió a:" << nuevoEstado;

    // Ocultar todos los botones por defecto, se muestran solo si corresponde
    btnComenzar->hide();
    btnOpciones->hide();
    btnContinuar->hide();
    mostrarElementosJuego(false);

    // Deshabilitar la nave por defecto (se habilita solo en Playing)
    if (nivelManager->getNivelActual() && nivelManager->getNivelActual()->getNave()) {
        nivelManager->getNivelActual()->getNave()->setEnabled(false);
        nivelManager->getNivelActual()->getNave()->clearFocus();
    }

    // Restaurar el fondo scrolleable para todos los estados que no sean menú/gameover
    if (nuevoEstado == GameManager::Playing || nuevoEstado == GameManager::Paused) {
        if (!timerFondo->isActive()) {
            timerFondo->start(GameConstants::FRAME_UPDATE_MS);
        }
        QBrush brush(fondoEscalado);
        brush.setTransform(QTransform::fromTranslate(0, desplazamientoY));
        scene->setBackgroundBrush(brush);
    } else {
        timerFondo->stop();
    }

    // --- LÓGICA POR ESTADO ---
    switch (nuevoEstado) {
        case GameManager::Menu:
            // Fondo del menú
            {
                QPixmap portada(GameConstants::Resources::MENU_BACKGROUND);
                // ... (lógica de escalado de portada, ya estaba en tu código)
                if (portada.isNull()) {
                    portada = QPixmap(GameConstants::SCENE_WIDTH, GameConstants::SCENE_HEIGHT);
                    portada.fill(Qt::darkBlue);
                } else {
                    portada = portada.scaled(GameConstants::SCENE_WIDTH, GameConstants::SCENE_HEIGHT, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                }
                QBrush brush(portada);
                scene->setBackgroundBrush(brush);
            }
            limpiarEscena(); // Limpiar elementos temporales de otras pantallas
            btnComenzar->setText("Comenzar el juego");
            btnComenzar->show();
            btnOpciones->show();
            break;

        case GameManager::Playing:
            // Visibilidad del HUD y habilitar la nave
            mostrarElementosJuego(true);
            if (nivelManager->getNivelActual() && nivelManager->getNivelActual()->getNave()) {
                nivelManager->getNivelActual()->getNave()->setEnabled(true);
                nivelManager->getNivelActual()->getNave()->setFocus(); // Para que responda a teclas
            }
            break;

        case GameManager::Paused:
            // Mostrar elementos de pausa (no implementado aún, pero detiene el fondo)
            break;

        case GameManager::GameOver:
            qDebug() << "MainWindow: Entrando en estado GAME OVER.";
            // Limpiar escena y mostrar pantalla de Game Over
            mostrarElementosJuego(false);
            mostrarPantallaGameOver();

            // Re-conectar el botón 'Comenzar' para que ahora reinicie
            disconnect(btnComenzar, nullptr, nullptr, nullptr);
            connect(btnComenzar, &QPushButton::clicked, [this]() {
                btnComenzar->hide();
                gameManager->reiniciarJuego(); // Llama a iniciarJuego()
                mostrarIntroNivel(NivelManager::Nivel1); // Carga la intro del primer nivel
            });
            btnComenzar->setText("Reiniciar Juego");
            btnComenzar->show();
            break;

        case GameManager::Victory:
            // El estado de Victory se gestiona en onNivelCompletado, pero es bueno tenerlo aquí
            // para limpiar el foco de la nave.
            break;
    }

    scene->update();
}

void MainWindow::onNivelCargado(NivelManager::NivelID nivelID)
{
    qDebug() << "MainWindow: Nivel" << nivelID << "cargado";
    // Corregido: Usar %1 para mostrar el ID
    setWindowTitle(QString("Voyager Legacy - Nivel %1").arg(nivelID));

    // Si es el Nivel 2, configurar el label de gemas
    if (nivelID == NivelManager::Nivel2) {
        // Debemos asegurarnos de que el Label de gemas se muestre
        Nivel2 *nivel2 = dynamic_cast<Nivel2*>(nivelManager->getNivelActual());
        if (nivel2 && nivel2->getLabelGemas()) {
            QLabel *labelGemas = nivel2->getLabelGemas();
            labelGemas->setParent(ui->graphicsView); // Asignar al widget padre correcto
            labelGemas->move(GameConstants::SCENE_WIDTH - labelGemas->width() - GameConstants::HUD_MARGIN_X, GameConstants::HUD_MARGIN_Y);
            labelGemas->show();
        }
    } else {
        // Asegurar que cualquier label de nivel anterior (como el de gemas) esté oculto
        // Se asume que el labelGemas está en el nivel y se limpia con el nivel.
    }
}

void MainWindow::onNivelCompletado(NivelManager::NivelID nivelCompletado)
{
    qDebug() << "MainWindow: Nivel" << nivelCompletado << "completado";

    // Detener el nivel actual (aunque lo hace el manager, por si acaso)
    nivelManager->pausarNivel();
    timerFondo->stop();
    mostrarElementosJuego(false);

    mostrarPantallaVictoria(nivelCompletado);
}

void MainWindow::onNivelFallado(NivelManager::NivelID nivelID)
{
    qDebug() << "MainWindow: Nivel" << nivelID << "fallado. Terminando juego...";

    // 1. Notificar al GameManager que el juego terminó.
    gameManager->terminarJuego(); // Esto cambia el estado a GameOver

    // La lógica de detener la nave, ocultar HUD y mostrar pantalla se gestiona en
    // manejarEstadoJuego(GameOver)
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

void MainWindow::mostrarPantallaVictoria(NivelManager::NivelID nivelCompletado)
{
    QString titulo;
    QPixmap imagenVictoria;

    // --- 1. CONFIGURACIÓN DE PANTALLA DE VICTORIA ---

    if (nivelCompletado == NivelManager::Nivel1) {
        titulo = "¡Nivel 1 Completado!";
        qDebug() << "Cargando imagen de la Luna";
        imagenVictoria = QPixmap(GameConstants::Resources::LUNA);

        // Se re-conecta al final de la función para limpiar el mensaje
    } else if (nivelCompletado == NivelManager::Nivel2) {
        titulo = "¡Nivel 2 Completado!";
        imagenVictoria = QPixmap(GameConstants::Resources::BACKGROUND); // O alguna imagen de victoria final

        // Se re-conecta al final de la función para limpiar el mensaje
    }

    // --- 2. MANEJO DE IMAGEN  ---

    if (imagenVictoria.isNull()) {
        qWarning() << "ERROR: No se pudo cargar imagen de victoria para el nivel" << nivelCompletado;
        imagenVictoria = QPixmap(scene->width(), scene->height());
        imagenVictoria.fill(Qt::black);
    }

    QPixmap imagenEscalada = imagenVictoria.scaled(
        scene->width(),
        scene->height(),
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );

    // Mostrar la imagen como fondo.
    QBrush brush(imagenEscalada);
    scene->setBackgroundBrush(brush);
    scene->update();

    // --- 3. MOSTRAR TEXTO Y BOTÓN ---

    // Opcional: Mostrar un mensaje como QGraphicsTextItem sobre el fondo.
    QGraphicsTextItem *mensajeVictoria = new QGraphicsTextItem(titulo);
    mensajeVictoria->setDefaultTextColor(Qt::yellow);
    QFont font = mensajeVictoria->font();
    font.setBold(true);
    font.setPointSize(30);
    mensajeVictoria->setFont(font);

    // Centrar el texto
    qreal textWidth = mensajeVictoria->boundingRect().width();
    mensajeVictoria->setPos((scene->width() - textWidth) / 2, scene->height() / 4);
    scene->addItem(mensajeVictoria);

    // Mostrar botón de continuar
    btnContinuar->setText((nivelCompletado == NivelManager::Nivel1) ? "Continuar al Nivel 2" : "Finalizar Juego");
    btnContinuar->show();

    // Re-conectar el botón 'Continuar' con la limpieza adecuada.
    disconnect(btnContinuar, nullptr, nullptr, nullptr);
    if (nivelCompletado == NivelManager::Nivel1) {
        connect(btnContinuar, &QPushButton::clicked, [this, mensajeVictoria]() {
            if (mensajeVictoria) scene->removeItem(mensajeVictoria); delete mensajeVictoria;
            btnContinuar->hide();
            mostrarIntroNivel(NivelManager::Nivel2);
        });
    } else if (nivelCompletado == NivelManager::Nivel2) {
        connect(btnContinuar, &QPushButton::clicked, [this, mensajeVictoria]() {
            if (mensajeVictoria) scene->removeItem(mensajeVictoria); delete mensajeVictoria;
            onTodosLosNivelesCompletados();
        });
    }
}

void MainWindow::mostrarPantallaGameOver()
{
    // Limpiar escena de elementos de juego
    nivelManager->limpiarNivelActual();

    // Mostrar fondo de Game Over
    QPixmap gameOver(GameConstants::Resources::GAME_OVER);
    QGraphicsTextItem *mensajeGameOver = nullptr;

    if (gameOver.isNull()) {
        qWarning() << "ERROR: No se pudo cargar GAME_OVER. Usando fondo negro.";
        gameOver = QPixmap(GameConstants::SCENE_WIDTH, GameConstants::SCENE_HEIGHT);
        gameOver.fill(Qt::black);

        // Si no hay imagen, mostramos un mensaje de texto.
        mensajeGameOver = new QGraphicsTextItem("GAME OVER");
        mensajeGameOver->setDefaultTextColor(Qt::red);
        QFont font = mensajeGameOver->font();
        font.setBold(true);
        font.setPointSize(40);
        mensajeGameOver->setFont(font);
        qreal textWidth = mensajeGameOver->boundingRect().width();
        mensajeGameOver->setPos((scene->width() - textWidth) / 2, scene->height() / 4);
        scene->addItem(mensajeGameOver);
    } else {
        gameOver = gameOver.scaled(GameConstants::SCENE_WIDTH, GameConstants::SCENE_HEIGHT, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    // Establecer el fondo (imagen o negro)
    scene->setBackgroundBrush(QBrush(gameOver));
    scene->update();

    // El botón de Reiniciar se muestra en manejarEstadoJuego(GameOver)

    // Si se usó un mensaje de texto temporal, necesitamos limpiar la escena cuando se reinicie
    if (mensajeGameOver) {
        // Re-conectar el botón 'Comenzar' para que limpie el mensaje al reiniciar
        disconnect(btnComenzar, nullptr, nullptr, nullptr);
        connect(btnComenzar, &QPushButton::clicked, [this, mensajeGameOver]() {
            if (mensajeGameOver) scene->removeItem(mensajeGameOver); delete mensajeGameOver;
            btnComenzar->hide();
            gameManager->reiniciarJuego();
            mostrarIntroNivel(NivelManager::Nivel1);
        });
    }
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

    // El Game Over por vidas se gestiona en el Nivel, que emite nivelFallado.
    // Esta verificación es redundante si el Nivel lo hace, pero la mantenemos como fallback.
    if (vidasActuales <= 0 && gameManager->getEstado() == GameManager::Playing) {
        // Usamos nivelManager->pausarNivel() aquí para detener los timers del nivel
        // antes de que el GameManager cambie el estado.
        nivelManager->pausarNivel();
        gameManager->terminarJuego(); // Esto activa manejarEstadoJuego(GameOver)
    }
}

void MainWindow::mostrarElementosJuego(bool mostrar)
{
    if (textoVidas) textoVidas->setVisible(mostrar);

    for (auto corazon : corazones) {
        corazon->setVisible(false); // Se actualizarán en actualizarHUD
    }

    // Asegurar que los labels del Nivel 2 se oculten si no estamos en ese nivel o jugando
    if (!mostrar && nivelManager->getNivelActualID() == NivelManager::Nivel2) {
        Nivel2 *nivel2 = dynamic_cast<Nivel2*>(nivelManager->getNivelActual());
        if (nivel2 && nivel2->getLabelGemas()) {
            nivel2->getLabelGemas()->hide();
        }
    }
}

void MainWindow::limpiarEscena()
{
    // Limpia elementos de la escena que no pertenecen a un nivel específico
    // (Actualización: Esta función ahora se usa para limpiar cualquier mensaje temporal
    // cuando se vuelve al menú principal)
}
void MainWindow::mostrarIntroNivel(NivelManager::NivelID nivelID)
{
    qDebug() << "Mostrando intro del Nivel" << nivelID;

    // Detener el fondo scrolleable
    timerFondo->stop();

    // Ocultar elementos del HUD
    mostrarElementosJuego(false);
    btnContinuar->hide(); // Ocultar si estuviera visible

    // Ocultar botones del menú/gameover
    btnComenzar->hide();
    btnOpciones->hide();

    // Cargar la imagen de intro correspondiente
    QString rutaIntro;
    if (nivelID == NivelManager::Nivel1) {
        rutaIntro = GameConstants::Resources::NIVEL1_INTRO;
    } else if (nivelID == NivelManager::Nivel2) {
        rutaIntro = GameConstants::Resources::NIVEL2_INTRO; // <- Asegura que esta ruta esté correcta en constants.h
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
