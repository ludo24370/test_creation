#include "graphicsview.h"
#include "mainwindow.h"

#include <QtGui>

#include <math.h>

#include <QHBoxLayout>
#include <QApplication>
#include <QImage>
#include <QPushButton>
#include <QWidget>
#include <QObject>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    fenetre = new QWidget(); // création de la fenetre globale

    scene = new QGraphicsScene(this); // création de la scene

    view = new GraphicsView(); // création de la view

    width_image_tot = 0;
    height_image_tot = 0;

    view->setScene(scene);  // ajout de la scene dans la view
    view->centerOn(2136, 1424); // je centre mon image qui fait 4272*2848
    view->setFixedSize(280,360);// je force la taille de ma view dans ma fenetre à 280*360

    image = new QImage();   // création de l'image

    if (image->load("../lulu.JPG")==true)       // test de chargement de l'image, si vraie
    {
        width_image_tot = image->width();       // j'enregistre la largeur et la hauteur de l'image dans les variable
        height_image_tot = image->height();     // width_image_tot et height_image_tot
    }
    else                            // Si erreur de chargement j'affiche une QBox et ...
    {
        QMessageBox *box = new QMessageBox();
        box->setText("erreur lors du chargement de l'image");
        box->show();
    }

    QPixmap pixmap_image = QPixmap::fromImage(*image);      // je transforme l'image en pixmap et je la met dans ma scene
    scene->addPixmap(pixmap_image);

    valeur = new QPushButton("test, bouton de lulu :");     // création du boutons qui permet d'enregistrer l'image

    bouton_plus = new QPushButton("plus");                  // Création des boutons de l'IHM simpletouch
    bouton_moins = new QPushButton("moins");
    bouton_haut = new QPushButton("haut");
    bouton_bas = new QPushButton("bas");
    bouton_droite = new QPushButton("droite");
    bouton_gauche = new QPushButton("gauche");

    /*
     * Création de 6 timer (zoom_plus_pressed, zoom_moins_pressed, haut_pressed, bas_pressed,
     * droite_pressed, gauche_pressed)
     *
     * Ces timer ont pour but de créer une répétition de click sur les bouton de l'IHM (Zoom + -, haut, bas, droite, gauche)
     * pour éviter d'avoir à clicker un nombre de fois incalculable sur les boutons pour ce déplacer.
     */

    zoom_plus_pressed = new QTimer;
    connect(zoom_plus_pressed, SIGNAL(timeout()), this, SLOT(zoom_plus()));

    zoom_moins_pressed = new QTimer;
    connect(zoom_moins_pressed, SIGNAL(timeout()), this, SLOT(zoom_moins()));

    haut_pressed = new QTimer;
    connect(haut_pressed, SIGNAL(timeout()), this, SLOT(vers_le_haut()));

    bas_pressed = new QTimer;
    connect(bas_pressed, SIGNAL(timeout()), this, SLOT(vers_le_bas()));

    droite_pressed = new QTimer;
    connect(droite_pressed, SIGNAL(timeout()), this, SLOT(vers_la_droite()));

    gauche_pressed = new QTimer;
    connect(gauche_pressed, SIGNAL(timeout()), this, SLOT(vers_la_gauche()));

    QVBoxLayout *layout_principal = new QVBoxLayout;    // création du layout pour organiser ma fenetre
    QGridLayout *layout_photo = new QGridLayout;        // création du grid layout pour l'IHM simpletouch

    layout_photo->addWidget(view,1,1);              // J'ajoute à mon gridlayout ma view et mes bouton
    layout_photo->addWidget(bouton_plus,3,2);
    layout_photo->addWidget(bouton_moins,3,0);
    layout_photo->addWidget(bouton_haut,0,1);
    layout_photo->addWidget(bouton_bas,2,1);
    layout_photo->addWidget(bouton_droite,1,2);
    layout_photo->addWidget(bouton_gauche,1,0);

    layout_principal->addLayout(layout_photo);      // j'ajoute ma gridlayout à mon layout pricipal
    layout_principal->addWidget(valeur);            // j'ajoute le bouton de test à mon layout pricipal

    // je connecte mon bouton valeur à la fonction get_size_image
    connect(valeur, SIGNAL(clicked()), this, SLOT(get_size_image()));

    /*
     * Sur tous les bouton de l'IHM simpletouch je crée 3 connect :
     *
     * - un connect clicked qui renvoie à la fonction associée (bouton plus -> zoom plus)
     * - un connect pressed qui renvoie à la fonction timer start associé au bouton pour démarrer le timer qui éffectue
     *   l'action du bouton toutes les 75ms
     * - un connect released qui renvoie à la fonction timer stop associé au bouton pour arréter le timer
     */

    connect(bouton_plus, SIGNAL(clicked()), this, SLOT(zoom_plus()));       // je renvoie à zoom plus
    connect(bouton_plus, SIGNAL(pressed()), this, SLOT(timer_zoom_plus_start()));   // Je lance le Timer de la fonction zoom plus
    connect(bouton_plus, SIGNAL(released()), this, SLOT(timer_zoom_plus_stop()));   // j'arrête le Timer de la fonction zoom plus

    connect(bouton_moins, SIGNAL(clicked()), this, SLOT(zoom_moins()));     // idem avec zoom moins et etc...
    connect(bouton_moins, SIGNAL(pressed()), this, SLOT(timer_zoom_moins_start()));
    connect(bouton_moins, SIGNAL(released()), this, SLOT(timer_zoom_moins_stop()));

    connect(bouton_haut, SIGNAL(clicked()), this, SLOT(vers_le_haut()));
    connect(bouton_haut, SIGNAL(pressed()), this, SLOT(timer_haut_start()));
    connect(bouton_haut, SIGNAL(released()), this, SLOT(timer_haut_stop()));

    connect(bouton_bas, SIGNAL(clicked()), this, SLOT(vers_le_bas()));
    connect(bouton_bas, SIGNAL(pressed()), this, SLOT(timer_bas_start()));
    connect(bouton_bas, SIGNAL(released()), this, SLOT(timer_bas_stop()));

    connect(bouton_droite, SIGNAL(clicked()), this, SLOT(vers_la_droite()));
    connect(bouton_droite, SIGNAL(pressed()), this, SLOT(timer_droite_start()));
    connect(bouton_droite, SIGNAL(released()), this, SLOT(timer_droite_stop()));

    connect(bouton_gauche, SIGNAL(clicked()), this, SLOT(vers_la_gauche()));
    connect(bouton_gauche, SIGNAL(pressed()), this, SLOT(timer_gauche_start()));
    connect(bouton_gauche, SIGNAL(released()), this, SLOT(timer_gauche_stop()));

    fenetre->setLayout(layout_principal);   // je charge le layout dans la fenetre

    fenetre->setGeometry(100,100,1366,768); //j'affiche ma fentre
    fenetre->show();
}

