/***************************************************************************
                          trelaxgen.h  -  description
                             -------------------
    begin                : 2014.10.28
    copyright            : (C) 2014-2014 by atu
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

#ifndef TRELAXGEN_H
#define TRELAXGEN_H

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
   virtual double f( double t ) override;
   /** reimplemented from TMiso to reset */
   virtual int do_startLoop( int acnx, int acny ) override;

   PRM_DOUBLE( c, 0, "c", "Capacitance", "def=1\nmin=1e-16" );
   PRM_DOUBLE( r_1, 0, "r_1", "Charge resistance", "def=1\nmin=1e-6\nsep=col" );
   PRM_DOUBLE( r_2, 0, "r_2", "Discharge resistance", "def=0.2\nmin=1e-6" );
   PRM_DOUBLE( v_1, 0, "v_1", "Charge limit", "def=2\nsep=col" );
   PRM_DOUBLE( v_2, 0, "v_2", "Discharge limit", "def=0.1" );
   /** misc flags */
   PRM_SWITCH( useCharge, efNoRunChange, "Use i_ch", "Use i_ch as charge current", "sep=block" );
   PRM_SWITCH( useDischarge, efNoRunChange, "Use i_dis", "Use i_dis as discharge current", "" );
   PRM_SWITCH( useTrig, efNoRunChange, "Use trig", "Use trig as signal to discharge", "" );
   PRM_SWITCH( useContCharge, efNoRunChange, "Contiguous charge", "Use contiguous charging", "sep=col" );
   PRM_SWITCH( useContDischarge, efNoRunChange, "Contiguous discharge", "Use contiguous discharging", "" );

   PRM_DOUBLE( v,     efInner, "v", "voltage", "def=0" );
   PRM_DOUBLE( isDis, efInner, "isDis", "1 means generator discharges now", "def=0" );
   PRM_DOUBLE( I,     efInner, "I", "total charge/discharge current", "def=0" );
   PRM_DOUBLE( dv_ch, efInner, "dv_ch", "charging voltage difference ", "def=0" );
   PRM_DOUBLE( dv_dis,efInner, "dv_ch", "discharging voltage difference ", "def=0" );

   PRM_INPUT( v_in,  0, "v_in",  "Voltage input",     "sep=block" );
   PRM_INPUT( i_ch,  0, "i_ch",  "Charge current",    "sep=col" );
   PRM_INPUT( i_dis, 0, "i_dis", "Discharge current", "sep=col" );
   PRM_INPUT( trig,  0, "trig",  "Force discharge",   "sep=col" );
   DCL_DEFAULT_STATIC;
};


#endif
