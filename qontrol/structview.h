/***************************************************************************
                          structview.h  -  description
                             -------------------
    begin                : Sat Aug 12 2000
    copyright            : (C) 2000-2015 by atu
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

#ifndef STRUCTVIEW_H
#define STRUCTVIEW_H

#include <QWidget>

class QPaintEvent;
class QMouseEvent;
class QKeyEvent;
class QMenu;

#include "scheme.h"
#include "laboview.h"

class QPainter;


/**display model (scheme) structure ...
  *@author atu
  */

class StructView : public QWidget  {
  Q_OBJECT
  public:
    //* information about element to draw
    struct ElemInfo {
      // from element
      QString name, type;
      int vis_x, vis_y, ord, locked, onlyFirst, onlyLast, flip, noIcon;
      // number of signal and parametric inputs
      int n_inp, n_pinp;
      // calculated
      int xs0, ys0, xs, ys, xc, yc, flip_factor;
      int li_dst_x, pre_dst_x, li_src_x, li_pdst_y;
      const InputParams *pis;
    };
    StructView( Scheme *a_sch, LaboView *mview );
    virtual ~StructView();
    /** print contents */
    void printAll();
    /** grid size */
    int getGridSize() const { return grid_sz; }
    /** bound of all elements in model */
    QSize getElemsBound() const;
    virtual QSize sizeHint () const;
    /** gets coordinates on selected element */
    QPoint getSelCoords() const;
  public slots:
    void update();
  signals:
    void sig_changeSel(int,int,int);
    void sig_changeLevel(int);
  protected:
    /** fill and calc info about element */
    bool fill_elmInfo( const TMiso *ob, ElemInfo &el ) const;
    /** draw itself */
    virtual void paintEvent( QPaintEvent *pe );
    /** reaction to mouse */
    virtual void mousePressEvent( QMouseEvent *me );
    /** reaction on mouse doubleclick */
    virtual void mouseDoubleClickEvent( QMouseEvent *me );
    /** reaction to keyboard */
    virtual void keyPressEvent( QKeyEvent *ke );
    /** paint all in given painter */
    void drawAll( QPainter &p );
    /** create popup menu */
    QMenu* createPopupMenu( const QString &title, bool has_elem );

  protected:
    /** pointer to TModel to be drawed */
    Scheme *sch;
    /** main view (parent) */
    LaboView *mainview;
    /** type of output device */
    int devTp = 0;
    /** grid size, left, top margin size, object size,  elements margin */
    int grid_sz = 46, lm = 4, tm = 4, obj_sz = 32, el_marg = 1, em_small = 10, ex_small = 10;
};

#endif

// end of structview.h

