/***************************************************************************
                          thyst.h  -  description
                             -------------------
    begin                : Sat Aug 26 2000
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

#ifndef THYST_H
#define THYST_H

#include <tmiso.h>

static const char* const thyst_list = 
     "s+alpha*d\n"     // 0
     "alpha*d\n"       // 1
     "Thetta(d)\n"     // 2 
     "sign(d)\n"       // 3 
     "tanh(alpha*d)\n" // 4
     "tanh(s+alpha*d)"; // 5

/**basic hysteresis simulation
  *@author atu
  */
class THyst : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(THyst);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   /** reimplemented from TMiso to set init state */
   virtual int do_startLoop( int acnx, int acny ) override;
   void fixState(void);
 protected:
   /** type of hysteresis */
   PRM_LIST( type, efNoRunChange, "Type",  "Type of hysteresis", "", thyst_list );
   /** width of hysteresis  */
   PRM_DOUBLE( x0, 0, "x_0",  "x0 - width if hysteresis", "min=0\nsep=col\ndef=1" );
   /** slope of hysteresis  */
   PRM_DOUBLE( alpha, 0, "\\alpha", "Hysteresis slope", "def=0.2"  );
   /** output scale */
   PRM_DOUBLE( a, 0, "a scale", "output scale", "sep=col\ndef=1" );
   /** output shift */
   PRM_DOUBLE( b, 0, "b shift", "output shift", ""  );
   /** state d */
   PRM_DOUBLE( d, efInner, "d", "inner state 'd'", "" );
   /** state s */
   PRM_DOUBLE( s, efInner, "s", "inner state 's'", "" );
   
   PRM_INPUT( in_u, 0, "input", "Single input",  "sep=block" );
   
   DCL_DEFAULT_STATIC;
};

#endif
