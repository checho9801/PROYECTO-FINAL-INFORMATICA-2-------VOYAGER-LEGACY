#include "nivelmanager.h"
#include "nivel1.h"
#include "nivel2.h"
#include <QDebug>

NivelManager::NivelManager(QGraphicsScene *escena, QObject *parent)
    : QObject(parent),
    scene(escena),
    nivelActual(nullptr),
    nivelActualID(Ninguno)
{
}

NivelManager::~NivelManager()
{
    limpiarNivelActual();
}

bool NivelManager::cargarNivel(NivelID nivelID)
{
    qDebug() << "=== CARGANDO NIVEL" << nivelID << "===";

    // Limpiar nivel anterior
    limpiarNivelActual();

    // Crear nuevo nivel
    nivelActual = crearNivel(nivelID);

    if (!nivelActual) {
        qWarning() << "ERROR: No se pudo crear el nivel" << nivelID;
        return false;
    }

    nivelActualID = nivelID;

    // Conectar señales del nivel
    connect(nivelActual, &NivelBase::nivelCompletado, this, &NivelManager::onNivelCompletado);
    connect(nivelActual, &NivelBase::nivelFallado, this, &NivelManager::onNivelFallado);

    // Iniciar el nivel
    nivelActual->iniciar();

    emit nivelCargado(nivelID);
    return true;
}

void NivelManager::pausarNivel()
{
    if (nivelActual) {
        nivelActual->pausar();
    }
}

void NivelManager::reanudarNivel()
{
    if (nivelActual) {
        nivelActual->reanudar();
    }
}

bool NivelManager::haySiguienteNivel() const
{
    // Verificar si existe un nivel después del actual
    return nivelActualID < NivelFinal;
}

bool NivelManager::cargarSiguienteNivel()
{
    if (!haySiguienteNivel()) {
        qDebug() << "No hay más niveles disponibles";
        emit todosLosNivelesCompletados();
        return false;
    }

    NivelID siguienteID = static_cast<NivelID>(nivelActualID + 1);
    return cargarNivel(siguienteID);
}

void NivelManager::onNivelCompletado()
{
    qDebug() << "Nivel" << nivelActualID << "completado";
    emit nivelCompletado(nivelActualID);
}

void NivelManager::onNivelFallado()
{
    qDebug() << "Nivel" << nivelActualID << "fallado";
    emit nivelFallado(nivelActualID);
}

void NivelManager::limpiarNivelActual()
{
    if (nivelActual) {
        nivelActual->detener();
        nivelActual->limpiar();
        nivelActual->deleteLater();
        nivelActual = nullptr;
    }

    nivelActualID = Ninguno;
}

NivelBase* NivelManager::crearNivel(NivelID nivelID)
{
    switch (nivelID) {
    case Nivel1:
        return new ::Nivel1(scene, this);

    case Nivel2:
        return new ::Nivel2(scene, this);

    case NivelFinal:
        // Por ahora no existe, pero puedes agregarlo después
        qWarning() << "Nivel Final no implementado aún";
        return nullptr;

    default:
        qWarning() << "Nivel desconocido:" << nivelID;
        return nullptr;
    }
}
