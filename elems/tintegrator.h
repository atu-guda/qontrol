#pragma once
/***************************************************************************
                          tintegrator.h  -  description
                             -------------------
    begin                : Wed Aug 30 2000
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

   PRM_PARAMD( ki, 0, "k_i",  "Factor before integral", "def=1" );
   PRM_PARAMD( dis, 0, "Disc coeff", "Discharde coeff (if enabled)", "def=1e-4" );
   PRM_PARAMD( vmin, 0, "Min limit", "Limit min value", "def=-10000" );
   PRM_PARAMD( vmax, 0, "Max limit", "Limit max value", "def=10000" );

   PRM_DOUBLE( v, efInner, "v", "Current value", "" );
   PRM_DOUBLE( t_rst, efInner, "t_{rst}", "time from last reset", "" );
   PRM_DOUBLE( v_a, efInner, "v_a", "Average value", "" );
   /** flags */
   PRM_SWITCH( useMin,   efNRC, "use Min value", "limit output to minimum value", "sep=col" );
   PRM_SWITCH( useMax,   efNRC, "use Max value", "limit output to maximum value", "" );
   PRM_SWITCH( useReset, efNRC, "use Reset", "Use in_rst as Reset signal", "" );
   PRM_SWITCH( useBase,  efNRC, "use Base",  "Use in_base as base signal", "" );
   PRM_SWITCH( useAdd,   efNRC, "add base", "Add base to level", "" );
   PRM_SWITCH( useDis,   efNRC, "use Discharge", "Use discharge coefficient", "sep=col" );
   PRM_SWITCH( useHold,  efNRC, "Hold", "Hold output 1 tick after reset", "" );
   PRM_SWITCH( useAver,  efNRC, "Calc Aver", "Calculate average value", "" );
   PRM_SWITCH( useSqIn,  efNRC, "x^2 on input", "Calculate square on input", "");
   PRM_SWITCH( useSqrOut,efNRC, "\\sqrt(output)", "Calculate square root on output", "");

   PRM_INPUT( in_u, 0, "in_u", "Main input",  "sep=block" );
   PRM_INPUT( in_rst, 0, "rst", "Reset signal", "sep=col" );
   PRM_INPUT( in_base, 0, "Base", "Fourth input", "sep=col" );

   double v_old = 0;

   Q_CLASSINFO( "nameHintBase",  "int_" );
   DCL_DEFAULT_STATIC;
};




