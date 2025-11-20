#ifndef METEORITO_H
#define METEORITO_H

#include "nave.h"
#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>

class Meteorito : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit Meteorito(Nave *naveRef, QGraphicsItem *parent = nullptr);

private slots:
    void mover();

private:
    int velocidad;
    QTimer *timer;
    Nave *nave;
};

#endif // METEORITO_H
