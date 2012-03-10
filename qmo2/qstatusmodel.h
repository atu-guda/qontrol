/***************************************************************************
                          qstatusmodel.h  -  description
                             -------------------

    begin                : Sat Aug 17 2002 
    copyright            : (C) 2002 by atu
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

#ifndef QSTATUSMODEL_H
#define QSTATUSMODEL_H

#include <qwidget.h>
#include <qstatusbar.h>
//Added by qt3to4:
#include <QLabel>
#include "tmodel.h"
#include "qmo2view.h"

class QLabel;

/** displays status of model in bottom part of model window
  *@author atu
  */

class QStatusModel : public QStatusBar  {
 Q_OBJECT
 public:
   QStatusModel( QMo2View* mview, QWidget *parent, const char *name );
   ~QStatusModel();
   void update();

 protected:
   QMo2View *mainview;
   QLabel *l_mod, *l_level, *l_stat, *l_nums, *l_name,  *l_desc, *l_val;
};

#endif

// end of qstatusmodel.h

