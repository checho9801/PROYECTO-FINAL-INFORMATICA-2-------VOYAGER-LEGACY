#include "nivelbase.h"
#include "nave.h" // Necesario para instanciar 'new Nave()'
// Si usas 'constants.h' debes incluirlo también

NivelBase::NivelBase(QGraphicsScene *escena, QObject *parent)
    : QObject(parent),
    scene(escena),
    nave(nullptr),
    pausado(false),
    completado(false)
{
}

// 1. **ERROR RESUELTO AQUÍ:** El destructor debe llamar a la versión base de limpiar().
// Dado que limpiar() es virtual pura, debe tener una definición aquí.
// Pero como las clases derivadas hacen la limpieza real, la implementación base
// puede ser vacía o limitarse a llamar a 'limpiar()'.
NivelBase::~NivelBase()
{
    // Limpiar elementos específicos del nivel actual si es necesario.
    // Aunque limpiar() es virtual pura, se debe definir
    // si se llama desde el destructor.
    limpiar();
}

// 2. **DEFINICIÓN DE LA FUNCIÓN VIRTUAL PURA:** // Debemos darle una implementación, aunque sea vacía, para que el enlazador la encuentre.
void NivelBase::limpiar()
{
    // Implementación base vacía. La lógica de limpieza real debe estar
    // en las clases derivadas (Nivel1::limpiar(), Nivel2::limpiar(), etc.).
    // Esto evita el "undefined reference to `NivelBase::limpiar()'"
}

void NivelBase::inicializarNave()
{
    if (!nave && scene) {
        // Asegúrate de que la clase Nave haya sido definida e incluida
        nave = new Nave();
        scene->addItem(nave);
        nave->setFocus();
    }
}
