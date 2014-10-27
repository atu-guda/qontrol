/***************************************************************************
                          graphview.h  -  description
                             -------------------
    begin                : Sat Aug 19 2000
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

#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QWidget>
class QPaintEvent;
class QMouseEvent;

#include "tmodel.h"
#include "labodoc.h"
#include "laboview.h"
/**allow to see and edit graph descriptions (TGraph)
  *@author atu
  */

class GraphView : public QWidget  {
 Q_OBJECT
 public:
   GraphView( LaboDoc *adoc, LaboView *mview, QWidget *parent );
   ~GraphView();

 protected:
   /** draw itself */
   virtual void paintEvent( QPaintEvent *pe );
   /** reaction to mouse */
   virtual void mousePressEvent( QMouseEvent *me );
 protected:
   LaboDoc *doc;
   LaboView *mainview;
   TModel *model;
   int ex_sz;
   int grid_sz;
   int fwidth;
};

#endif

// end of graphview.cpp

