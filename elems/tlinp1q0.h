#pragma once
/***************************************************************************
                          tlinp1q0.h  -  description
                             -------------------
    begin                : Fri Sep 1 2000
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

/** simple linear diff element (aperiodic  1-st order)
  * with ability to handle nonlinear part
  * dx/dt= a * f(k_u*u - x)
  * or w/o nonliear part
  * 1/a * dx/dt + x = k_u * u
  * a = 1 / tau
  * linear element:
  * W = k_u / ( tau * p ) + 1
  * @author atu
  */

class TLinP1Q0 : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TLinP1Q0);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   virtual double f() noexcept override;
   virtual int miso_startLoop( long acnx, long acny ) override;

   PRM_PARAMD( a,         0, "&a", "a in dx/dt = a*f(ku*u(t)-x)", "def=1" );
   PRM_PARAMD( ku,        0, "&k_u",  "Amplification: coefficient near u(t) ", "def=1" );
   PRM_SWITCH( use_u1,    0, "use in_f as &f()", "Use in_f as source of f()", "sep=col" );
   PRM_SWITCH( use_u2,    0, "u^&2", "Calculate u^2 before usage", "sep=col" );
   PRM_SWITCH( use_sqrt0, 0, "sqrt0(x)", "Output sqrt0(x)", "" );

   PRM_INPUT( in_u,        0, "&in_u",     "Signal input",  "sep=block" );
   PRM_INPUT( in_f,        0,  "in_f", "Functional input",  "sep=col" );
   PRM_DOUBLE( tau,  efInner, "\\tau",   "Effective time",  "def=1"  );

   PRM_DOUBLE(  x2,  efInner, "x^2",     "Squared output", "" );

   /** old value of x  */
   double x_old = 0;

   Q_CLASSINFO( "nameHintBase",  "lp1_" );
   DCL_DEFAULT_STATIC;
};




