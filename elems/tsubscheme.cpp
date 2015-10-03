/***************************************************************************
                          tsubscheme.cpp  - subscheme element
                             -------------------
    begin                : 2015.10.03
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

#include "tsubscheme.h"

const char* TSubScheme::helpstr = "<H1>TSubScheme</H1>\n"
 "Interface to subscheme <br>\n";

STD_CLASSINFO(TSubScheme,clpElem );

CTOR(TSubScheme,TMiso)
{
}

TSubScheme::~TSubScheme()
{
  delete[] sch; sch = nullptr;
}


double TSubScheme::f( double t )
{
  return t; // TMP
}

int TSubScheme::do_preRun( int /*run_tp*/, int /*an*/,
                       int /*anx*/, int /*any*/, double /*atdt*/ )
{
  if( sch ) {
    delete[] sch;
  }
  sch = new Scheme( "sch", this );
  return 1;
}

int TSubScheme::do_postRun( int /*good*/ )
{
  delete[] sch; sch = nullptr;
  return 1;
}

int TSubScheme::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  // sch->startLoop
  return 1;
}


DEFAULT_FUNCS_REG(TSubScheme)


// end of tsubscheme.cpp


