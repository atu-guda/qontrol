/***************************************************************************
                          tswitch.cpp  -  description
                             -------------------
    begin                : Sun Nov 19 2000
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

#include "tswitch.h"

const char* TSwitch::helpstr = "<H1>TSwitch</H1>\n"
 "<p>Connects output to one of inputs, accrding to values of u[3]: <br>\n"
 " if( u[3] &lt; level1 ) out = u[0]; <br>\n"
 " if( u[3] &gt; level2 ) out = u[2]; <br>\n"
 " else out = u[1]. <br>\n"
 "Parameters:<br>\n"
 " - <b>level1</b> -- level of in_3 to switch in_0->in_1;<br>\n"
 " - <b>level2</b> -- level of in_3 to switch in_1->in_2.<br>\n"
 "Each parameter can be changed at any time.</p>";

STD_CLASSINFO(TSwitch,clpElem);

CTOR(TSwitch,TMiso)
{
}


double TSwitch::f( double /* t */ )
{
  double v;
  v = ( in_3 < level1 ) ? 
    in_0 
    : ( ( in_3>level2 ) ? in_2 : in_1 );
  return v;
}

DEFAULT_FUNCS_REG(TSwitch)


// end of tswitch.cpp

