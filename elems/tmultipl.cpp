/***************************************************************************
                          tmultipl.cpp  -  description
                             -------------------
    begin                : Fri Aug 25 2000
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

#include "tmultipl.h"

const char* TMultipl::helpstr = "<H1>TMultipl</H1>\n"
 "Simple multiplicator: <br>\n"
 "it multimplies all enabled inputs and parametr <b>a</b>\n"
 "Have 5 parameters: <b>on0 .. on3, a</b>, each can be changed at any time.";

TClassInfo TMultipl::class_info = {
  "TMultipl", TMultipl::create,
  &TMiso::class_info, helpstr, clpElem | clpPure };


TMultipl::TMultipl( TDataSet* aparent )
        :TMiso( aparent )
{
  on0 = on1 = 1; on2 = on3 = 0; a = 1;
}

TMultipl::~TMultipl()
{
}

TDataSet* TMultipl::create( TDataSet* apar )
{
  return new TMultipl( apar );
}


const TClassInfo* TMultipl::getClassInfo(void) const
{
  return &class_info;
}

const char *TMultipl::getHelp(void) const
{
  return helpstr;
}

double TMultipl::f( double /* t */ )
{
  double v = a; 
  if( on0 )
    v *= *in_so[0];
  if( on1 )
    v *= *in_so[1];
  if( on2 )
    v *= *in_so[2];
  if( on3 )
    v *= *in_so[3];
  return v; 
}

int TMultipl::registered = reg();

int TMultipl::reg()
{
  return EFACT.registerElemType( &class_info  );
}

// end of tmultipl.cpp


