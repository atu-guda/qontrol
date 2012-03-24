/***************************************************************************
                          qrunview.h  -  description
                             -------------------
    begin                : Fri Aug 17 2001
    copyright            : (C) 2001-2012 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QRUNVIEW_H
#define QRUNVIEW_H

#include <QDialog>

class TModel; class TDataSet; class TMiso; 
class QPaintEvent; class QMouseEvent; class QKeyEvent; 
class QPainter; class QPaintDevice;

/** displays run-time information about process and
  * give feedback 
  * @author atu
  */

class QRunView : public QDialog  {
 Q_OBJECT
 public:
   /** constructor */
   QRunView( TModel *amodel, int atype, QWidget *parent );
   /** destructor */
   ~QRunView();
   /** hint size for good view */
   virtual QSize sizeHint(void) const;
 public slots:
   /**  begins computations */
   void slotStartRun();
   /** interrupts computation */
   void slotStopRun();
   /** do n_steps computations (by timer(0)) */
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
   /** fill vars to model data */
   void fillVars(void);
   /** gets info from TModel and fills local vars */
   void getModelData(void);
   /** gets joystick values */
   void getJoyVal(void);
   /** gets sound values */
   void getSoundVal(void);
   /** gets unknown device values */
   void getAuxVal(void);
   // --------------------- draw some parts of window
   /** draw  all parts of window, if need */
   void drawAll( QPainter &p );
   /** draws cross-like indicators */
   void drawCross( QPainter &p );
   /** draws vertical bar indicators */
   void drawVbar( QPainter &p );
   /** draws vertical bar indicators */
   void drawGbar( QPainter &p );
   /** draws LED-like indicators */
   void drawLED( QPainter &p );
 // ========================================== data ==================  
  protected:
    /** pointer to model ro run */
    TModel *model;
    /** timer to send timer events */
    QTimer *timer;
    /** geometry: w_ - window, g_ - graph, c_ center, s_ - set */
    int w_w, w_h, g_w, g_h, c_x, c_y, s_h; 
    /** koefficients to transform visual and phis coorgs */
    double gkx, gky;
    /** run type */
    int run_type;
    /** state of computation */
    int state;
    /** start time */
    double s_time;
    /** mouse coordinates in graph [-1;1], mouse buttons */
    double mouse_x, mouse_y;
    /** mouse buttons state */
    int mouse_l, mouse_r, mouse_m;
    /** keyboard buttons state */
    int keys_state[10];
    /** input sound value [-1;1] */
    double sound_i_left, sound_i_right;
    /** joystick values */
    double joy_x, joy_y, joy_btn;
    /** unknown device values */
    double auxs[20];
    // some next vars filled from TModel =========================
    /** data array of model */
    double *data;
    /** total number of loops */
    int n_tot;
    /** total counter */
    int i_tot;
    /** number of steps per i/o action */
    int n_steps;
    /** flag for real and model time syncronization */
    int use_sync;
    // -------- input chanals indexes -------
    /** input from mouse (x,y,btn1,btn2,btn3) abs(x,y) <= 1, 0 - center */
    int ic_mouse;
    /** input from joystick (x,y,btn) abs(x,y) <= 1, 0 - center */
    int ic_joy;
    /** input from soundcard (lert,right) */
    int ic_sound;
    /** input from keyboard (left,right,top,bottom,space,enter) */
    int ic_key;
    /** input from unknown device, up to 20 values */
    int ic_aux;
    // -------- output chanals indexes -------
    /** indexes for output */
    int oc[6];
    /** types of output 1-3=cross, 4-7=vbar, 8-11=gbar, 12-15=leds, 16,17=sound */
    int oct[6];
};

#endif

// end of qrunview.h

