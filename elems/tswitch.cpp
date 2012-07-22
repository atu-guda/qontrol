/***************************************************************************
                          tswitch.cpp  -  description
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

#include "tswitch.h"

const char* TSwitch::helpstr = "<H1>TSwitch</H1>\n"
 "Connects output to one of inputs, accrding to values of u[3]: <br>\n"
 " if( u[3] &lt; level1 ) out = u[0]; <br>\n"
 " if( u[3] &gt; level2 ) out = u[2]; <br>\n"
 " else out = u[1]. <br>\n"
 "Parameters:<br>\n"
 " - <b>level1</b> -- level of u[3] to switch u[0]->u[1];<br>\n"
 " - <b>level2</b> -- level of u[3] to switch u[1]->u[2].<br>\n"
 "Each parameter can be changed at any time.";

TClassInfo TSwitch::class_info = {
  CLASS_ID_TSwitch, "TSwitch", TSwitch::create,
  &TMiso::class_info, helpstr, clpElem | clpPure };


TSwitch::TSwitch( TDataSet* aparent )
        :TMiso( aparent )
{
  level1 = 1; level2 = 2;
}

TSwitch::~TSwitch()
{
}

TDataSet* TSwitch::create( TDataSet* apar )
{
  return new TSwitch( apar );
}

const TClassInfo* TSwitch::getClassInfo(void) const
{
  return &class_info;
}

const char *TSwitch::getHelp(void) const
{
  return helpstr;
}

double TSwitch::f( double /* t */ )
{
  double v;
  v = ( *in_so[3] < level1 ) ? *in_so[0] : ( ( *in_so[3]>level2 ) ? *in_so[2] : *in_so[1] );
  return v;
}

int TSwitch::registered = reg();

int TSwitch::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


// end of tswitch.cpp

