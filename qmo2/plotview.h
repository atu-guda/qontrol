/***************************************************************************
                          plotview.h  -  description
                             -------------------
    begin                : Sun Aug 20 2000
    copyright            : (C) 2000-2014 by atu
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

#ifndef PLOTVIEW_H
#define PLOTVIEW_H

#include <QString>
#include <QWidget>
#include <QColor>
#include <scaledata.h>

class QResizeEvent;
class LaboDoc; class TRootData; class TModel;
class TDataSet; class TMiso; class TGraph;
class QPaintEvent; class QMouseEvent; class QKeyEvent;
class QPainter; class QPaintDevice;

/**can plot graphs for TModel on info by TGraph
  *@author atu
  */

class PlotView : public QWidget  {
 Q_OBJECT
 public:
   /** constructor */
   PlotView( LaboDoc *adoc, TGraph *agra, QWidget *parent );
   /** destructor */
   ~PlotView();
   /** hint size for good view */
   virtual QSize sizeHint(void) const;
 public slots:
   /** set geometry, axes and scale parms */
   void setScale(void);
   /** set colors for plots*/
   void setColors(void);
   /** set colors for printing */
   void setPrintColors(void);
   /** prints plot */
   void printPlot(void);
   /** export plot to graphics file (png) */
   void exportPlot();
   /** moves tool to given location */
   void moveTool(void);
   /** shown help */
   void showHelp(void);
   /** shows some infos about toll and ref coords ... */
   void dataInfo(void);

  protected:
   /** draw itself */
   virtual void paintEvent( QPaintEvent *pe );
   /** reaction to mouse */
   virtual void mousePressEvent( QMouseEvent *me );
   /** reaction to keyboard */
   virtual void keyPressEvent( QKeyEvent *ke );
   /** reimplemented from QWidget to rescale if resize */
   virtual void resizeEvent( QResizeEvent *e );
   /** set arrays and counters, minmax */
   virtual void initArrs(void);
   /** set fake arrays and counters, minmax if no real data*/
   virtual void initFakeArrs(void);
   /** calculate scale params */
   virtual void calcScale(void);
   /** fills colors and labels at startup */
   virtual void setStartColors(void);
   /** converts physical coords to visual with cast to integer */
   void phys2vis( double x, double y, int *ix, int *iy );
   /** converts physical coords to visual w/o cast */
   void phys2vis( double x, double y, double *idx, double *idy );
   /** converts visual coords to physical */
   void vis2phys( int ix, int iy, double *x, double *y );
   /** paint all in given painter */
   void drawAll( QPainter &p );
   /** draw grid and ticks */
   void drawGrid( QPainter &p );
   /** draw axes, labels, cross, ref point, bottom legend */
   void drawAxes( QPainter &p );
   /** draw plots */
   void drawPlots( QPainter &p );
   /** fills plp[*] */
   void calcPlp(void);
  protected:
    /** pointer to document to fill root, model.. */
    LaboDoc *doc;
    /** pointer to root */
    TRootData *root;
    /** pointer to model*/
    TModel *model;
    /** ptr to graph descritions */
    TGraph *gra;
    /** ptr scale descrition */
    ScaleData *scd;
    /** colors elements (dia) */
    QColor bgColor, scaleColor, gridColor, labelColor, plotColor[6];
    /** data maxmins and setup values (+-dia)*/
    double plotMinLX, plotMinLY;
    /** labels text */
    QString xLabel, yLabel[6];
    /** pointers to real data: set by initArrs() */
    const double *datax, *datay[6];
    /** arrays of flags -- is need to draw this point */
    char *plpbuf, *plp[6];
    /** number of graphs, number of points, number of y-coords */
    int ng, nn, ny;
    /** time value to recalc if model rerun, type of paint device */
    int modelSign, devTp;
    /** on/off plot states -- may be line thickness ? */
    int plotOn[6];
    /** margins and size in pixels */
    int pix_l, pix_t, pix_r, pix_b;
    int pix_x, pix_y, pix_x0, pix_y0, leg_sz, pix_w, pix_h;
    /** scale coeffs */
    double kx, ky;
    /** selected graph <0 - none; selected point index, quality plotting */
    int sel_g, sel_idx, qual, linew, need_rescale;
    /** minimal and maximum number of selected graph */
    int min_sel_g, max_sel_g;
    /** measure tool and reference points coords */
    double tool_x, tool_y, ref_x, ref_y;
    /** error string */
    const char *errstr;
    /** help string */
    static const char helpstr[];
};

#endif

// end of plotview.h

