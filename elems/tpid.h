#pragma once
/***************************************************************************
                          tpid.h  -  description
                             -------------------
    begin                : Sun Aug 27 2000
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

   PRM_PARAMD( kd2,      0, "k_{d2}",   "Coefficient in second derivation", "");
   PRM_PARAMD( kd1,      0, "k_{&d1}",  "Coefficient in first derivation" , "");
   PRM_PARAMD( kp,       0, "k_{&p}" ,  "Coefficient in proportional part", "sep=col\ndef=1");
   PRM_PARAMD( ki1,      0, "k_{&i1}",  "Coefficient in first integrator" , "sep=col");
   PRM_PARAMD( ki2,      0, "k_{i2}",   "Coefficient in second integrator", "");
   PRM_SWITCH( aver, efNRC, "&Average", "Calculate average value", "sep=col" );

   PRM_INPUT( in_u,      0, "in_{&u}",  "Input u(x)",  "sep=block" );

   /** accumulators and old values */
   double vi1 = 0, vi2 = 0, u_old = 0, u_old2 = 0, tdt2 = 1;
   /** flag for special handling initial steps */
   int start = 0;

   Q_CLASSINFO( "nameHintBase",  "pid_" );
   DCL_DEFAULT_STATIC;
};



