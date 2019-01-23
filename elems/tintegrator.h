#pragma once
/***************************************************************************
                          tintegrator.h  -  description
                             -------------------
    begin                : Wed Aug 30 2000
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


/**realistic integrator with reset, discharge and floating base
  *@author atu
  */

class TIntegrator : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TIntegrator);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f() noexcept override;
   /** reimplemented from TMiso to init state */
   virtual int miso_startLoop( long acnx, long acny ) override;

   PRM_PARAMD( ki,   0, "&k_i",  "Factor before integral", "def=1" );
   PRM_PARAMD( vmin, 0, "Min limit", "Limit min value", "def=-10000" );
   PRM_PARAMD( vmax, 0, "Max limit", "Limit max value", "def=10000" );

   PRM_DOUBLE( v,     efInner, "v", "Current value", "" );
   PRM_DOUBLE( t_rst, efInner, "t_{rst}", "time from last reset", "" );
   PRM_DOUBLE( v_a,   efInner, "v_a", "Average value", "" );
   /** flags */
   PRM_SWITCH( useAver,  efNRC, "output &Average", "Output average value", "sep=col" );
   PRM_SWITCH( useMin,   efNRC, "limit Min value", "limit output to minimum value", "" );
   PRM_SWITCH( useMax,   efNRC, "limit Max value", "limit output to maximum value", "" );
   PRM_SWITCH( useSqIn,  efNRC, "x^{&2} on input", "Calculate square on input", "sep=col" );
   PRM_SWITCH( useSqrOut,efNRC, "\u221As&qrt(output)", "Calculate square root on output", "" );
   PRM_SWITCH( invKi,    efNRC, "Inverse k_i", "Use 1/ki instead of ki", "" );

   PRM_INPUT(     in_u, 0, "&in_u",   "Main input",  "sep=block" );
   PRM_LOGICIN( in_rst, 0, "rst",   "Reset signal", "sep=col" );
   PRM_INPUT(    v_rst, 0, "v_{rst}", "Value of v on reset", "sep=col" );

   double v_old = 0;
   double last_rst = 0;

   Q_CLASSINFO( "nameHintBase",  "int_" );
   DCL_DEFAULT_STATIC;
};




