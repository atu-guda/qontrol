#pragma once
/***************************************************************************
                          statusmodel.h  -  description
                             -------------------

    begin                : Sat Aug 17 2002
    copyright            : (C) 2002-2022 by atu
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


#include <QStatusBar>

#include "commonsubwin.h"

class QLabel;

/** displays status of model in bottom part of model window
  *@author atu
  */

class StatusModel : public QStatusBar  {
 Q_OBJECT
 public:
   explicit StatusModel( CommonSubwin *a_up_view );
   void update();

 protected:
   CommonSubwin *up_view;
   QLabel *l_mod, *l_level, *l_stat, *l_name,  *l_desc, *l_val;
};


// end of statusmodel.h

