#ifndef NIVELMANAGER_H
#define NIVELMANAGER_H

#include "nivelbase.h"
#include <QObject>

// FORWARD DECLARATIONS ← ¡ESTO SOLUCIONA TODO!
class Nivel1;
class Nivel2;
class Nivel3;

class NivelManager : public QObject
{
    Q_OBJECT

public:
    enum class NivelID {
        Ninguno = 0,
        Nivel1 = 1,
        Nivel2 = 2,
        Nivel3 = 3
    };

    explicit NivelManager(QGraphicsScene *escena, QObject *parent = nullptr);
    ~NivelManager();

    void cargarNivel(NivelID nivelID);
    bool cargarSiguienteNivel();
    void pausarNivel();
    void reanudarNivel();
    NivelBase* getNivelActual();
    NivelID getNivelActualID() const { return nivelActualID; }
    bool haySiguienteNivel() const;

signals:
    void nivelCargado(NivelID nivelID);
    void nivelCompletado(NivelID nivelID);
    void nivelFallado(NivelID nivelID);
    void todosLosNivelesCompletados();

private slots:  // ✅ AGREGAR ESTA SECCIÓN
    void onNivelCompletado();
    void onNivelFallado();

private:
    NivelBase* crearNivel(NivelID nivelID);
    void limpiarNivelActual();

    QGraphicsScene *scene;
    NivelBase *nivelActual;
    NivelID nivelActualID;
};

#endif // NIVELMANAGER_H
