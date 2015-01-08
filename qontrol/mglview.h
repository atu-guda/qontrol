/***************************************************************************
                          mglview.h  -  window to show plots via MathGL
                             -------------------
    begin                : 2014.11.29
    copyright            : (C) 2014-2015 by atu
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
#include <memory>

#include <QString>
#include <QFont>
#include <QWidget>
#include <QColor>
#include <QList>

#include <tgraph.h>

class TGraph;
class ScaleData;
class mglData;
class mglGraph;
class mglDraw;
/**can plot graphs for TModel on info by TGraph
  *@author atu
  */

class MglView : public QWidget  {
 Q_OBJECT
 public:
   MglView( TGraph *agra, QWidget *parent );
   ~MglView();
   virtual QSize sizeHint() const override;
   QString getInfo( bool more = false ) const;

 public slots:
   void resetScale();
   void markToBase();
   void setPhi( double a_phi, bool add = false );
   void setTheta( double a_theta, bool add = false );
   void setXmag( double amag, bool mul = false );
   void setYmag( double amag, bool mul = false );
   void setZmag( double amag, bool mul = false );
   void setAlpha( double al, bool rel = false );
   void zoom(); // from base to mark
   void zoomReset();
   void printPlot();
   void exportPlot();
   void showInfo();
   void showHelp();
   void setMark();
   void setScale();
   void saveScale();
   void togglePlot();
   void toggleAllPlots();
   void linkToPlot();
   void unlinkFromPlot();
   void nextPointInPlot( int step = 1 );

 protected:
   virtual void paintEvent( QPaintEvent *pe ) override;
   virtual void mousePressEvent( QMouseEvent *me ) override;
   virtual void keyPressEvent( QKeyEvent *ke ) override;
   virtual void resizeEvent( QResizeEvent *e ) override;
 protected:
   void drawAll( QPainter &p );
   void Reload();
   void resetData();
   QSize getSize0() const;
   void drawFooter( QPainter &p, int hg );
   void setMarkToLink();
 protected:
   TGraph *gra;
   ViewData vd;
   mglPoint pr_min {0,0,0}, pr_max {1,1,1}, pr_dlt {1,1,1};
   mglGraph gr = mglGraph( 0,100, 100 );
   std::shared_ptr<ScaleData> scd;
   ScaleData *scd_o; // _o = ptr to original
   const QFont &pa_fnt;
   int em = 10, ex = 10, bottom_h = 40; // default only
   std::vector<uint8_t> pb; // pix buf
   int alc_x = 0, alc_y = 0; // size of allocated buffer**4
   double angle_step = 5.0, mag_step = 0.707106781, scale_step = 0.10;
   int sel = 0; //* selected plot, may be no plot here
   int linkPlot = -1; //* linked data index
   int linkIdx = 0;   //* current point index in linked array
   bool data_loaded = false;
};

#endif


