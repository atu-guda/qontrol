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

#include <vector>
#include <string>

#include <QString>
#include <QStringList>
#include <QWidget>
#include <QColor>
#include <QList>
#include <mgl2/qt.h>

class TGraph;
class TModel;
class ScaleData;
class QMathGL;
class mglData;
class mglGraph;
class mglDraw;
class QVBoxLayout;
class QLabel;

//* converts color and width to style
QString color2style( int color, int lw = 1, const QString &extra = QString() );

//* inner helper class to draw MathGL plot

class MglDrawer : public mglDraw
{
  public:
   MglDrawer( TGraph *agra );
   virtual ~MglDrawer() override;
   virtual int Draw( mglGraph *gr ) override;
   virtual void Reload() override;
   void resetData();
   QSize getSize0() const;
  protected:
   TGraph *gra;
   TModel *model = nullptr;
   // TGraph::PlotType type = TGraph::PlotType::PlotPlot; // need include for now
   int type = 0;
   double x_min = 0, x_max = 0.01, y_min = 0, y_max = 0; // TODO: z?
   QList<mglData*> d;
   QString label_x, label_y;
   std::vector<std::string> labels; // not QStringList: pass to c-alike funcs
   std::vector<std::string> extras;
   mglData *dx = nullptr;
   ScaleData *scd = nullptr, *scd_del = nullptr;
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
   QVBoxLayout *lay;
   MglDrawer *drawer;
   QMathGL *mgl;
   QLabel *lbl;
};

#endif


