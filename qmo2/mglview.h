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
class ScaleData;
class mglData;
class mglGraph;
class mglDraw;

//* converts color and width to style
QString color2style( int color, int lw = 1, const QString &extra = QString() );

// modified copy of GraphElem
struct DataLineInfo {
  int type;
  int is2D;
  int ig;
  std::string label;
  std::string extra;
  std::string opt;
  double v_min, v_max;
  mglData *md;
  const std::vector<double> *ve;
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
   virtual void paintEvent( QPaintEvent *pe ) override;
   virtual void mousePressEvent( QMouseEvent *me ) override;
   virtual void keyPressEvent( QKeyEvent *ke ) override;
   virtual void resizeEvent( QResizeEvent *e ) override;
 protected:
   int Draw( mglGraph *gr );
   void Reload();
   void resetData();
   QSize getSize0() const;
 protected:
   TGraph *gra;
   mglGraph gr = mglGraph( 0,100, 100 );
   std::vector<DataLineInfo> dl;
   double x_min = 0, x_max = 0.01, y_min = 0, y_max = 0, z_min = 0, z_max = 0;
   std::string label_x, label_y, label_z;
   mglData *d_x, *d_y, *d_z; // axiz data
   mglData *d_c0, *d_c1, *d_c2, *d_c3, *d_c4, *d_c5; // aux data - not owning
   ScaleData *scd, *scd_o; // _o = ptr to original
   std::vector<uint8_t> pb; // pix buf
   int alc_x = 0, alc_y = 0; // size of allocated buffer**4
   mglPoint mark_point { 0, 0, 0 };
   mglPoint base_point { 0, 0, 0 };
};

#endif


