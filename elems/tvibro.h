/***************************************************************************
                          tvibro.h  -  description
                             -------------------
    begin                : Mon Aug 28 2000
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

#ifndef TVIBRO_H
#define TVIBRO_H

#include <tmiso.h>

/**can simulate vibrational element
  *@author atu
  */

class TVibro : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TVibro);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   virtual double f() noexcept override;
   virtual int do_startLoop( int acnx, int acny ) override;
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt ) override;

   PRM_PARAMD( c0, 0, "c_0", "Damping coefficient", "def=0.4" );
   PRM_PARAMD( Omega, 0, "\\Omega", "Natural frequency if c_0=0 and f(x)=x", "def=1.2" );
   PRM_PARAMD( v0, efNoRunChange, "v_0", "v(0)", "sep=col" );
   PRM_SWITCH( use_u1, efNoRunChange, "use Force",
        "Use in_force as return force element output", "" );

   /** Current velocity dx/dt */
   PRM_DOUBLE( v, efInner, "v", "v(t)", "" );

   PRM_INPUT( in_u, 0, "u(t)", "Main input",  "sep=block" );
   PRM_INPUT( in_f, 0, "f(x)", "Force input (if enabled)", "sep=col" );

   /** flag for start(1,2,0)  */
   int isStart = 1;
   /** old values and misc */
   double u_old = 0, f_old = 0, x_old = 0, x_old2 = 0, tdt2 = 1;

   Q_CLASSINFO( "nameHintBase",  "vi_" );
   DCL_DEFAULT_STATIC;
};

#endif


