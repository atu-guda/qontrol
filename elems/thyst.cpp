/***************************************************************************
                          thyst.cpp  -  description
                             -------------------
    begin                : Sat Aug 26 2000
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

#include <cmath>
#include "miscfun.h"
#include "thyst.h"

using namespace std;


const char* THyst::helpstr = "<H1>THyst</H1>\n"
 "Can simulate some simple models of hysteresis: <br>\n"
 "Have 5 parameters: <b>x0, alpha, a, b, type</b>\n"
 " <b>x0, alpha </b> - parameers of hysteresys\n"
 " <b>a, b </b> - output scale\n"
 "Can be changed at any time: <b>x0, alpha</b>.";

TClassInfo THyst::class_info = {
  CLASS_ID_THyst, "THyst", THyst::create,
  &TMiso::class_info, helpstr, clpElem };


THyst::THyst( TDataSet* aparent )
        :TMiso( aparent )
{
  type = 0; x0 = 1; alpha = 0.2; a = 1; b = 0; s = d = 0;
}

THyst::~THyst()
{
}

TDataSet* THyst::create( TDataSet* apar )
{
  return new THyst( apar );
}

const TClassInfo* THyst::getClassInfo(void) const
{
  return &class_info;
}

const char *THyst::getHelp(void) const
{
  return helpstr;
}

double THyst::f( double /* t */ )
{
  double ud, ts , v, u_old;
  if( prm_mod ) {
    fixState(); 
  }
  // atu test
  u_old = d + s;
  // 
  ud = *in_so[0] - u_old; 
  ts = s + ud;
  if( ts >= x0 ) { 
    s = x0;  d += ts - x0;  
  } else { 
    if( ts <= -x0 ) { 
      s = -x0;  d += ts + x0; 
    }  else { 
      s = ts; 
    };
  };
  switch( type ) {
    case 0: v = s + alpha * d; break;
    case 1: v = alpha * d; break;   
    case 2: v = thetta( d ); break;
    case 3: v = sign( d ); break;
    case 4: v = tanh( alpha * d ); break;
    case 5: v = tanh( s + alpha * d ); break;
    default: v = 0;
  };
  v = a * v + b;
  return v;
}

void THyst::fixState(void) 
{
  if( s > x0 )  
    { d += s - x0;  s = x0;  };
  if( s < -x0 ) 
    { d += s + x0;  s = -x0; };
}

int THyst::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  s = d = 0;
  return 0;
}

int THyst::registered = reg();

int THyst::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


// end of thyst.cpp

 
