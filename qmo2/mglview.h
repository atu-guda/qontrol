/***************************************************************************
                          mglview.h  -  window to show plots via MathGL
                             -------------------
    begin                : 2014.11.29
    copyright            : (C) 2014-2014 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MGLVIEW_H
#define MGLVIEW_H

#include <QString>
#include <QWidget>
#include <QColor>
#include <QList>
#include <mgl2/qt.h>

class TGraph;
class TModel;
class QMathGL;
class mglData;
class mglGraph;
class mglDraw;

//* inner helper class to draw MathGL plot

class MglDrawer : public mglDraw
{
  public:
   MglDrawer( TGraph *agra );
   virtual ~MglDrawer() override;
   virtual int Draw( mglGraph *gr ) override;
   virtual void Reload() override;
  protected:
   TGraph *gra;
};

/**can plot graphs for TModel on info by TGraph
  *@author atu
  */

class MglView : public QWidget  {
 Q_OBJECT
 public:
   MglView( TGraph *agra, QWidget *parent );
   ~MglView();
   virtual QSize sizeHint() const override;

 public slots:

 protected:
   // virtual void paintEvent( QPaintEvent *pe );
   // virtual void mousePressEvent( QMouseEvent *me );
   // virtual void keyPressEvent( QKeyEvent *ke );
 protected:
   // TGraph *gra;
   // TModel *model = nullptr;
   MglDrawer *drawer;
   // MglDrawer drawer;
   QMathGL *mgl;
   // QWidget *xxw; // more debug
   int xxx = 1; // just for debug
   // TGraph::PlotType type =TGraph::PlotType::PlotPlot; // need include for now
   // int type = 0;
   // int w0 = 200, h0 = 180;
   // double x_min = 0, x_max = 0.01, y_min = 0, y_max = 0; // TODO: z?
   // QList<mglData*> d;
   // mglData *dx = nullptr;
};

#endif


