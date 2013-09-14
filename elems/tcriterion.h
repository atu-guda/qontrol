/***************************************************************************
                          tcriterion.h  -  description
                             -------------------
    begin                : Sat Sep 2 2000
    copyright            : (C) 2000-2013 by atu
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

#ifndef TCRITERION_H
#define TCRITERION_H

#include <tmiso.h>


/**miscellaneous criterions
  *@author atu
  */

class TCriterion : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TCriterion);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   /** reimplemented from TMiso to init state */
   virtual int do_startLoop( int acnx, int acny ) override;

   /** type of criterion */
   PRM_LIST( type, efNRC, "Type", "Type of criterion", "",
       "|x|<a\n|x|>a\nx<a\nx>a\na<x<b"  );
   /** coefficients  */
   PRM_DOUBLE( a, 0, "a", "level 'a'", "def=0.5" );
   PRM_DOUBLE( b, 0, "b", "level 'b'", "def=1" );
   PRM_DOUBLE( t0, 0, "t0",  "Start time", "" );
   /** flags and state */
   PRM_SWITCH( useA, efNRC, "Use in_a is a", "Use in_a as 'a' value", "sep=col" );
   PRM_SWITCH( useT0, efNRC, "t0", "Dont work before given time (t0)", "" );
   PRM_SWITCH( useEnable, efNRC, "use Enable", "Use in_ena signal as Enable", "" );
   PRM_SWITCH( useLock, efNRC, "Lock", "Lock after first front", "sep=col" );
   PRM_SWITCH( usePulse, efNRC, "Pulse Output", "output is pulse +1, 0, -1", "" );
   PRM_INT( st, efInner, "state", "current state", "");

   PRM_INPUT( in_pos, 0, "input+", "Positive input",  "sep=block" );
   PRM_INPUT( in_neg, 0, "input-", "Negative input", "sep=col" );
   PRM_INPUT( in_ena, 0, "enable", "Signal to enable",  "sep=col" );
   PRM_INPUT( in_a,   0, "input a", "Reference signal", "sep=col" );

   DCL_DEFAULT_STATIC;
};

#endif


