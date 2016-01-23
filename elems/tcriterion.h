#pragma once
/***************************************************************************
                          tcriterion.h  -  description
                             -------------------
    begin                : Sat Sep 2 2000
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


/**miscellaneous criterions
  *@author atu
  */

class TCriterion : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TCriterion);
   DCL_CREATE;
   DCL_STD_INF;

   enum CritType {
     cr_mod_less = 0, cr_mod_greate, cr_less, cr_greate, cr_between
   };
   Q_ENUMS(CritType);
   Q_CLASSINFO( "enum_CritType_0", "|x|<a" );    // cr_mod_less
   Q_CLASSINFO( "enum_CritType_1", "|x|>a" );    // cr_mod_greate
   Q_CLASSINFO( "enum_CritType_2", "x<a" );      // cr_less
   Q_CLASSINFO( "enum_CritType_3", "x>a" );      // cr_greate
   Q_CLASSINFO( "enum_CritType_4", "a<x<b" );    // cr_between

 protected:
   /** main computation function */
   virtual double f() noexcept override;
   /** reimplemented from TMiso to init state */
   virtual int do_startLoop( int acnx, int acny ) override;

   /** type of criterion */
   PRM_LIST( type, efNRC, "Type", "Type of criterion", "enum=CritType" );
   /** coefficients  */
   PRM_PARAMD( a, 0, "a", "level 'a'", "def=0.5" );
   PRM_PARAMD( b, 0, "b", "level 'b'", "def=1" );
   PRM_PARAMD( t0, 0, "t0",  "Start time", "" );
   /** flags and state */
   PRM_SWITCH( useA, efNRC, "Use in_a is a", "Use in_a as 'a' value", "sep=col" );
   PRM_SWITCH( useT0, efNRC, "t0", "Dont work before given time (t0)", "" );
   PRM_SWITCH( useEnable, efNRC, "use Enable", "Use in_ena signal as Enable", "" );
   PRM_SWITCH( useLock, efNRC, "Lock", "Lock after first front", "sep=col" );
   PRM_SWITCH( usePulse, efNRC, "Pulse Output", "output is pulse +1, 0, -1", "" );
   PRM_SWITCH( termSim,  efNRC, "Terminate simulation", "Terminate simulation if condition is true", "" );
   PRM_INT( termCode, efNRC, "Term code", "Terminations code in !=0", "def=1" );

   PRM_INT( st, efInner, "state", "current state", "");

   PRM_INPUT( in_pos, 0, "in+", "Positive input",  "sep=block" );
   PRM_INPUT( in_neg, 0, "in-", "Negative input", "sep=col" );
   PRM_INPUT( in_ena, 0, "en", "Signal to enable",  "sep=col" );
   PRM_INPUT( in_a,   0, "in_a", "Reference signal", "sep=col" );

   Q_CLASSINFO( "nameHintBase",  "crit_" );
   DCL_DEFAULT_STATIC;
};



