#pragma once
/***************************************************************************
                          tpid.h  -  description
                             -------------------
    begin                : Sun Aug 27 2000
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

/**combines integrators, differentiators and proportional
  *@author atu
  */

class TPid : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TPid);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   virtual double f() noexcept override;
   virtual int do_preRun() override;
   virtual int miso_startLoop( long acnx, long acny ) override;

   PRM_PARAMD( kd2,      0, QSL("k_{d2}"),   QSL("Coefficient in second derivation"), QES );
   PRM_PARAMD( kd1,      0, QSL("k_{&d1}"),  QSL("Coefficient in first derivation") , QES );
   PRM_PARAMD( kp,       0, QSL("k_{&p}") ,  QSL("Coefficient in proportional part"), QSL("sep=col\ndef=1") );
   PRM_PARAMD( b,        0, QSL("&b"),       QSL("Const shift")                     , QES );
   PRM_PARAMD( ki1,      0, QSL("k_{&i1}"),  QSL("Coefficient in first integrator") , QSL("sep=col") );
   PRM_PARAMD( ki2,      0, QSL("k_{i2}"),   QSL("Coefficient in second integrator"), QES );
   PRM_PARAMD( vmin,     0, QSL("vmin") ,    QSL("Minimum value")                   , QSL("def=-1e100\nsep=col") );
   PRM_PARAMD( vmax,     0, QSL("vmax") ,    QSL("Maximum value")                   , QSL("def=1e100") );
   PRM_SWITCH( aver, efNRC, QSL("&Average"), QSL("Calculate average value")         , QSL("sep=block") );

   PRM_INPUT( in_u,      0, QSL("in_{&u}"),  QSL("Input u(x)"),     QSL("sep=block") );
   PRM_INPUT( in_n,      0, QSL("in_{n}") ,  QSL("Negative input"), QSL("sep=col") );

   PRM_DOUBLE(  d1, efInner, QSL("d1"),          QSL("dx/dt"), QES );
   PRM_DOUBLE(  d2, efInner, QSL("d2"),      QSL("d^2x/dt^2"), QES );
   PRM_DOUBLE( vi1, efInner, QSL("vi1"), QSL("integral x dt"), QES );
   PRM_DOUBLE( vi2, efInner, QSL("vi2"),  QSL("int int x dt"), QES );

   PRM_DOUBLE(  x2, efInner, QSL("x^2"), QSL("Squared output"), QES );

   /** accumulators and old values */
   double u_old = 0, u_old2 = 0, tdt2 = 1;
   /** flag for special handling initial steps */
   int start = 0;

   Q_CLASSINFO( "nameHintBase",  "pid_" );
   DCL_DEFAULT_STATIC;
};



