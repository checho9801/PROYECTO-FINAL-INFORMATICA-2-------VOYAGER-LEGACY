#ifndef NAVE_H
#define NAVE_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QKeyEvent>

class Nave : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit Nave(QGraphicsItem *parent = nullptr);
    void restarVida();
    int getVidas() const;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    int velocidad;
    int vidas;
};

#endif // NAVE_H
