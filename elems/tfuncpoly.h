/***************************************************************************
                          tfuncpoly.h  -  description
                             -------------------
    begin                : Sun Aug 27 2000
    copyright            : (C) 2000-2013 by atu
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

#ifndef TFUNCPOLY_H
#define TFUNCPOLY_H

#include <tmiso.h>

static const char* const tfuncpoly_list =
     "a*y+g\n"                                 // 0
     "a*y^2+b*y+g\n"                           // 1
     "a*y^3+b*y^2+c*y+g\n"                     // 2
     "a*u0^2+b*u0*u1+c*u1^2+g\n"               // 3
     "a*sqrt(y)+g\n"                           // 4
     "hypot(a*u0,b*u1)+g\n"                    // 5
     "a*u0^2+..+d*u3^2+g\n"                    // 6
     "Vibro Ampl(omega); A=c0 B=Ome\n"         // 7
     "a*(u1-u0^2)^2+b*(1-u0)^2\n"              // 8
     "1-exp(-a*((u0^2+u1^2-1)^2-b*u0-c*u1))\n" // 9
     "a*(1+b*y)+g\n"                           // 10
     "a*(b*y+c*abs(y))+g"                      // 11
;


/**polinomial functions
  *@author atu
  */

class TFuncPoly : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TFuncPoly);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f( double t ) override;

   /** type of function */
   PRM_LIST( type, 0, "Type", "Function type", "", tfuncpoly_list );
   /** coefficients and shift */
   PRM_DOUBLE( a,  0, "a", "Parameter a", "sep=col\ndef=1" );
   PRM_DOUBLE( b,  0, "b", "Parameter b", "def=1" );
   PRM_DOUBLE( c,  0, "c", "Parameter c", "" );
   PRM_DOUBLE( d,  0, "d", "Parameter d", "sep=col" );
   PRM_DOUBLE( e,  0, "e", "Parameter e", "" );
   PRM_DOUBLE( g,  0, "g", "Parameter g", "" );
   PRM_DOUBLE( x0, 0, "x0", "Input shift: y = in_0 - in_1 - x0;", "" );

   PRM_INPUT( in_0, 0, "input 0", "First input",  "sep=block" );
   PRM_INPUT( in_1, 0, "input 1", "Second input", "sep=col" );
   PRM_INPUT( in_2, 0, "input 2", "Third input",  "sep=col" );
   PRM_INPUT( in_3, 0, "input 3", "Fourth input", "sep=col" );

   DCL_DEFAULT_STATIC;
};

#endif


