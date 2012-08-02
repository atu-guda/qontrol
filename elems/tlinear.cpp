/***************************************************************************
                          tlinear.cpp  -  description
                             -------------------
    begin                : Tue Aug 1 2000
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

#include "tlinear.h"

const char* TLinear::helpstr = "<H1>TLinear</H1>\n"
 "Linear summator: <br>\n"
 "<b>x = a0*u0 + a1*u1 + a2*u2 + a3*u3 + b</b><br>\n"
 "Have 5 parameters: <b>a0 .. a3, b</b>, each can be changed at any time.";

TClassInfo TLinear::class_info = {
  CLASS_ID_TLinear, "TLinear", TLinear::create,
  &TMiso::class_info, helpstr, clpElem | clpPure };


TLinear::TLinear( TDataSet* aparent )
        :TMiso( aparent )
{
  a0 = 1; a1 = -1; a2 = a3 = 0; b = 0;
}

TLinear::~TLinear()
{
}

TDataSet* TLinear::create( TDataSet* apar )
{
  return new TLinear( apar );
}


const TClassInfo* TLinear::getClassInfo(void) const
{
  return &class_info;
}

const char *TLinear::getHelp(void) const
{
  return helpstr;
}

double TLinear::f( double /* t */ )
{
  return *in_so[0]*a0 + *in_so[1]*a1 + *in_so[2]*a2 + *in_so[3]*a3 + b;
}

int TLinear::registered = reg();

int TLinear::reg()
{
  return EFACT.registerElemType( &class_info  );
}


// end of tlinear.cpp

