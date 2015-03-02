/***************************************************************************
                          tlinear.h  -  description
                             -------------------
    begin                : Tue Aug 1 2000
    copyright            : (C) 2000-2015 by atu
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
   virtual double f( double t ) override;
   /** coefficients on input and shift */
   PRM_DOUBLE( a0, 0, "a0", "factor in input 0", "def=1" );
   PRM_DOUBLE( a1, 0, "a1", "factor in input 1", "def=-1" );
   PRM_DOUBLE( a2, 0, "a2", "factor in input 2", "sep=col" );
   PRM_DOUBLE( a3, 0, "a3", "factor in input 3", "" );
   PRM_DOUBLE( b,  0, "b", "static shift", "sep=col" );
   PRM_DOUBLE( a,  0, "a", "common factor", "sep=col\ndef=1" );

   PRM_INPUT( in_0, 0, "input 0", "First input",  "sep=block" );
   PRM_INPUT( in_1, 0, "input 1", "Second input", "sep=col" );
   PRM_INPUT( in_2, 0, "input 2", "Third input",  "sep=col" );
   PRM_INPUT( in_3, 0, "input 3", "Fourth input", "sep=col" );

   DCL_DEFAULT_STATIC;
};

#endif
