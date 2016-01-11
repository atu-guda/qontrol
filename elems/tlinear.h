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

#ifndef TLINEAR_H
#define TLINEAR_H

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
   /** main computation function */
   virtual double f() noexcept override;
   /** coefficients on input and shift */
   PRM_PARAMD( a0, 0, "a0", "factor in input 0", "def=1" );
   PRM_PARAMD( a1, 0, "a1", "factor in input 1", "def=-1" );
   PRM_PARAMD( a2, 0, "a2", "factor in input 2", "sep=col" );
   PRM_PARAMD( a3, 0, "a3", "factor in input 3", "" );
   PRM_PARAMD( b,  0, "b", "static shift", "sep=col" );
   PRM_PARAMD( a,  0, "a", "common factor", "sep=col\ndef=1" );

   PRM_INPUT( in_0, 0, "in_0", "First input",  "sep=block" );
   PRM_INPUT( in_1, 0, "in_1", "Second input", "sep=col" );
   PRM_INPUT( in_2, 0, "in_2", "Third input",  "sep=col" );
   PRM_INPUT( in_3, 0, "in_3", "Fourth input", "sep=col" );

   Q_CLASSINFO( "nameHintBase",  "sum_" );
   DCL_DEFAULT_STATIC;
};

#endif
