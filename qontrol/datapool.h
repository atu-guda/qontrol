#pragma once
/***************************************************************************
    datapool.h - container for misc data header
                             -------------------
    begin                : 2016.10.30
    copyright            : (C) 2016-2016 by atu
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


#include "linkedobj.h"


/** Genetic data pool - to add to model, simulations ...
  *@author atu
  */

class DataPool : public LinkedObj  {
  Q_OBJECT
 public:
  DCL_CTOR(DataPool);
  // virtual ~DataPool() override;
  DCL_CREATE;
  DCL_STD_INF;

 protected:

  Q_CLASSINFO( "nameHintBase",  "d_" );
  DCL_DEFAULT_STATIC;

};

// end of datapool.h

