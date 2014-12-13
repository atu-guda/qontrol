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
#include <QFont>
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
//* converts MathGL point to QString
QString toQString( const mglPoint &p );
//* distance beween points
double mglLen( const mglPoint &a, const mglPoint &b );

// modified copy of GraphElem
struct DataLineInfo {
  int type;
  int is2D;
  int ig;
  int on;
  std::string label;
  std::string extra;
  std::string opt;
  double v_min, v_max;
  mglData *md;
  const std::vector<double> *ve; // until md is filled
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
   QString getInfo( bool more = false ) const;

 public slots:
   void resetScale();
   void markToBase();
   void setPhi( double a_phi, bool add = false );
   void setTheta( double a_theta, bool add = false );
   void setXmag( double amag, bool mul = false );
   void setYmag( double amag, bool mul = false );
   void setZmag( double amag, bool mul = false );
   void setXbase( double base, bool rel = false );
   void setYbase( double base, bool rel = false );
   void setZbase( double base, bool rel = false );
   void setAlpha( double al, bool rel = false );
   void zoom(); // from base to mark
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
   int Draw( mglGraph *gr );
   void Reload();
   void resetData();
   QSize getSize0() const;
   void drawFooter( QPainter &p, int hg );
   void setMarkToLink();
   int findNearest( const mglPoint &p, int dl_idx );//* find nearest point index
 protected:
   TGraph *gra;
   mglGraph gr = mglGraph( 0,100, 100 );
   std::vector<DataLineInfo> dl;
   std::vector<int> dli; // convert sel -> idx in dl[], or -1
   std::string label_x, label_y, label_z;
   mglData *d_zero; // special data: zeros
   mglData *d_x, *d_y, *d_z; // axiz data
   int nn = 0, ny = 1, nx = 0; //* copy from arrays
   mglData *d_c0, *d_c1, *d_c2, *d_c3, *d_c4, *d_c5; // aux data - not owning
   ScaleData *scd, *scd_o; // _o = ptr to original
   const QFont &pa_fnt;
   int em = 10, ex = 10, bottom_h = 40; // default only
   std::vector<uint8_t> pb; // pix buf
   int alc_x = 0, alc_y = 0; // size of allocated buffer**4
   mglPoint mark_point { 0, 0, 0 };
   mglPoint base_point { 0, 0, 0 };
   mglPoint pr_min { 0, 0, 0 }, pr_max { 1, 1, 1 }, pr_dlt { 1, 1, 1 }; // real
   mglPoint pv_min { 0, 0, 0 }, pv_max { 1, 1, 1 }, pv_dlt { 1, 1, 1 }; // visual
   mglPoint mag { 1, 1, 1 };
   double angle_step = 5.0, mag_step = 0.707106781, scale_step = 0.10;
   int sel = 0; //* selected plot, may be none: see dli[sel]
   int linkPlot = -1; //* linked data index (unlike sel, dl[linkTo])
   int linkIdx = 0;   //* current point index in linked array
   bool data_loaded = false;
};

#endif


