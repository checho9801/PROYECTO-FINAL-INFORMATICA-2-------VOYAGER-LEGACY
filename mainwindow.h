#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "nave.h"
#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>
#include <QPixmap>
#include <QGraphicsTextItem>
#include <QFont>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void actualizarHUD(); // método para refrescar la label
    void mostrarLuna();

private slots:
    void actualizarFondo();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QPixmap fondoEscalado;
    int desplazamientoY;
    int velocidad;
    QTimer *timer;
    QTimer *meteorito;
    QTimer *acelerar;
    Nave *nave;
    QLabel *labelVidas;
};
#endif // MAINWINDOW_H