MainWindow::~MainWindow()
{

}

/*
 * La fonction get_size_image récupère les information de ce que je regarde (les coordonnées des pixels affiché dans la view)
 * et le coefficient multiplicateur appliqué à mon zoom.
 *
 * Les coordonnée du point (0,0) et le coef multiplicateur appliquer au rectangle 280*360 donne valeur_x, valeur_y du point (0,0)
 * width_regarde et height_regarde la largeur et la hauteur du rectangle que je regarde et que je veux enregistrer
 *
 * Enfin, comme les résultats envoyé par la view sont en qreal, je doit vérifier que les valeurs finales donnent une image en 7/9
 * et qui ne dépasse pas des bord de l'image, je réalise donc des tests et je récupère une image recalculé au pixel près.
 */

void MainWindow::get_size_image(void)
{
    int i = 0;              // compteur utilisé pour déterminer la largeur et la hauteur parfaite en comparaison à ce que je
                            // regarde, width doit être un multiple de 7 donc je vérifie cela grâce à ce i un peu plus loin

    qreal ScaleFactor = view->totalScaleFactor_old;         // ScaleFactor est le coef multiplicateur de la view
    qreal valeur_x_real = view->mapFromScene(0,0).x();      // valeur_x_real récupère la valeur x de (0,0)
    qreal valeur_y_real = view->mapFromScene(0,0).y();      // valeur_y_real récupère la valeur y de (0,0)

    valeur_x_real = -(valeur_x_real / ScaleFactor);
    // je divise par ScaleFactor pour avoir cette valeur sur l'image de base et je multiplie par -1 car le point (0,0)
    // est en haut à gauche et que le sytème de coordonnée pointe x vers le haut et y vers la gauche
    valeur_y_real = -(valeur_y_real / ScaleFactor);         // idem pour y

    qreal width_regarde = 280/ScaleFactor;                  // La largeur de ce que je regarde est égale aux 280px de ma view divisé par le ScaleFactor
    qreal height_regarde;

    int valeur_x = (int) valeur_x_real;     // je prends la partie entière de valeur_x_real
    int valeur_y = (int) valeur_y_real;     // idem pour y

    /*
     * width_regarde est un réel donc sa partie entière ne sera pas un multiple de 7 car on divise un entier par une valeur
     * déjà arrondie d'un réel contenant beaucoup de décimale.
     *
     * je vérifie donc si width_regarde > 7(i+1), si non, je fait i++ si oui je sort de la boucle et je force
     * width_regarde à 7*i et height_regarde à 9*i
     */
    while (width_regarde > 7*(i+1))
        i++;

    width_regarde = 7*i;
    height_regarde = 9*i;

    // je vérifie si je ne regarde pas plus que la hauteure de l'image, si oui i-- et je recalcule width et height
    if (height_regarde > height_image_tot)
    {
        i--;
        width_regarde = 7*i;
        height_regarde = 9*i;
    }

    // je vérifie si je ne regarde pas trop "loin", si valeur_x + width_regarde > width_image_tot je diminue valeur_x
    while(valeur_x + width_regarde > width_image_tot)
        valeur_x--;

    // idem pour y
    while(valeur_y + height_regarde > height_image_tot)
        valeur_y--;

    // j'enregistre enfin ce que j'ai obtenue dans une image qui est la copy d'une partie de l'image originelle, le rectangle
    // (valeur_x, valeur_y, width_regarde, height_regarde)
    QImage *image_finale = new QImage(image->copy(valeur_x, valeur_y, width_regarde, height_regarde));
    image_finale->save(QString("../images/traitement_image/current_img.jpg"));

    QImage *image_vignette = new QImage(image_finale->scaled(QSize(200,252)));
    image_vignette->save(QString("../images/traitement_image/vignette.jpg"));
}

