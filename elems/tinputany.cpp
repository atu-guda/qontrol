/***************************************************************************
                          tinputany.cpp  -  description
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
#include "tinputany.h"

const char* TInputAny::helpstr = "<H1>TInputAny</H1>\n"
 "Gets signal from given source: <br>\n"
 "1-st form: #12 - get models var #12 <br>\n"
 "2-nd form: name relative to model: source.u <br>\n"
 "Have 1 parameter: <b>name</b>.";

STD_CLASSINFO(TInputAny,clpElem);

CTOR(TInputAny,TMiso),
	fake_so(0), so( &fake_so )
{
}


int TInputAny::do_preRun( int /*run_tp*/, int /*an*/, 
                          int /*anx*/, int /*any*/, double /*adt*/ )
{
  so = &fake_so;
  if( name.cval().isEmpty() ) { // empty name is good: fake source
    return 0;
  }

  if( name.cval()[0] == '#' ) { // access to model vars by number
    QString nname = name;
    nname.remove( 0, 1 );
    int ne = nname.toInt();
    so = &( (*model->getVars())[ne] );
    return 0;
  };

  so = par->getDoublePtr( name ); 
  if( !so ) {
    so = &fake_so; 
    qDebug( "WARN: TInputAny::preRun: fail to find source name \"%s\"",
             qPrintable(name) );
  }
  return 0;
}

double TInputAny::f( double /* t */ )
{
  return *so;
}

DEFAULT_FUNCS_REG(TInputAny)


// end of tinputany.cpp


