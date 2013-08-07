/***************************************************************************
                          qgraphview.h  -  description
                             -------------------
    begin                : Sat Aug 19 2000
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

#ifndef QGRAPHVIEW_H
#define QGRAPHVIEW_H

#include <QWidget>
class QPaintEvent;
class QMouseEvent;

#include "tmodel.h"
#include "qmo2doc.h"
#include "qmo2view.h"
/**allow to see and edit graph descriptions (TGraph)
  *@author atu
  */

class QGraphView : public QWidget  {
 Q_OBJECT
 public:
   QGraphView( QMo2Doc *adoc, QMo2View *mview, QWidget *parent );
   ~QGraphView();

 protected:
   /** draw itself */
   virtual void paintEvent( QPaintEvent *pe );
   /** reaction to mouse */
   virtual void mousePressEvent( QMouseEvent *me );
 protected:
   QMo2Doc *doc;
   QMo2View *mainview;
   TModel *model;
   int grid_sz;
};

#endif

// end of qgraphview.cpp

