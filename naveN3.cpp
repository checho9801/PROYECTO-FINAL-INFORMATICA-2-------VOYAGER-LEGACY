#include "naveN3.h"
#include <QGraphicsScene>
#include "bala.h"

NaveN3::NaveN3(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent),
    velocidad(13) // píxeles por movimiento
{
    // Cargar imagen de la nave
    setPixmap(QPixmap(":/new/imagen/images/voyager.png").scaled(100, 100, Qt::KeepAspectRatio,Qt::SmoothTransformation));

    // Posición inicial
    setPos(260, 506);

    // Permitir que reciba eventos de teclado
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();

    vidas = 3;
    barraVida = new BarraVida(this);
    barraVida->setPos(pixmap().width()/2 - 30, -15);  // encima de la nave
    barraVida->actualizar(vidas, vidaMax);

    musicPlayer = new QMediaPlayer();
    audioOutput = new QAudioOutput();

    musicPlayer->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.5);

}

void NaveN3::restarVida()
{
    vidas--;
    barraVida->actualizar(vidas, vidaMax);

    if (vidas <= 0) {
        emit gameOver();
    }
}

int NaveN3::getVidas() const
{
    return vidas;
}

void NaveN3::reproducirSonidoBala()
{
    musicPlayer->setSource(QUrl("qrc:/new/audios/sounds/laser.mp3"));
    musicPlayer->play();
}

void NaveN3::activarDisparo(bool activar)
{
    puedeDisparar = activar;
}

void NaveN3::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_A:
        if (x() > 0)
            setPos(x() - velocidad, y());
        break;
    case Qt::Key_D:
        if (x() + pixmap().width() < scene()->width())
            setPos(x() + velocidad, y());
        break;
    case Qt::Key_Space: {

        if (!puedeDisparar) return;

        Bala *bala = new Bala();
        reproducirSonidoBala();
        bala->setPos(x() + pixmap().width()/2 - bala->pixmap().width()/2,y() - bala->pixmap().height());
        scene()->addItem(bala);
        break;
    }
    default:
        break;
    }
}

void NaveN3::limpiarTotal()
{
    if (barraVida) {
        delete barraVida;
        barraVida = nullptr;
    }

    if (musicPlayer) {
        musicPlayer->stop();
        musicPlayer->deleteLater();
        musicPlayer = nullptr;
    }

    if (audioOutput) {
        audioOutput->deleteLater();
        audioOutput = nullptr;
    }
}
