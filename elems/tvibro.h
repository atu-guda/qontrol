#pragma once
/***************************************************************************
                          tvibro.h  -  description
                             -------------------
    begin                : Mon Aug 28 2000
    copyright            : (C) 2000-2019 by atu
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
   virtual int miso_startLoop( long acnx, long acny ) override;
   virtual int do_preRun() override;

   PRM_PARAMD( c0,         0, "&c_0",    "Damping coefficient", "def=0.4" );
   PRM_PARAMD( Omega,      0, "\\Omega", "Natural frequency if c_0=0 and f(x)=x", "def=1.2" );
   PRM_PARAMD( v0,     efNRC, "&v_0",    "v(0)", "" );
   PRM_PARAMD( beta,       0, "\\beta",  "Coefficient near x^3", "def=0" );

   PRM_SWITCH( use_u1, efNRC, "use f_x", "Use in_f as return force element output", "sep=col" );
   PRM_SWITCH( mul_fa, efNRC, "v * f_a", "Multiply f_a to v", "" );
   PRM_SWITCH( no_ome2,efNRC, "no Omega^2", "Coefficient near x is Omega, not Omega^2", "" );

   PRM_DOUBLE(  v,   efInner, "v",   "v(t)", "" );
   PRM_DOUBLE( x2,   efInner, "x^2", "Squared output", "" );
   PRM_DOUBLE(  a,   efInner, "a",    "a", "" );

   PRM_INPUT( in_u,        0, "&u(t)",  "Main input",  "sep=block" );
   PRM_INPUT( in_f,        0, "&f()",   "Return force input (if enabled), multiplied to \\Omega", "sep=col" );
   PRM_INPUT( in_fa,       0, "&f_a()", "Additional force (left equation part)", "" );

   /** old values and misc */
   double x_old = 0, x_old2 = 0, tdt2 = 1;

   Q_CLASSINFO( "nameHintBase",  "vi_" );
   DCL_DEFAULT_STATIC;
};



