/***************************************************************************
                          qstructview.h  -  description
                             -------------------
    begin                : Sat Aug 12 2000
    copyright            : (C) 2000-2013 by atu
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

#ifndef QSTRUCTVIEW_H
#define QSTRUCTVIEW_H

#include <QWidget>

class QPaintEvent;
class QMouseEvent;
class QKeyEvent;

#include "tmodel.h"
#include "qmo2doc.h"
#include "qmo2view.h"

class QPainter;

/**display TModel structure ...
  *@author atu
  */

class QStructView : public QWidget  {
   Q_OBJECT
 public:
   /** constructor */
   QStructView( QMo2Doc *adoc, QMo2View *mview,  QWidget *parent );
   /** destructor */
   virtual ~QStructView();
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

 protected:
   /** pointer to  document */
   QMo2Doc *doc;
   /** main view (parent) */
   QMo2View *mainview;
   /** pointer to TModel to be drawed */
   TModel *model;
   /** type of output device */
   int devTp;
   /** grid size, left, top margin size, object size */
   int grid_sz, lm, tm, obj_sz;
};

#endif

// end of qstructview.h

