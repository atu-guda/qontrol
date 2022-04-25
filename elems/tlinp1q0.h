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
   virtual void preCalc() override;
   void pre_f() noexcept;

   PRM_PARAMD( a,         0, QSL("&a"),    QSL("a in dx/dt = a*f(ku*u(t)-x)"),           QSL("def=1") );
   PRM_PARAMD( ku,        0, QSL("&k_u"),  QSL("Amplification: coefficient near u(t) "), QSL("def=1") );

   PRM_PARAMD( vmin,      0, QSL("vmin") , QSL("Minimum value")                        , QSL("def=-1e100\nsep=col") );
   PRM_PARAMD( vmax,      0, QSL("vmax") , QSL("Maximum value")                        , QSL("def=1e100") );

   PRM_SWITCH( use_u1,    0, QSL("use in_f"), QSL("Use in_f as source of f()")         , QSL("sep=col") );
   PRM_SWITCH( use_u2,    0, QSL("u^&2"),     QSL("Calculate u^2 before usage")        , QSL("sep=col") );
   PRM_SWITCH( use_sqrt0, 0, QSL("sqrt0(x)"), QSL("Output sqrt0(x)")                   , QES );

   PRM_INPUT( in_u,        0, QSL("&in_u"),  QSL("Signal input"),     QSL("sep=block") );
   PRM_INPUT( in_n,        0, QSL("in_n"),   QSL("Negative input"),   QSL("sep=col") );
   PRM_INPUT( in_f,        0, QSL("in_f"),   QSL("Functional input"), QSL("sep=col") );

   PRM_DOUBLE( tau,  efInner, QSL("\\tau"),   QSL("Effective time"),  QSL("def=1")  );
   PRM_DOUBLE(  u0,  efInner, QSL("u0"),      QSL("u_{in}-u_{n}"),    QES  );

   PRM_DOUBLE(  x2,  efInner, QSL("x^2"),     QSL("Squared output"), QES );

   /** old value of x  */
   double x_old = 0;

   Q_CLASSINFO( "nameHintBase",  "lp1_" );
   DCL_DEFAULT_STATIC;
};




