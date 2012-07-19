/***************************************************************************
                          tpid.cpp  -  description
                             -------------------
    begin                : Sun Aug 27 2000
    copyright            : (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "tpid.h"

const char* TPid::helpstr = "<H1>TPid</H1>\n"
 "Integrators, differenciators and proportional element: <br>\n"
 "Have 6 parameters: <b>kd2, kd1, kp, ki1, ki1, aver</b>.\n"
 "Coefficients can be changed at any time, aver - no.";

TClassInfo TPid::class_info = {
  CLASS_ID_TPid, "TPid", TPid::create,
  &TMiso::class_info, helpstr, clpElem };


TPid::TPid( TDataSet* aparent )
        :TMiso( aparent ),
	PRM_INIT( kd2,  "k_d2" ),
	PRM_INIT( kd1,  "k_d1" ),
	PRM_INIT( kp,   "k_p" ),
	PRM_INIT( ki1,  "k_i1" ),
	PRM_INIT( ki2,  "k_i2" ),
	PRM_INIT( aver, "Average" )
{
  kd2 = kd1 = ki2 = ki1 = 0; kp = 1; aver = 0;
  tdt2 = 1; vi1 = vi2 = u_old = u_old2 = 0;

  PRMI(kd2).setDescr(  "Coefficient in second derivation" );
  PRMI(kd1).setDescr(  "Coefficient in first derivation" );
  PRMI(kp).setDescr(   "Coefficient in proportional part" );
  PRMI(ki1).setDescr(  "Coefficient in first intergator" );
  PRMI(ki2).setDescr(  "Coefficient in second intergator" );
  PRMI(aver).setDescr( "Average" );
}

TPid::~TPid()
{
}

TDataSet* TPid::create( TDataSet* apar )
{
  return new TPid( apar );
}


const TClassInfo* TPid::getClassInfo(void) const
{
  return &class_info;
}

const char *TPid::getHelp(void) const
{
  return helpstr;
}

const char** TPid::getIcon(void) const
{
  return icon;
}

int TPid::do_preRun( int /*run_tp*/, int /*an*/, 
                     int /*anx*/, int /*any*/, double /*adt*/ )
{
  if( tdt < 1e-100 ) 
    return 1;
  tdt2 = tdt * tdt;
  return 0;
}

int TPid::startLoop( int acnx, int acny )
{
  int rc = TMiso::startLoop( acnx, acny );
  vi1 = vi2 = u_old = u_old2 = 0;
  start = 1;
  return rc;
}

double TPid::f( double t )
{
  double v, d1, d2;
  v = 0;
  if( start == 1 ) {
    start = 2; u_old = *in_so[0]; vi1 += *in_so[0] * tdt; vi2 += vi1 * tdt;
    v = kp * *in_so[0] + ki1 * vi1 + ki2 * vi2;
    return v;
  };
  if( start == 2 ) {
    start = 0; d1 = ( *in_so[0] - u_old ) / tdt;
    vi1 += *in_so[0] * tdt; vi2 += vi1 * tdt;
    u_old2 = u_old; u_old = *in_so[0];
    v = kd1 * d1 + kp * *in_so[0] + ki1 * vi1 + ki2 * vi2;
    return v;
  };
  d1 = ( *in_so[0] - u_old ) / tdt;
  d2 = ( *in_so[0] - 2*u_old + u_old2 ) / tdt2;
  vi1 += *in_so[0] * tdt;
  vi2 += vi1 * tdt;
  v = kd2 * d2 + kd1 * d1 + kp * *in_so[0] + ki1 * vi1 + ki2 * vi2;
  if( aver )
    v /= t;
  u_old2 = u_old; u_old = *in_so[0];
  return v; 
}

int TPid::registered = reg();

int TPid::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


/* XPM */
const char* TPid::icon[] = {
/* width height num_colors chars_per_pixel */
"    32    32       16            1",
/* colors */
"` c #000000",
". c #800000",
"# c #008000",
"a c #808000",
"b c #000080",
"c c #800080",
"d c #008080",
"e c #808080",
"f c #c0c0c0",
"g c #ff0000",
"h c #00ff00",
"i c #ffff00",
"j c #0000ff",
"k c #ff00ff",
"l c #00ffff",
"m c #ffffff",
/* pixels */
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll",
"llllllggggllllllllllllllllllllll",
"lllllgggggglllllllllllllllllllll",
"llllggllllggllllllllllllllllllll",
"llllggllllggllggggggggggggglllll",
"llllggllllglllggggggggggggglllll",
"lllllggllllllllggllllllllgllllll",
"lllllggllllllllgggllllllggllllll",
"llllllggglllllllggllllllglllllll",
"lllllllgglllllllgggllllgglllllll",
"lllllllggllllllllggllllgglllllll",
"llllllllgglllllllgggllggllllllll",
"llllllllggllllllllggllggllllllll",
"lllllllllgglllllllggggglllllllll",
"lllllllllggllllllllgggglllllllll",
"lllllllllggllllllllgggllllllllll",
"lllllllllgglllllllllggllllllllll",
"lllllglllgglllllllllllllllllllll",
"llllgglllgglllllllllllllllllllll",
"llllgggllgglllllllllllllllllllll",
"lllllgggggllllllllllllllllllllll",
"llllllllllllllllllllllllllllllll"
};

// end of tpid.cpp


