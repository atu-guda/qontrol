#pragma once
/***************************************************************************
                          tadjgen.h  -  description
                             -------------------
    begin                : Wed Aug 30 2000
    copyright            : (C) 2000-2015 by atu
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


/**Controlled on frequency generator sqare pulses
  *@author atu
  */

class TAdjGen : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TAdjGen);
   DCL_CREATE;
   DCL_STD_INF;
   enum GenType { gen_def=0, gen_mai, gen_dual };
   Q_ENUM(GenType);
   Q_CLASSINFO( "enum_GenType_0", "Default" );                 // gen_def
   Q_CLASSINFO( "enum_GenType_1", "MAI" );                     // gen_mai
   Q_CLASSINFO( "enum_GenType_2", "Dual(\\omega,\\omega_2)" ); // gen_dual
 protected:
   virtual double f() noexcept override;
   /** reimplemented from TMiso to reset */
   virtual int miso_startLoop( long acnx, long acny ) override;

   /** type of averaging, */
   PRM_LIST( type, efNRC, "&Type", "Type of generator", "enum=GenType" );
   /** misc flags */
   // PRM_SWITCH( useReset, efNRC, "use &Reset", "Use in_rst  signal", "sep=col" );
   // PRM_SWITCH( useLock, efNRC, "use Lock", "Use lock signal", "" );
   PRM_SWITCH( outStrobe, efNRC, "Output Strobe", "Output only at switching moments", "" );
   PRM_SWITCH( useZero, efNRC, "0 is neg. out", "Negative output is 0, not -1", "sep=col" );
   PRM_SWITCH( useSignStrobe, efNRC, "Signed strobe", "Output +1 - switch to ON, -1 - OFF", "" );
   PRM_SWITCH( usePlusStrobe, efNRC, "Only + strobe", "Output only positive strobe", "" );
   PRM_SWITCH( useF, efNRC, "input &F", "input is F, not omega", "sep=col" );

   /** koeff to F-omega conversion if useF */
   PRM_PARAMD( omega_0, 0, "\\omega_0", "Base frequency if flag 'input F' is set", "def=1.2" );
   PRM_PARAMD( k_omega, 0, "&k_\\omega", "Frequency scale if flag 'input F' is set", "def=1");
   /**  accumulators, ... */
   PRM_DOUBLE( ctt, efInner, "ctt", "Accumulator 0", "" );
   PRM_DOUBLE( ig, efInner,  "ig",  "Accumulator 1", "" );
   PRM_DOUBLE( ig2, efInner, "ig2", "Accumulator 2", "" );
   PRM_DOUBLE( dlt, efInner, "dlt", "Delta during last tick", "" );
   PRM_DOUBLE( tick_T, efInner, "tick_T", "period of last tick", "" );
   PRM_DOUBLE( tick_old, efInner, "tick_old", "time of last tick", "" );
   PRM_DOUBLE( av, efInner,  "av",  "Average 1", "" );
   PRM_DOUBLE( av2, efInner, "av2", "Average 2", "" );
   PRM_DOUBLE( av_diff, efInner, "av_diff", "difference of averages", "" );

   PRM_INPUT(  in_omega, 0, "\\omega_{&in}", "omega input",  "sep=block" );
   PRM_LOGICIN(  in_rst, 0, "reset",         "reset input input", "sep=col" );
   PRM_LOGICIN( in_lock, 0, "lock",          "lock input",  "sep=col" );
   PRM_INPUT( in_omega2, 0, "\\omega_{&2}",  "second omega input", "sep=col" );

   PRM_INT( currOut, efInner, "Current out", "Current INT output", "");
   PRM_DOUBLE( tick, efInner, "Tick", "1 means generator ticks now", "" );

   /** real tick: exported to double tick */
   int real_tick;
   double cav, cav2; // current values for av, av2

   Q_CLASSINFO( "nameHintBase",  "gen_" );
   DCL_DEFAULT_STATIC;
};