/*
 * la fonction zoom_plus permet le zoom simpletouch
 */
void MainWindow::zoom_plus(void)
{
    if (view->totalScaleFactor_old <= (view->multiplieur_maxi-0.003)) // test la veuleur du coef actuel, si je ne suis pas au max je peux zoomer
    {
        view->totalScaleFactor_old += 0.003;            // j'augmente alors la valeur du coefficient multiplicateur totalScaleFactor_old
        view->setTransform(QTransform().scale(view->totalScaleFactor_old,view->totalScaleFactor_old));  // je recrée la view avec le nouveau coef
    }
}

/*
 * la fonction zoom_moins permet le dézoom simpletouch
 */
void MainWindow::zoom_moins(void)
{
    if (view->totalScaleFactor_old >= (view->multiplieur_mini+0.003)) // test la veuleur du coef actuel, si je ne suis pas au min je peux dézoomer
    {
        view->totalScaleFactor_old -= 0.003;            // je diminue alors la valeur du coefficient multiplicateur totalScaleFactor_old
        view->setTransform(QTransform().scale(view->totalScaleFactor_old,view->totalScaleFactor_old));  // je recrée la view avec le nouveau coef
    }
}

/*
 * la fonction vers_le_haut déplace la view vers le haut
 */
void MainWindow::vers_le_haut(void)
{
    QScrollBar* yPos = view->verticalScrollBar();
    qreal Valeur_y = yPos->sliderPosition();        // Je récupère la valeur verticale de la scrollBar donc de ce que je regarde
    yPos->setValue((int) Valeur_y - 1);             // je modifie sa valeur, je fait -1 pour remonter d'un px
    view->viewport()->update();                     // J'update la view pour recréer l'ellipse du calque
}

