#pragma once
/***************************************************************************
                          tsumn.h  - N-input linear summator
                             -------------------
    begin                : 2015.10.31
    copyright            : (C) 2015-2019 by atu
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


#include "tmiso.h"

/** N-input summator, like TLinear, with common factor
  *@author atu
  */

class TSumN : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TSumN);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   virtual double f() noexcept override;

   PRM_PARAMD( a,  0, "a", "common factor", "def=1\nsep=colend" );
   PRM_PARAMD( b,  0, "b",  "static shift", "sep=blockend" );

   PRM_DOUBLE( x2, efInner, "x^2", "Squared output", "" );

   Q_CLASSINFO( "nameHintBase",  "sumn_" );
   DCL_DEFAULT_STATIC;
};

