#include "nivel3.h"
#include <QBrush>
#include <QTransform>
#include <QFont>
#include <QTimer>
#include "constants.h"
#include "enemigo.h"

Nivel3::Nivel3(QGraphicsScene *escena, QObject *parent)
    : NivelBase(escena, parent), naveN3(nullptr), enemigo(nullptr), timerFondo(nullptr)
{
    qDebug() << "=== NIVEL 3 CREADO ===";
}

Nivel3::~Nivel3()
{
    limpiar();
}

void Nivel3::iniciar()
{
    qDebug() << "=== NIVEL 3 INICIADO ===";

    scene->clear();

    // FONDO ANIMADO
    QPixmap fondo(GameConstants::Resources::NIVEL3_FONDO);
    if (!fondo.isNull()) {
        fondoEscalado = fondo.scaled(500, 1200, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QBrush brush(fondoEscalado);
        scene->setBackgroundBrush(brush);
    }

    // NAVE ✅ NaveN3 HEREDA de Nave → CONVERSIÓN IMPLÍCITA OK
    naveN3 = new NaveN3();
    scene->addItem(naveN3);
    naveN3->setPos(GameConstants::NAVE_INITIAL_X, GameConstants::NAVE_INITIAL_Y);
    naveN3->setFlag(QGraphicsItem::ItemIsFocusable);
    naveN3->setFocus();
    naveN3->activarDisparo(true);
    nave = dynamic_cast<Nave*>(naveN3);  // ✅ CAST EXPLÍCITO SEGURO

    // ENEMIGO
    enemigo = new Enemigo();
    scene->addItem(enemigo);
    enemigo->setPos(200, 50);

    // SEÑALES
    connect(naveN3, &NaveN3::gameOver, this, &Nivel3::mostrarGameOver);
    connect(enemigo, &Enemigo::enemigoDerrotado, this, &Nivel3::nivelSuperado);

    // FONDO TIMER
    timerFondo = new QTimer(this);
    connect(timerFondo, &QTimer::timeout, this, &Nivel3::actualizarFondo);
    timerFondo->start(16);
}

void Nivel3::detener()  // ✅ DECLARADO en .h
{
    qDebug() << "Nivel 3 detenido";
    if (timerFondo) timerFondo->stop();
}

void Nivel3::pausar()  // ✅ DECLARADO en .h
{
    if (timerFondo) timerFondo->stop();
}

void Nivel3::reanudar()  // ✅ DECLARADO en .h
{
    if (timerFondo) timerFondo->start(16);
}

void Nivel3::actualizar()  // ✅ DECLARADO en .h
{
    // Lógica de IA enemiga, etc.
}

bool Nivel3::estaCompletado() const  // ✅ DECLARADO en .h
{
    return enemigo == nullptr;  // Enemigo muerto = nivel completado
}

void Nivel3::limpiar()
{
    qDebug() << "Nivel 3 limpiado SEGURAMENTE";

    // Timers
    if (timerFondo) {
        timerFondo->stop();
        timerFondo->deleteLater();  // ✅ QTimer hereda QObject
        timerFondo = nullptr;
    }

    // Nave (NaveN3 hereda QObject?)
    if (naveN3) {
        if (scene) scene->removeItem(naveN3);
        delete naveN3;  // ✅ delete directo (NO deleteLater)
        naveN3 = nullptr;
        nave = nullptr;
    }

    // Enemigo
    if (enemigo) {
        if (scene) scene->removeItem(enemigo);
        delete enemigo;  // ✅ delete directo
        enemigo = nullptr;
    }

    // Balas - delete directo
    QList<QGraphicsItem*> items = scene->items();
    for (QGraphicsItem *item : items) {
        if (dynamic_cast<Bala*>(item) || dynamic_cast<BalaEnemiga*>(item)) {
            scene->removeItem(item);
            delete item;  // ✅ QGraphicsItem usa delete normal
        }
    }
}

void Nivel3::actualizarFondo()
{
    desplazamientoY += velocidadFondo;
    if (desplazamientoY >= 600) desplazamientoY = 0;

    QBrush brush(fondoEscalado);
    brush.setTransform(QTransform::fromTranslate(0, desplazamientoY));
    scene->setBackgroundBrush(brush);
}

void Nivel3::mostrarGameOver()
{
    QGraphicsTextItem *texto = scene->addText("GAME OVER", QFont("Arial", 30));
    texto->setDefaultTextColor(Qt::red);
    texto->setPos(120, 250);
    emit nivelFallado();
}

void Nivel3::nivelSuperado()
{
    qDebug() << "🎉 ¡JEFE FINAL DERROTADO! Emisión segura...";

    // ✅ MOSTRAR TEXTO PERO NO LIMPIAR AÚN
    QGraphicsTextItem *texto = scene->addText("¡NIVEL 3 SUPERADO! 🎉", QFont("Arial", 36, QFont::Bold));
    texto->setDefaultTextColor(Qt::yellow);
    texto->setPos(20, scene->height() / 2 - 40);

    // ✅ DETENER SOLO TIMERS (NO delete)
    if (timerFondo) {
        timerFondo->stop();
        // NO deleteLater() aquí!
    }

    // ✅ EMITIR señal DESPUÉS de 500ms (anti-race condition)
    QTimer::singleShot(500, this, [this]() {
        qDebug() << "Nivel3: Emitiendo nivelCompletado() seguro";
        emit nivelCompletado();
    });
}


