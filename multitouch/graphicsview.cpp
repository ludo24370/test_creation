/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "graphicsview.h"
#include "mainwindow.h"

#include <QTouchEvent>
#include <QMessageBox>
#include <QString>

GraphicsView::GraphicsView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent), totalScaleFactor_old(1)
{
    viewport()->setAttribute(Qt::WA_AcceptTouchEvents); // autoriser les action de touch

    setDragMode(ScrollHandDrag); // on peut déplacer l'image en "l'attrapant" clicker, glisser relacher déplace l'image

    totalScaleFactor_old = 0.12641;   // un facteur multiplicateur pour afficher l'image en entière plutôt que tout et du coup déjà zoomé
    setTransform(QTransform().scale(totalScaleFactor_old,totalScaleFactor_old)); // effectuer la transformation de scale de l'image

    setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff ); // ne pas afficher les scrollbar pour un soucis d'estétisme
    setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );

    // deux multiplieur pour empécher de zoomer et dézoomer trop
    multiplieur_maxi = 0.5;         // Permet d'obtenir en ettant au max une image en 560*720 parfait pour l'impression
    multiplieur_mini = 0.12640;     // Permet d'affciher l'image sur tout sa hauteur dans la view, sinon il y a des bande blanche qui apparaissent
}

/*
 * la fonction drawForeground est une fonction virtuelle protegée de QGraphicsView
 * elle permet la création de l'ellipse et des traits de positionnement du calque
 *
 * Cette fonction est dépendente de la matrice de modification de la scène donc il faut l'en "détaché" pour qu'elle
 * ne soit pas déformée avec la scene.
 */
void GraphicsView::drawForeground(QPainter * painter, const QRectF & rect)
{
    painter->save();                                        // Je sauve le "contexte"
    painter->setWorldMatrixEnabled(false);
    painter->setRenderHint( QPainter::Antialiasing );       // Je commence le travail de dessin
    painter->setPen( Qt::white );                           // un Pen blanc pour dessiner l'ellipse en blanc

    QPen pen;                           // création d'un QPen pour faire les trait verticaux et horizontaux diférents de l'ellipse (pointillé)
    pen.setColor(Qt::white);
    pen.setStyle(Qt::DotLine);

    painter->drawEllipse(45,30,190,285);    // je dessine l'ellipse avec le Pen de base, blanc entier
    painter->setPen(pen);                   // je choisi le QPen créé juste avant, blanc en pointillé
    painter->drawLine(0,103,280,103);       // je dessine les 2 lignes verticale (P1(x), P1(y), P2(x), P2(y))
    painter->drawLine(0,183,280,183);
    painter->drawLine(140,0,140,360);       // je dessine la ligne verticale

    painter->setWorldMatrixEnabled(true);       // je restaure le contexte
    painter->restore();
    QGraphicsView::drawForeground(painter, rect);   // je peint mon painter dans le rect de la view
}

/*
 * la fonction viewportEvent est une fonction virtuelle protegée de QGraphicsView
 *
 * elle est lancée à chaque event sur la view, dans notre cas à chaque touch
 */
bool GraphicsView::viewportEvent(QEvent *event)
{
    // On choisie de récupérer 3 types d'évent : début du touché, déplacement du touché, et fin du touché
    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    {
        QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
        // Enregistrement du nombre de touchpoints (les doigts) dans un QList
        // tous les touchpoint sont enregistré
        QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();

        if (touchPoints.count() == 1)
            viewport()->update();           // J'update la view pour recréer l'ellipse du calque

        if (touchPoints.count() == 2)   // si j'ai 2 touchpoint je fait le multitouch à proprement parlé
        {
            // détermination du scale factor
            const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();   // le touchPoint0 sera le first touchpoint de la list
            const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();    // le touchPoint1 sera le last touchpoint de la list car la dalle n'en supporte que 2

            /*
             * le currentScaleFactor est le coef multiplicateur en temps réel, il est déterminer comme ceci :
             * 1) calcul de la position initiale des deux doigts
             * 2) calcul de la distance les séparants : dist_init
             * 3) calcul de la position courante des deux doigts
             * 4) calcul de la distance les séparants : dist_cour
             * 5) currentScaleFactor = dist_cour/dist_init
             */
            currentScaleFactor =
                    QLineF(touchPoint0.pos(), touchPoint1.pos()).length()
                    / (QLineF(touchPoint0.startPos(), touchPoint1.startPos()).length());

            totalScaleFactor = totalScaleFactor_old * currentScaleFactor;   // calcul du scalefactor courant en multipliant le scalefactor initial pour le current

            if (totalScaleFactor > multiplieur_maxi)        // si le scalefactor total courant dépasse une valeur max de zoom on stop le zoom
            {
                totalScaleFactor_old = multiplieur_maxi;    // totalscalefactor est bloquer au max et vaux le max
                currentScaleFactor = 1;                     // current est set à 1
                touchPoints.removeFirst();                  // on delete les touchpoints pour éviter des désagréments
                touchPoints.removeLast();
            }
            if (totalScaleFactor < multiplieur_mini)        // si le scalefactor total courant passe en dessous d'une valeur min de zoom on stop le dézoom
            {
                totalScaleFactor_old = multiplieur_mini;    // totalscalefactor est bloquer au min et vaux le min
                currentScaleFactor = 1;                     // current est set à 1
                touchPoints.removeFirst();                  // on delete les touchpoints pour éviter des désagréments
                touchPoints.removeLast();
            }

            if (touchEvent->touchPointStates() & Qt::TouchPointReleased)
            {
                // if one of the fingers is released, remember the current scale
                // factor so that adding another finger later will continue zooming
                // by adding new scale factor to the existing remembered value.
                totalScaleFactor_old *= currentScaleFactor;
                currentScaleFactor = 1;
            }

            // effectue la transformation en temp réel de ce que je regarde
            setTransform(QTransform().scale(totalScaleFactor_old * currentScaleFactor,
                                            totalScaleFactor_old * currentScaleFactor));

            viewport()->update();           // J'update la view pour recréer l'ellipse du calque
        }
        return true;
    }
    default:
        break;
    }
    return QGraphicsView::viewportEvent(event);
}
