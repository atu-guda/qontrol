#pragma once
/***************************************************************************
                          tlinear.h  -  description
                             -------------------
    begin                : Tue Aug 1 2000
    copyright            : (C) 2000-2016 by atu
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

/** Simple element, which summing it's inputs with given coefficients
  *@author atu
  */

class TLinear : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TLinear);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   virtual double f() noexcept override;
   Q_INVOKABLE virtual double f_d( double arg0, double arg1 = 0, double arg2 = 0, double arg3 = 0 ) override;

   /** coefficients on input and shift */
   PRM_PARAMD( a,  0, "&a",  "common factor", "def=1" );
   PRM_PARAMD( b,  0, "&b",  "static shift", "sep=col" );
   PRM_PARAMD( a0, 0, "a&0", "factor in input 0", "def=1\nsep=block" );
   PRM_PARAMD( a1, 0, "a&1", "factor in input 1", "def=-1\nsep=col" );
   PRM_PARAMD( a2, 0, "a&2", "factor in input 2", "sep=col" );
   PRM_PARAMD( a3, 0, "a&3", "factor in input 3", "sep=col" );

   PRM_INPUT( in_0, 0, "&in_0", "First input",  "sep=block" );
   PRM_INPUT( in_1, 0, "i&n_1", "Second input", "sep=col" );
   PRM_INPUT( in_2, 0, "in_2",  "Third input",  "sep=col" );
   PRM_INPUT( in_3, 0, "in_3",  "Fourth input", "sep=col" );

   Q_CLASSINFO( "nameHintBase",  "sum_" );
   DCL_DEFAULT_STATIC;
};

