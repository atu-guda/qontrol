#pragma once
/***************************************************************************
    tsinsum.h -- sum of sinuses
                             -------------------
    begin                : 2016.12.26
    copyright            : (C) 2016-2019 by atu
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

class TSinSum : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TSinSum);
   DCL_CREATE;
   DCL_STD_INF;

 protected:
   virtual double f() noexcept override;
   Q_INVOKABLE virtual double f_d( double arg0, double arg1 = 0, double arg2 = 0, double arg3 = 0 ) override;

   PRM_PARAMD( x0    , 0 , "&x0"     , "Input shift "         , "def=0" );
   PRM_PARAMD( c_0   , 0 , "&c_0"    , "Constant base"        , "sep=col" );
   PRM_PARAMD( c_lin , 0 , "c_{lin}" , "Linear coeff"         , "sep=col" );

   PRM_PARAMD( b     , 0 , "&b"      , "Scale for input "     , "def=1\nsep=block" );
   PRM_PARAMD( d     , 0 , "&d"      , "Denominator"          , "def=1\nsep=col" );
   PRM_SWITCH( mul_pi, 0, "*Pi"      , "sing arg *\\pi"       , "sep=col" );
   PRM_SWITCH( use_bd, 0, "use b/d"  , "use 8b/d fir linear"  , "sep=col" );


   PRM_PARAMD( a_1   , 0 , "a_1"     , "Amplitude for ch 1"   , "sep=block" );
   PRM_PARAMD( f_1   , 0 , "f_1"     , "Frequency  for ch 1"  , "sep=col" );
   PRM_PARAMD( p_1   , 0 , "p_1"     , "Phase for ch 1"       , "sep=col" );

   PRM_PARAMD( a_2   , 0 , "a_2"     , "Amplitude for ch 2"   , "sep=block" );
   PRM_PARAMD( f_2   , 0 , "f_2"     , "Frequency  for ch 2"  , "sep=col" );
   PRM_PARAMD( p_2   , 0 , "p_2"     , "Phase for ch 2"       , "sep=col" );

   PRM_PARAMD( a_3   , 0 , "a_3"     , "Amplitude for ch 3"   , "sep=block" );
   PRM_PARAMD( f_3   , 0 , "f_3"     , "Frequency  for ch 3"  , "sep=col" );
   PRM_PARAMD( p_3   , 0 , "p_3"     , "Phase for ch 3"       , "sep=col" );

   PRM_PARAMD( a_4   , 0 , "a_4"     , "Amplitude for ch 4"   , "sep=block" );
   PRM_PARAMD( f_4   , 0 , "f_4"     , "Frequency  for ch 4"  , "sep=col" );
   PRM_PARAMD( p_4   , 0 , "p_4"     , "Phase for ch 4"       , "sep=col" );

   PRM_PARAMD( a_5   , 0 , "a_5"     , "Amplitude for ch 5"   , "sep=block" );
   PRM_PARAMD( f_5   , 0 , "f_5"     , "Frequency  for ch 5"  , "sep=col" );
   PRM_PARAMD( p_5   , 0 , "p_5"     , "Phase for ch 5"       , "sep=col" );

   PRM_INPUT(  u_0   , 0 , "u_{&0}"  , "input"                ,  "sep=block" );

   PRM_DOUBLE( p     , efInner , "p"    , ""       , "" );
   PRM_DOUBLE( x2    , efInner, "x^2"   , "Squared output", "" );

   Q_CLASSINFO( "nameHintBase",  "si_" );
   DCL_DEFAULT_STATIC;
};



