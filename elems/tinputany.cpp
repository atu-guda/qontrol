/***************************************************************************
                          tinputany.cpp  -  description
                             -------------------
    begin                : Sat Sep 2 2000
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

TClassInfo TInputAny::class_info = {
  CLASS_ID_TInputAny, "TInputAny", TInputAny::create,
  &TMiso::class_info, helpstr, clpElem };


TInputAny::TInputAny( TDataSet* aparent )
        :TMiso( aparent ),
	fake_so(0), so( &fake_so )
{
}

TInputAny::~TInputAny()
{
}

TDataSet* TInputAny::create( TDataSet* apar )
{
  return new TInputAny( apar );
}


const TClassInfo* TInputAny::getClassInfo(void) const
{
  return &class_info;
}

const char *TInputAny::getHelp(void) const
{
  return helpstr;
}

int TInputAny::do_preRun( int /*run_tp*/, int /*an*/, 
                          int /*anx*/, int /*any*/, double /*adt*/ )
{
  so = &fake_so;
  if( name.isEmpty() ) { // empty name is good: fake source
    return 0;
  }

  if( name[0] == '#' ) { // access to model vars by number
    QString nname = name;
    nname.remove( 0, 1 );
    int ne = nname.toInt();
    so = model->getVars() + ne;
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

int TInputAny::registered = reg();

int TInputAny::reg()
{
  return EFACT.registerElemType( &class_info  );
}


// end of tinputany.cpp


