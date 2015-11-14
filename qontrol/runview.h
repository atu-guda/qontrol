/***************************************************************************
                          runview.h  -  description
                             -------------------
    begin                : Fri Aug 17 2001
    copyright            : (C) 2001-2015 by atu
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

#ifndef RUNVIEW_H
#define RUNVIEW_H

#include "defs.h"
#include <QDialog>
#include <QSemaphore>

class TModel; class Scheme; class ModelRunner;
// class QPaintEvent; class QMouseEvent; class QKeyEvent;
// class QPainter; class QPaintDevice;

/** displays run-time information about process and
  * give feedback
  * @author atu
  */

class RunView : public QDialog  {
 Q_OBJECT
 public:
   RunView( TModel *a_mod, QWidget *parent );
   ~RunView();
   /** hint size for good view */
   virtual QSize sizeHint() const;
 public slots:
   /**  begins computations */
   void slotStartRun();
   /** interrupts computation */
   void slotStopRun();
   /** do some computations (by timer(0)) */
   void slotRunNext();

  protected:
   // -------------------- events -----------------
   /** reimplemented from QWidget to calc window size data */
   virtual void resizeEvent( QResizeEvent *e );
   /** draw itself */
   virtual void paintEvent( QPaintEvent *pe );
   /** reaction to mouse press */
   virtual void mousePressEvent( QMouseEvent *me );
   /** reaction to mouse release */
   virtual void mouseReleaseEvent( QMouseEvent *me );
   /** reaction to mouse move */
   virtual void mouseMoveEvent( QMouseEvent *me );
   /** reaction to keyboard press */
   virtual void keyPressEvent( QKeyEvent *ke );
   /** reaction to keyboard release */
   virtual void keyReleaseEvent( QKeyEvent *ke );
   // ------------------------- inner -----------------------------
   /** converts physical coords to visual with cast to integer */
   void phys2vis( double x, double y, int *ix, int *iy );
   /** converts physical coords to visual w/o cast */
   void phys2vis( double x, double y, double *idx, double *idy );
   /** converts visual coords to physical */
   void vis2phys( int ix, int iy, double *x, double *y );
   /** fill vars to scheme data */
   void fillVars();
   /** gets info from Scheme and fills local vars */
   void getSchemeData();
   // --------------------- draw some parts of window
   /** draw  all parts of window, if need */
   void drawAll( QPainter &p );
 // ========================================== data ==================
  protected:
    /** pointer to model ro run */
    TModel *model;
    //* pointer to active scheme: just for fast access
    Scheme *sch = nullptr;
    /** timer to send timer events */
    QTimer *timer;
    //* sync with 
    QSemaphore sem_io;
    //* model runner in thread
    ModelRunner *runner = nullptr;
    /** geometry: w_ - window, g_ - graph, c_ center, s_ - set */
    int w_w, w_h, g_w, g_h, c_x, c_y, s_h;
    /** coefficients to transform visual and phis coords */
    double gkx, gky;
    /** state of computation */
    int state;
    /** start time */
    double s_time = 0;
    /** mouse coordinates in graph [-1;1], mouse buttons */
    double mouse_x = 0, mouse_y = 0;
    /** mouse buttons state */
    int mouse_l = 0, mouse_r = 0, mouse_m = 0;
    /** keyboard buttons state */
    int keys_state[10];
    // some next vars filled from Scheme =========================
    /** total number of loops */
    int n_tot = 1;
    //* iteration per loop
    int N = 1;
    /** misc counters */
    int i_tot = 0, il1 = 0, il2 = 0;
    //* current time
    double t;
    //* total time
    double T = 1.0;
    /** flag for real and scheme time synchronization */
    int syncRT = 0;
    //* one step per I/O
    double io_t = 1.0;
    /** flag to start w/o keyboard hit */
    int autoStart = 0;
};

#endif

// end of runview.h