/*
 * la fonction vers_le_bas déplace la view vers le bas
 */
void MainWindow::vers_le_bas(void)
{
    QScrollBar* yPos = view->verticalScrollBar();
    qreal Valeur_y = yPos->sliderPosition();        // Je récupère la valeur verticale de la scrollBar donc de ce que je regarde
    yPos->setValue((int) Valeur_y + 1);             // je modifie sa valeur, je fait +1 pour remonter d'un px
    view->viewport()->update();                     // J'update la view pour recréer l'ellipse du calque
}

/*
 * la fonction vers_la_droite déplace la view vers la droite
 */
void MainWindow::vers_la_droite(void)
{
    QScrollBar* xPos = view->horizontalScrollBar();
    qreal Valeur_x = xPos->sliderPosition();        // Je récupère la valeur horizontale de la scrollBar donc de ce que je regarde
    xPos->setValue((int) Valeur_x + 1);             // je modifie sa valeur, je fait +1 pour me déplacer d'un px vers la droite
    view->viewport()->update();                     // J'update la view pour recréer l'ellipse du calque
}

/*
 * la fonction vers_la_gauche déplace la view vers la gauche
 */
void MainWindow::vers_la_gauche(void)
{
    QScrollBar* xPos = view->horizontalScrollBar();
    qreal Valeur_x = xPos->sliderPosition();        // Je récupère la valeur horizontale de la scrollBar donc de ce que je regarde
    xPos->setValue((int) Valeur_x - 1);             // je modifie sa valeur, je fait -1 pour me déplacer d'un px vers la gauche
    view->viewport()->update();                     // J'update la view pour recréer l'ellipse du calque
}

/*
 * Les 12 fonctions suivantes sont les fonctions qui lancent les timers de l'IHM simpletouch
 * tous ces timers sont créé plus haut et ici on voit qu'ils sont start toutes les 75ms
 * ce qui équivaut à environ 13 click à la seconde
 */

// timer du bouton zoom plus
void MainWindow::timer_zoom_plus_start(void)    {zoom_plus_pressed->start(75);}

void MainWindow::timer_zoom_plus_stop(void)     {zoom_plus_pressed->stop();}

// timer du bouton zoom moins
void MainWindow::timer_zoom_moins_start(void)   {zoom_moins_pressed->start(75);}

void MainWindow::timer_zoom_moins_stop(void)    {zoom_moins_pressed->stop();}

// timer du bouton vers le haut
void MainWindow::timer_haut_start(void)         {haut_pressed->start(75);}

void MainWindow::timer_haut_stop(void)          {haut_pressed->stop();}

// timer du bouton vers le bas
void MainWindow::timer_bas_start(void)          {bas_pressed->start(75);}

void MainWindow::timer_bas_stop(void)           {bas_pressed->stop();}

// timer du bouton vers la droite
void MainWindow::timer_droite_start(void)       {droite_pressed->start(75);}

void MainWindow::timer_droite_stop(void)        {droite_pressed->stop();}

// timer du bouton vers la gauche
void MainWindow::timer_gauche_start(void)       {gauche_pressed->start(75);}

void MainWindow::timer_gauche_stop(void)        {gauche_pressed->stop();}
