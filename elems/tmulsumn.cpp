/***************************************************************************
        tmulsumn.h  - N-input multiplicator/summator + COG
                             -------------------
    begin                : 2015.11.15
    copyright            : (C) 2015-2015 by atu
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

#include "tmulsumn.h"

const char* TMulsumN::helpstr = "<h1>TMulsumN</h1>\n"
 "N-input multiplicator/summator + COG calculator: <br>\n"
 "p_i - input with subchannel 0,  <br>\n"
 "f_i - input with subchannel 1, <br>\n"
 "i   - channel number (i>=0), <br>\n"
 "spf = \\sum_i p_i \\cdot f_i ;<br>\n"
 "sp  = \\sum_i p_i ;<br>\n"
 "pge = spf / sp ;<br>\n"
 "ple,spfl,spl - local (near extremum variant);<br>\n"
 "ne - index of extremal point <br>\n"
 "pe - coordinate in extremal point <br>\n"
 "fe - function in extremal point <br>\n";

STD_CLASSINFO(TMulsumN,clpElem);

CTOR(TMulsumN,TMiso)
{
  allowed_types = "InputSimple,+SPECIAL";
}

double TMulsumN::f( double /* t */ )
{
  double r = 0;
  return r;
}

int TMulsumN::do_preRun( int /*run_tp*/, int /*an*/, int /*anx*/, int /*any*/, double /*atdt*/ )
{
  inputs.clear(); p_ins.clear(); f_ins.clear();
  np = 0;
  int max_ch = -1;
  for( auto i : TCHILD(InputSimple*) ) {
    int ch  = i->getDataD( "channel", -1 );
    int sch = i->getDataD( "subchannel", -1 );
    if( sch < 0 || sch > 1 ) {
      continue;
    }
    if( ch > max_ch ) {
      max_ch = ch;
    }
    inputs.append( i );
  }
  qWarning() << "inputs.size= " << inputs.size() << " max_ch" << max_ch << NWHE;

  return true;
}

DEFAULT_FUNCS_REG(TMulsumN)


// end of tsumn.cpp

