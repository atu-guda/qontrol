#pragma once
/***************************************************************************
                          trelaxgen.h  -  description
                             -------------------
    begin                : 2014.10.28
    copyright            : (C) 2014-2016 by atu
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


/** Relaxation generator
  *@author atu
  */

class TRelaxGen : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TRelaxGen);
   DCL_CREATE;
   DCL_STD_INF;
   enum GenType { gen_def=0, gen_mai, gen_dual };
 protected:
   /** main computation function */
   virtual double f() noexcept override;
   /** reimplemented from TMiso to reset */
   virtual int miso_startLoop( long acnx, long acny ) override;

   PRM_PARAMD( c,   0, "&c",   "Capacitance", "def=1\nmin=1e-16" );
   PRM_PARAMD( r_1, 0, "&r_1", "Charge resistance", "def=1\nmin=1e-6\nsep=col" );
   PRM_PARAMD( r_2, 0, "r_2",  "Discharge resistance", "def=0.2\nmin=1e-6" );
   PRM_PARAMD( v_1, 0, "&v_1", "Charge limit", "def=2\nsep=col" );
   PRM_PARAMD( v_2, 0, "v_2",  "Discharge limit", "def=0.1" );
   /** misc flags */
   PRM_SWITCH( useCharge, efNRC, "Use i_{ch}", "Use i_ch as charge current", "sep=block" );
   PRM_SWITCH( useDischarge, efNRC, "Use i_{dis}", "Use i_dis as discharge current", "" );
   PRM_SWITCH( useTrig, efNRC, "Use trig", "Use trig as signal to discharge", "" );
   PRM_SWITCH( useContCharge, efNRC, "Contiguous charge", "Use contiguous charging", "sep=col" );
   PRM_SWITCH( useContDischarge, efNRC, "Contiguous discharge", "Use contiguous discharging", "" );

   PRM_DOUBLE( v,     efInner, "v", "voltage", "def=0" );
   PRM_DOUBLE( isDis, efInner, "isDis", "1 means generator discharges now", "def=0" );
   PRM_DOUBLE( I,     efInner, "I", "total charge/discharge current", "def=0" );
   PRM_DOUBLE( dv_ch, efInner, "dv_{ch}", "charging voltage difference ", "def=0" );
   PRM_DOUBLE( dv_dis,efInner, "dv_{ch}", "discharging voltage difference ", "def=0" );

   PRM_INPUT(   v_in,  0, "v_{&in}", "Voltage input",     "sep=block" );
   PRM_INPUT(   i_ch,  0, "i_{ch}",  "Charge current",    "sep=col" );
   PRM_INPUT(  i_dis,  0, "i_{dis}", "Discharge current", "sep=col" );
   PRM_LOGICIN( trig,  0, "trig",    "Force discharge",   "sep=col" );

   Q_CLASSINFO( "nameHintBase",  "relax_" );
   DCL_DEFAULT_STATIC;
};



