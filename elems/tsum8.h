#pragma once
/***************************************************************************
                          tsum8.h  - 8-input linear summator
                             -------------------
    begin                : 2015.02.22
    copyright            : (C) 2015-2016 by atu
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

/** 8-input summator, like TLinear
  *@author atu
  */

class TSum8 : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TSum8);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f() noexcept override;
   PRM_PARAMD( a,   0, "&a", "common factor shift", "def=1" );
   PRM_PARAMD( b,   0, "&b", "static shift", "sep=col" );
   PRM_PARAMD( a_0, 0, "a_{&0}", "factor in input 0", "def=1\nsep=block" );
   PRM_PARAMD( a_1, 0, "a_{&1}", "factor in input 1", "def=-1" );
   PRM_PARAMD( a_2, 0, "a_{&2}", "factor in input 2", "sep=col" );
   PRM_PARAMD( a_3, 0, "a_{&3}", "factor in input 3", "" );
   PRM_PARAMD( a_4, 0, "a_{&4}", "factor in input 4", "sep=col" );
   PRM_PARAMD( a_5, 0, "a_{&5}", "factor in input 5", "" );
   PRM_PARAMD( a_6, 0, "a_{&6}", "factor in input 6", "sep=col" );
   PRM_PARAMD( a_7, 0, "a_{&7}", "factor in input 7", "" );

   PRM_INPUT( u_0, 0, "&u_0", "Input 0",  "sep=block" );
   PRM_INPUT( u_1, 0, "u_1",  "Input 1", "" );
   PRM_INPUT( u_2, 0, "u_2",  "Input 2",  "sep=col" );
   PRM_INPUT( u_3, 0, "u_3",  "Input 3", "" );
   PRM_INPUT( u_4, 0, "u_4",  "Input 4",  "sep=col" );
   PRM_INPUT( u_5, 0, "u_5",  "Input 5", "" );
   PRM_INPUT( u_6, 0, "u_6",  "Input 6",  "sep=col" );
   PRM_INPUT( u_7, 0, "u_7",  "Input 7", "" );

   Q_CLASSINFO( "nameHintBase",  "sum_" );
   DCL_DEFAULT_STATIC;
};

