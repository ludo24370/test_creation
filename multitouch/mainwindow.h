#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "graphicsview.h"

#include <QMainWindow>
#include <QGraphicsScene>
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QWidget *fenetre;       // création de la fenetre
    QGraphicsScene *scene;  // création de la scene
    GraphicsView *view;     // création de la view
    QImage *image;          // création de l'image

    // size de l'image :
    int width_image_tot;
    int height_image_tot;

    // création des timer :
    QTimer *zoom_plus_pressed;
    QTimer *zoom_moins_pressed;
    QTimer *haut_pressed;
    QTimer *bas_pressed;
    QTimer *droite_pressed;
    QTimer *gauche_pressed;

    // création de tous les boutons :
    QPushButton *valeur;
    QPushButton *bouton_plus;
    QPushButton *bouton_moins;
    QPushButton *bouton_haut;
    QPushButton *bouton_bas;
    QPushButton *bouton_droite;
    QPushButton *bouton_gauche;

private slots:
    // fonctions de récupération de l'image et simpletouch :
    void get_size_image(void);
    void zoom_plus(void);
    void zoom_moins(void);
    void vers_le_haut(void);
    void vers_le_bas(void);
    void vers_la_droite(void);
    void vers_la_gauche(void);

    // fonctions permettant de conserver l'action en gardant les boutons appuyer :
    void timer_zoom_plus_start(void);
    void timer_zoom_plus_stop(void);

    void timer_zoom_moins_start(void);
    void timer_zoom_moins_stop(void);

    void timer_haut_start(void);
    void timer_haut_stop(void);

    void timer_bas_start(void);
    void timer_bas_stop(void);

    void timer_droite_start(void);
    void timer_droite_stop(void);

    void timer_gauche_start(void);
    void timer_gauche_stop(void);
};

#endif // MAINWINDOW_H
