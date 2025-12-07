#ifndef BALA_H
#define BALA_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>

class Bala : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit Bala(QGraphicsItem *parent = nullptr);

    void limpiarTotal();

public slots:
    void mover();

private:
    QTimer *timer;
};

#endif
