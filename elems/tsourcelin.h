#pragma once
/***************************************************************************
                          tsourcelin.h  -  description
                             -------------------
    begin                : Thu Aug 24 2003
    copyright            : (C) 2003-2016 by atu
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


#include "tmiso.h"

/** This element provide source, which consist of
  * linear elements.
  * @author atu
  */

constexpr int def_n_slopes=16; // keep with init strings

class TSourceLin : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TSourceLin);
   DCL_CREATE;
   DCL_STD_INF;
   /** called before each inner param loop. Unused param - -1 */
   void recalc(void);
 protected:
   virtual double f() noexcept override;
   virtual int miso_startLoop( long acnx, long acny ) override;
   virtual void do_post_set() override;

   PRM_PARAMD( a,  0, "&a", "Common factor", "sep=col\ndef=1" );
   PRM_PARAMD( b,  0, "&b", "Shift", "" );

   PRM_SWITCH( cycle_before, 0, "Cycle bef&ore", "Continue function periodicaly at t<0", "sep=col\ndef=1" );
   PRM_SWITCH( cycle_after,  0, "Cycle af&ter",  "Continue function periodicaly at t>T_c", "def=1" );

   PRM_DOUBLE( T_c,    efRO, "T_c",     "Period", "sep=block" );
   PRM_DOUBLE( omega,  efRO, "\\omega", "Frequency", "sep=col" );
   PRM_DOUBLE( t_r,  efInner, "t_r", "relative time", "" );
   PRM_DOUBLE( t_rr,  efInner, "t_{rr}", "relative to slope start time", "" );
   PRM_DOUBLE( t_r0,  efInner, "t_{r0}", "slope start time", "" );
   PRM_DOUBLE( n_rr,  efInner, "n_{rr}", "slope number", "" );

   PRM_INPUT( u, 0, "&u", "Input",  "sep=block\ndef=t" );

   // time 0 means end of sequence - to start
   PRM_DOUBLE_ARR( t_int, efNRC, "t_{i}", "Time intervals", "N=16\ndef=0\ndefs=1 1 1  1  1 0\nmin=0\nsep=tab\ntabname=Arrays" );
   PRM_DOUBLE_ARR( vs,    efNRC, "vs",    "start value",    "N=16\ndef=0\ndefs=0 0 1 -1 -1 0\nsep=col" );
   PRM_DOUBLE_ARR( ve,    efNRC, "ve",    "end value",      "N=16\ndef=0\ndefs=0 1 1 -1  0 0\nsep=col" );

   /** slope values of each line */
   dvector slopes;
   dvector tsta; //* start time of every slope
   /** number of lines */
   int n_lin = 0;

   Q_CLASSINFO( "nameHintBase",  "solin_" );
   DCL_DEFAULT_STATIC;
};


