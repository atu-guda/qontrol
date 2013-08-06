/***************************************************************************
                          tcounter.cpp  -  description
                             -------------------
    begin                : Sun Nov 19 2000
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

#include "tcounter.h"

const char* TCounter::helpstr = "<H1>TCounter</H1>\n"
 "Counts input pulses up to <b>n</b>, and changes output by type.<br>\n"
 "Parameters:<br>\n"
 " - <b>type</b> -- output type: level, pulse{+, +-}.. ;<br>\n"
 " - <b>n</b> -- up to count;<br>\n"
 " - <b>useReset</b> -- use u[1] as reset counter signal;<br>\n"
 " - <b>cn</b> -- counter [0;n-1] (ro).<br>\n";

TClassInfo TCounter::class_info = {
  "TCounter", TCounter::create,
  &TMiso::class_info, helpstr, clpElem };


TCounter::TCounter( TDataSet* aparent )
        :TMiso( aparent )
{
  cn = type = useReset = flip = 0; u_old = 9e300; n = 2;
}

TCounter::~TCounter()
{
}

TDataSet* TCounter::create( TDataSet* apar )
{
  return new TCounter( apar );
}


const TClassInfo* TCounter::getClassInfo(void) const
{
  return &class_info;
}

const char *TCounter::getHelp(void) const
{
  return helpstr;
}


int TCounter::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  cn = flip = 0; u_old = 9e300;
  return 0;
}

double TCounter::f( double /* t */ )
{
  double v, du;
  int tick;
  du = *in_so[0] - u_old; u_old  = *in_so[0]; tick = 0;
  if( useReset && *in_so[1] > 0.1 ) {
    cn = flip = 0; 
  } else {
    if( du > 0.1 ) {
      cn++;
      if( cn >= n ) {
	cn = 0; tick = 1; flip = ! flip;
      };
    };
  };
  switch( type ) {
    case 0: v = flip; break; // level
    case 1: v = tick; break; // pulse+
    case 2: v = tick ? ( -1 + 2*flip ) : 0; break; // pulse+-
    case 3: v = cn; break; // count;
    default: v = 0;
  };
  return v;
}

int TCounter::registered = reg();

int TCounter::reg()
{
  return EFACT.registerElemType( &class_info  );
}


// end of tcounter.cpp

