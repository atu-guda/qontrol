/***************************************************************************
                          tlinp1q0.h  -  description
                             -------------------
    begin                : Fri Sep 1 2000
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

#ifndef TLINP1Q0_H
#define TLINP1Q0_H

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
   /** main computation function */
   virtual double f( double t ) noexcept override;
   /** reimplemented from TMiso to init state */
   virtual int do_startLoop( int acnx, int acny ) override;

   /** coefficient near f(x) */
   PRM_PARAMD( a, 0, "a", "a in dx/dt = a*f(ku*u(t)-x)", "def=1" );
   /** coefficient near u(t)  */
   PRM_PARAMD( ku, 0, "k_u",  "Amplification", "def=1" );
   /** Use u[1] as f(x) */
   PRM_SWITCH( use_u1, 0, "use in_f as f()",
        "Use in_f as source of f()", "sep=col" );
   PRM_SWITCH( use_u2, 0, "u^2",
        "Calculate u^2 before usage", "" );

   PRM_INPUT( in_u, 0, "in_u", "Signal input",  "sep=block" );
   PRM_INPUT( in_f, 0, "in_f", "Functional input", "sep=col" );

   /** old value of x  */
   double x_old = 0;

   Q_CLASSINFO( "nameHintBase",  "lp1_" );
   DCL_DEFAULT_STATIC;
};


#endif


