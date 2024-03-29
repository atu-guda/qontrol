/***************************************************************************
                          tsubscheme.cpp  - subscheme element
                             -------------------
    begin                : 2015.10.03
    copyright            : (C) 2015-2022 by atu
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

#include "autoact.h"
#include "tsubscheme.h"

// --------------------- SubOutput --------------------------------- 

STD_CLASSINFO(SubOutput,clpSpecial);
const double SubOutput::fake_in {0};

CTOR(SubOutput,LinkedObj)
{
}


bool SubOutput::set_link()
{
  p = &fake_in; src_obj = nullptr; linkType = LinkBad;
  if( source.cval().isEmpty() ) {
    linkType = LinkNone;
    return true; // really?
  }

  if( !par ) {
    return false;
  }
  Scheme *sch = par->getObjT<Scheme*>( "sch" );
  if( !sch ) {
    qWarning() << "scheme not found " << NWHE;
    return false;
  }

  int lt;
  const LinkedObj *srct = nullptr;
  const double *cp = sch->getDoublePtr( source, &lt, &srct, 0 );
  if( lt == LinkElm || lt == LinkSpec ) {
    p = cp;  src_obj = srct; linkType = lt;
  } else {
    qWarning() << "ptr not found for output " << source << NWHE;
    return false;
  }
  return true;
}

// QVariant SubOutput::dataObj( int col, int role ) const
// {
//   return LinkedObj::dataObj( col, role );
// }


const char* SubOutput::helpstr { "Gets value from given subscheme element and outputs to current scheme" };

DEFAULT_FUNCS_REG(SubOutput);


// --------------------- TSubScheme --------------------------------- 

const char* TSubScheme::helpstr = "<H1>TSubScheme</H1>\n"
 "Interface to subscheme <br>\n";

STD_CLASSINFO(TSubScheme,clpElem );

CTOR(TSubScheme,TMiso)
{
  allowed_types = "Scheme,InputSimple,SubOutput,+SPECIAL";
  setParm( "bad_types", "Scheme" );
}



double TSubScheme::f() noexcept
{
  double v = 0;

  if( sch ) {
    sch->runOneLoop( iter_c );
  } else {
    qWarning() << "No scheme?" << NWHE;
  }

  int i=0;
  for( auto so : subouts ) {
    so->getInput();
    if( i == 0 ) { // first output is out0;
      v = so->value();
    }
    ++i;
  }

  return v; // TMP
}

int TSubScheme::do_preRun()
{
  IGNORE_MOD_HERE;
  sch_proto = getObjOfAncessorT<Scheme*>( sch_name );
  if( !sch_proto ) {
    qWarning() << "Subscheme prototype is not available" << sch_name <<NWHE;
    return 0;
  }
  if( getObj( sch_ename ) ) {
    delObj( sch_ename );
  }

  sch = addObjT<Scheme>( sch_ename );
  if( ! sch ) {
    qWarning() << "Fail to create subscheme " << NWHE;
    return 0;
  }
  sch->addFlags( efNoSave );

  QString ss = sch_proto->toString();
  if( ! sch->fromString( ss ) ) {
    qWarning() << "Fail to copy prototype " << sch_name << NWHE;
    return 0;
  }

  // handleStructChanged();
  sch->handleStructChanged();


  subouts.clear();
  for( auto so : TCHILD(SubOutput*) ) {
    so->set_link();
    subouts.append( so );
  }

  return 1;
}

int TSubScheme::do_postRun( int /*good*/ )
{
  return 1;
  // if( sch ) {
  //   sch->postRun();
  //   // delObj( sch_ename, true ); // tmp: for debug: or forever?
  //   return 1;
  // }
  // return 0;
}



void TSubScheme::do_post_set()
{
  TMiso::do_post_set();
  sch_proto = getObjOfAncessorT<Scheme*>( sch_name );
  // if( !sch_proto ) {
  //   qWarning() << "Fail to find prototype cheme " << sch_name << NWHE; // Not here!
  // }
  return;
}

DEFAULT_FUNCS_REG(TSubScheme)


// end of tsubscheme.cpp


