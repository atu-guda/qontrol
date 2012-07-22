/***************************************************************************
                          trossler.cpp  -  description
                             -------------------
    begin                : Thu May 03 2012
    copyright            : (C) 2012 by atu
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

#include <time.h>
#include <math.h> 
#include "miscfun.h" 
#include "tmodel.h"
#include "trossler.h"

const char* TRossler::helpstr = "<H1>TRossler</H1>\n"
 "Simulator element for Rossler system <br>\n"
 " \\dot{x} = -y -x );   <br>\n"
 " \\dot{y} = x + a * y; <br>\n"
 " \\dot{z} = b + z * (x-c)<br>\n"
 " <br>\n"
 " Inputs 0-2: add to x, y, z <br>\n"
 " <br>\n"
 " Outputs <b>x</b> variable - other can be get by TIinputAny<br>\n"
 " <br>\n"
 "Have 6 parameters: 3 from system, <b>a, b ,c </b>, <br>"
 "and 3 inital state: <b> x_0, y_0, z_0 </b>, <br>"
 "First 3 may can be changed at any time.";

TClassInfo TRossler::class_info = {
  CLASS_ID_TRossler, "TRossler", TRossler::create,
  &TMiso::class_info, helpstr, clpElem };

static const int IPARM = efNoRunChange;
static const int XPARM = efNoDial | efNoSave ;


TRossler::TRossler( TDataSet* aparent )
        :TMiso( aparent )
{
  a = 0.25; b = 1.0; c = 5.58; 
  x_0 = 0.1; y_0 = 0.0; z_0 = 0.0; 
  x = y = z = 0;
}

TRossler::~TRossler()
{
}

TDataSet* TRossler::create( TDataSet* apar )
{
  return new TRossler( apar );
}

const TClassInfo* TRossler::getClassInfo(void) const
{
  return &class_info;
}

const char *TRossler::getHelp(void) const
{
  return helpstr;
}

double TRossler::f( double /*t*/ )
{
  double xn, yn, zn;
  xn = x + tdt * ( -y - z )      + *in_so[0];
  yn = y + tdt * ( x  + a*y )    + *in_so[1];
  zn = z + tdt * ( b + z*(x-c) ) + *in_so[2];
  x = xn; y = yn; z = zn;
  return x; 
}

int TRossler::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  x = x_0; y = y_0 ; z = z_0;
  return 0;
}

int TRossler::registered = reg();

int TRossler::reg()
{
  return ElemFactory::theFactory().registerElemType( &class_info  );
}


// end of trossler.cpp

