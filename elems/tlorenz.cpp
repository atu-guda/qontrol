/***************************************************************************
                          tlorenz.cpp  -  description
                             -------------------
    begin                : Fri Mar 09 2012
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

#include <ctime>
#include <cmath> 
#include "miscfun.h" 
#include "tlorenz.h"

using namespace std;

const char* TLorenz::helpstr = "<H1>TLorenz</H1>\n"
 "Simulator element for Lorenz system <br>\n"
 " \\dot{x} = \\sigma ( y-x );<br>\n"
 " \\dot{y} = x (r-z ) - y; <br>\n"
 " \\dot{z} = xy - bz.<br>\n"
 " <br>\n"
 " Inputs 0-2: add to x, y, z <br>\n"
 " <br>\n"
 " Outputs <b>x</b> variable - other can be get by TIinputAny<br>\n"
 " <br>\n"
 "Have 6 parameters: 3 from system, <b>sigma, r ,b </b>, <br>"
 "and 3 inital state: <b> x_0, y_0, z_0 </b>, <br>"
 "First 3 may can be changed at any time.";

TClassInfo TLorenz::class_info = {
  CLASS_ID_TLorenz, "TLorenz", TLorenz::create,
  &TMiso::class_info, helpstr, clpElem };


TLorenz::TLorenz( TDataSet* aparent )
        :TMiso( aparent )
{
  sigma = 10.0; r = 28.0; b = 8.0/3.0; // Well-known Lorenz params
  x_0 = 3.05; y_0 = 1.58; z_0 = 16.62; 
  x = y = z = 0;
}

TLorenz::~TLorenz()
{
}

TDataSet* TLorenz::create( TDataSet* apar )
{
  return new TLorenz( apar );
}


const TClassInfo* TLorenz::getClassInfo(void) const
{
  return &class_info;
}

const char *TLorenz::getHelp(void) const
{
  return helpstr;
}

double TLorenz::f( double /*t*/ )
{
  double xn, yn, zn;
  xn = x + tdt * ( sigma * (y-x) ) + *in_so[0];
  yn = y + tdt * ( x * (r-z) - y ) + *in_so[1];
  zn = z + tdt * ( x*y - b*z )     + *in_so[2];
  x = xn; y = yn; z = zn;
  return x; 
}


int TLorenz::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  x = x_0; y = y_0 ; z = z_0;
  return 0;
}

int TLorenz::registered = reg();

int TLorenz::reg()
{
  return EFACT.registerElemType( &class_info  );
}


// end of tlorenz.cpp

