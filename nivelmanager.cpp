#include "nivelmanager.h"
#include "nivel1.h"
#include "nivel2.h"
#include "nivel3.h"
#include <QDebug>

NivelManager::NivelManager(QGraphicsScene *escena, QObject *parent)
    : QObject(parent), scene(escena), nivelActual(nullptr), nivelActualID(NivelID::Ninguno)
{
    qDebug() << "✅ NivelManager inicializado";
}

NivelManager::~NivelManager()
{
    limpiarNivelActual();
}

NivelBase* NivelManager::getNivelActual()
{
    return nivelActual;
}

void NivelManager::onNivelCompletado()
{
    qDebug() << "NivelManager: Nivel" << static_cast<int>(nivelActualID) << "completado";

    // ✅ NO tocar nivelActual aquí
    emit nivelCompletado(nivelActualID);
}


void NivelManager::onNivelFallado()
{
    qDebug() << "Nivel" << static_cast<int>(nivelActualID) << "fallado";
    emit nivelFallado(nivelActualID);
}

void NivelManager::cargarNivel(NivelID nivelID)
{
    limpiarNivelActual();

    switch (nivelID) {
    case NivelID::Nivel1: nivelActual = new Nivel1(scene, this); break;
    case NivelID::Nivel2: nivelActual = new Nivel2(scene, this); break;
    case NivelID::Nivel3: nivelActual = new Nivel3(scene, this); break;
    }

    // ✅ TIPOS COINCIDEN: void() → void()
    connect(nivelActual, &NivelBase::nivelCompletado, this, &NivelManager::onNivelCompletado);
    connect(nivelActual, &NivelBase::nivelFallado, this, &NivelManager::onNivelFallado);

    nivelActualID = nivelID;
    nivelActual->iniciar();
    emit nivelCargado(nivelID);
}


NivelBase* NivelManager::crearNivel(NivelManager::NivelID nivelID)
{
    switch (nivelID) {
    case NivelID::Nivel1:
        return new Nivel1(scene, this);
    case NivelID::Nivel2:
        return new Nivel2(scene, this);
    case NivelID::Nivel3:
        return new Nivel3(scene, this);
    default:
        qWarning() << "❌ Nivel desconocido:" << static_cast<int>(nivelID);
        return nullptr;
    }
}

void NivelManager::pausarNivel()
{
    if (nivelActual) nivelActual->pausar();
}

void NivelManager::reanudarNivel()
{
    if (nivelActual) nivelActual->reanudar();
}

bool NivelManager::haySiguienteNivel() const
{
    return nivelActualID < NivelID::Nivel3;
}

bool NivelManager::cargarSiguienteNivel()
{
    if (!haySiguienteNivel()) {
        qDebug() << "🏆 No hay más niveles - VICTORIA TOTAL!";
        emit todosLosNivelesCompletados();
        return false;
    }

    NivelID siguiente = static_cast<NivelID>(static_cast<int>(nivelActualID) + 1);
    qDebug() << "Cargando siguiente nivel:" << static_cast<int>(siguiente);
    cargarNivel(siguiente);
    return true;
}


void NivelManager::limpiarNivelActual()
{
    if (nivelActual) {
        nivelActual->detener();
        nivelActual->limpiar();
        delete nivelActual;
        nivelActual = nullptr;
    }
    nivelActualID = NivelID::Ninguno;
}
