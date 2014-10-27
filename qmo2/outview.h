/***************************************************************************
                          outview.h  -  description
                             -------------------
    begin                : Fri Aug 18 2000
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

#ifndef OUTVIEW_H
#define OUTVIEW_H

#include <QWidget>

class QPaintEvent;
class QMouseEvent;

#include "tmodel.h"
#include "labodoc.h"
#include "laboview.h"

/**allow to see TOutArr as buttons and edit properties
  *@author atu
  */
class TModel;

class OutView : public QWidget  {
 Q_OBJECT
 public:
   /** constructor */
   OutView(  LaboDoc *adoc, LaboView *mview,  QWidget *parent );
   /** destructor */
   ~OutView();

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

// end of outview.h

