/***************************************************************************
                          toutputany.cpp  -  description
                             -------------------
    begin                : Sat Sep 2 2000
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

#include <string.h>
#include <stdlib.h>
#include "miscfun.h"
#include "tmodel.h"
#include "toutputany.h"

const char* TOutputAny::helpstr = "<H1>TOutputAny</H1>\n"
 "<h2>Obsoleted element, use only out out to model vars (obsoleted too)</h2>\n"
 "Puts signal to given target: <br>\n"
 "1-st form: #12 - put models var #12 <br>\n"
 "2-nd form: name relative to model: source.u <br>\n"
 "Have 1 parameter: <b>name</b>.";

TClassInfo TOutputAny::class_info = {
  "TOutputAny", TOutputAny::create,
  &TMiso::class_info, helpstr, clpElem };


TOutputAny::TOutputAny( TDataSet* aparent )
        :TMiso( aparent )
{
  type = -1; 
  useEnable = 0; ne = -1; pel = 0;
}

TOutputAny::~TOutputAny()
{
}


// TODO: fix! NO: remove this object!
int TOutputAny::do_preRun( int /*run_tp*/, int /*an*/, 
                           int /*anx*/, int /*any*/, double /*adt*/ )
{
  int l;
  lastname = "";
  type = -1; ne = -1; pel = 0;
  l = name.size();
  if( l < 2 ) return 0; // smallest: '#0'
  if( name[0] == '#' ) { // access to model vars by number
    type = 1; 
    ne = atoi( name.toLocal8Bit().constData() + 1 );
    return 0;
  };
  // all output dropped
  return 0;
}

double TOutputAny::f( double /* t */ )
{
  if( useEnable && *in_so[1] < 0.1 )
    return *in_so[0];
  switch( type ) {
    // case 0: pel->setDataID( ne, *in_so[0], 1 ); break; TODO: drop at all
    case 1: model->setVar( ne, *in_so[0] ); break;
    default: ;
  };
  return *in_so[0];
}

DEFAULT_FUNCS_REG(TOutputAny)


// end of toutputany.cpp


