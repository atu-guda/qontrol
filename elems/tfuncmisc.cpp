/***************************************************************************
                          tfuncmisc.cpp  -  description
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

#include <math.h>
#include "miscfun.h"
#include "tfuncmisc.h"

const char* TFuncMisc::helpstr = "<H1>TFuncMisc</H1>\n"
 "Miscelenios nonlinear functions: <br>\n"
 "Argument <b>y</b> calculates as:<br>\n"
 "<b>y = u[0] - u[1] - x0</b><br>\n"
 "Integer parameter <b>type</b> selects type of function.<br>\n"
 "Double parameters <b>a, b, c, d, e, g, x0</b> can be changed at any time\n";


TClassInfo TFuncMisc::class_info = {
  CLASS_ID_TFuncMisc, "TFuncMisc", TFuncMisc::create,
  &TMiso::class_info, helpstr, clpElem | clpPure };


TFuncMisc::TFuncMisc( TDataSet* aparent )
        :TMiso( aparent )
{
  type = 0;
  a = b = 1; c = d = e = g = x0 = 0; 
}

TFuncMisc::~TFuncMisc()
{
}

TDataSet* TFuncMisc::create( TDataSet* apar )
{
  return new TFuncMisc( apar );
}


const TClassInfo* TFuncMisc::getClassInfo(void) const
{
  return &class_info;
}

const char *TFuncMisc::getHelp(void) const
{
  return helpstr;
}

double TFuncMisc::f( double /* t */ )
{
  double v, y;
  y = *in_so[0] - *in_so[1] - x0; 
  switch( type ) {
    case 0:  v = a * deadLine( y, b ); break;
    case 1:  v = a * limitLine( y, b ); break;
    case 2:  v = a * deadLimitLine( y, b, c, d ); break;
    case 3:  v = a * sign( y ); break;
    case 4:  v = a * thetta( y ); break;
    case 5:  v = ( y > 0 ) ? y : 0; v *= a; break;
    case 6:  v = a * threeStateLine( y, b ); break;
    case 7:  v = a * triangleLine( y , b ); break;
    case 8:  v = a * rectLine( y, b, c ); break;
    case 9:  v = ( y >= 0 ) ? (a * y) : ( b * y ); break;
    case 10: v = a * *in_so[0] / ( *in_so[1] +  b );  break;
    case 11: v = a * int( y * b );  break;
    case 12: v = a * ( ( y * b ) - int( y * b ) );  break;
    case 13: v = a * asin( fabs( sin( y * b) ) );  break;
    case 14: v = a * erf( y*b );  break;
    case 15: v = a * fmin( *in_so[0], *in_so[1] );  break;
    case 16: v = a * fmax( *in_so[0], *in_so[1] );  break;
    default: v = 0;
  };
  v += g;
  return v; 
}

int TFuncMisc::registered = reg();

int TFuncMisc::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


// end of tfuncmisc.cpp


