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

#ifndef TPID_H
#define TPID_H

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
   /** main computation function */
   virtual double f( double t ) override;
   /** will be called before any action -- good place for allocs */
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt ) override;
   /** reimplemented from TMiso to reset integrators and diffs */
   virtual int do_startLoop( int acnx, int acny ) override;

   /** coefficients  */
   PRM_DOUBLE( kd2, 0, "k_{d2}", "Coefficient in second derivation", "");
   PRM_DOUBLE( kd1, 0, "k_{d1}", "Coefficient in first derivation" , "");
   PRM_DOUBLE( kp,  0, "k_p" ,   "Coefficient in proportional part", "sep=col\ndef=1");
   PRM_DOUBLE( ki1, 0, "k_{i1}", "Coefficient in first integrator" , "sep=col");
   PRM_DOUBLE( ki2, 0, "k_{i2}", "Coefficient in second integrator", "");
   /** should output be devided to t */
   PRM_SWITCH( aver, efNRC, "Average", "Calculate average value", "sep=col" );

   PRM_INPUT( in_u, 0, "input", "Single input u(x)",  "sep=block" );

   /** accumulators and old values */
   double vi1 = 0, vi2 = 0, u_old = 0, u_old2 = 0, tdt2 = 1;
   /** flag for special handling initial steps */
   int start = 0;

   Q_CLASSINFO( "nameHintBase",  "pid_" );
   DCL_DEFAULT_STATIC;
};

#endif


