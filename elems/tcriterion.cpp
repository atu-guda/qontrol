/***************************************************************************
                          tcriterion.cpp  -  description
                             -------------------
    begin                : Sat Sep 2 2000
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
#include "tcriterion.h"


const char* TCriterion::helpstr = "<H1>TCriterion</H1>\n"
 "Miscellaneous criterions: <br>\n"
 "Parameter <b>type</b> -- type of criterion. <br>\n"
 "Parameters <b>a,b</b> -- to with compare (x=u[0]-u[1]). <br>\n"
 "Parameter <b>t0</b> -- start time (if enabled). <br>\n"
 "5 flags: <b>useA, useT0, useEnable, useLock, usePulse</b>.";

TClassInfo TCriterion::class_info = {
  CLASS_ID_TCriterion, "TCriterion", TCriterion::create,
  &TMiso::class_info, helpstr, clpElem };


TCriterion::TCriterion( TDataSet* aparent )
        :TMiso( aparent )
{
  a = 0.5; b = 1.0; type = 0; t0 = 0; 
  useA = useT0 = useEnable = useLock = usePulse = 0;
}

TCriterion::~TCriterion()
{
}

TDataSet* TCriterion::create( TDataSet* apar )
{
  return new TCriterion( apar );
}


const TClassInfo* TCriterion::getClassInfo(void) const
{
  return &class_info;
}

const char *TCriterion::getHelp(void) const
{
  return helpstr;
}

int TCriterion::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  st = 0;
  return 0;
}

double TCriterion::f( double t )
{
  int ok, rv; 
  double aa, x;
  if( useT0 && t < t0 ) return 0;
  x = *in_so[0] - *in_so[1];
  if( useLock && st > 0 )
    return usePulse ? 0 : 1;
  if( useEnable && *in_so[2] < 0.1 ) {
    if( !st ) return 0;
    return usePulse ? 0 : 1;
  };
  aa = a;
  if( useA )
    aa = *in_so[3];
  switch( type ) {
    case 0: ok = ( fabs( x ) < aa ); break;
    case 1: ok = ( fabs( x ) > aa ); break;
    case 2: ok = ( x < aa ); break;
    case 3: ok = ( x > aa ); break;
    case 4: ok = ( (x > aa) && (x < b) ); break;
    default: ok = 0;
  };
  if( usePulse ) {
    if( ok == st ) {
      rv = 0;
    } else {
      rv = ok > st ? 1 : -1;
    };
  } else {
    rv = ok;
  };
  st = ok;
  return rv;
}

int TCriterion::registered = reg();

int TCriterion::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


// end of tcriterion.cpp


