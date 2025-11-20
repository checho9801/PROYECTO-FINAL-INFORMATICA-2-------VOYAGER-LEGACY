#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QBrush>
#include <QTransform>
#include "meteorito.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Crear escena
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    // Tamaño de la escena
    scene->setSceneRect(10, 10, 500, 600);

    desplazamientoY = 0;
    velocidad = 2;

    // --------- FONDO REPETIDO (INFINITO) -----------
    QPixmap fondo(":/imagen/Space.png");
    fondoEscalado = fondo.scaled(500, 600, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    // Asignar fondo inicial
    QBrush brush(fondoEscalado);

    // Timer para actualizar
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::actualizarFondo);
    connect(timer, &QTimer::timeout, this, &MainWindow::actualizarHUD);
    timer->start(16);

    // Crear la nave y añadirla a la escena
    nave = new Nave();
    scene->addItem(nave);

    meteorito = new QTimer(this);
    connect(meteorito, &QTimer::timeout, [this]() {
        Meteorito *m = new Meteorito(nave);
        scene->addItem(m);
    });
    meteorito->start(2000);

    // Timer que acelera la aparición de meteoritos
    acelerar = new QTimer(this);
    connect(acelerar, &QTimer::timeout, [this]() {
        int currentInterval = meteorito->interval();

        // Reducir el intervalo poco a poco
        if (currentInterval > 900) { // límite mínimo: 1 segundos
            meteorito->setInterval(currentInterval - 200);
        }
    });
    acelerar->start(7000); // cada 7 segundos reduce la espera

    // Crear el QLabel directamente en la ventana principal (parentWidget)
    labelVidas = new QLabel("Vidas: " + QString::number(nave->getVidas()), this);
    labelVidas->setStyleSheet("color: Black; font-size: 20px;");
    labelVidas->move(100, 80);
    labelVidas->show();

    QTimer *meta = new QTimer(this);
    connect(meta, &QTimer::timeout, [this]() {
        mostrarLuna();
    });
    meta->start(60000);
}

void MainWindow::mostrarLuna() {
    // Detener timers principales
    timer->stop();
    meteorito->stop();
    acelerar->stop();

    // Eliminar meteoritos existentes
    for (QGraphicsItem *item : scene->items()) {
        Meteorito *m = dynamic_cast<Meteorito*>(item);
        if (m) {
            scene->removeItem(m);
            m->deleteLater();
        }
    }

    // Cargar imagen de la Luna
    QPixmap luna(":/imagen/luna.png");
    QPixmap lunaEscalada = luna.scaled(scene->width(), scene->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    // Asignar como fondo completo
    QBrush brush(lunaEscalada);
    scene->setBackgroundBrush(brush);

    // Opcional: mostrar mensaje
    QMessageBox::information(this, "¡Meta alcanzada!", "Has llegado a la Luna 🌕");

    // Terminar la aplicación
    qApp->quit();
}

void MainWindow::actualizarHUD() {
    labelVidas->setText("Vidas: " + QString::number(nave->getVidas()));
    if (nave->getVidas() <= 0) {
        labelVidas->setText("Game Over");

        // Mostrar un mensaje emergente
        QMessageBox::information(this, "Fin del juego", "¡Has perdido todas las vidas!");

        // Terminar la aplicación
        qApp->quit();
    }
}

void MainWindow::actualizarFondo()
{
    desplazamientoY += velocidad;
    if (desplazamientoY >= fondoEscalado.height())
        desplazamientoY = 0;

    QBrush brush(fondoEscalado);
    brush.setTransform(QTransform::fromTranslate(0, desplazamientoY));
    scene->setBackgroundBrush(brush);
}

MainWindow::~MainWindow()
{
    delete ui;
}


