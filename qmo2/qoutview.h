/***************************************************************************
                          qoutview.h  -  description
                             -------------------
    begin                : Fri Aug 18 2000
    copyright            : (C) 2000 by atu
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

#ifndef QOUTVIEW_H
#define QOUTVIEW_H

#include <qwidget.h>
#include "tmodel.h"
#include "qmo2doc.h"
#include "qmo2view.h"
/**allow to see TOutArr as buttons and edit properties
  *@author atu
  */
class TModel;

class QOutView : public QWidget  {
 Q_OBJECT
 public:
   /** constructor */
   QOutView(  QMo2Doc *adoc, QMo2View *mview,  QWidget *parent, const char *name=0 );
   /** destructor */
   ~QOutView();

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

// end of qoutview.h

