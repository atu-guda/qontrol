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

// --------------------- SubOutput --------------------------------- 

STD_CLASSINFO(SubOutput,clpSpecial);
const double SubOutput::fake_in {0};

CTOR(SubOutput,LinkedObj)
{
}

SubOutput::~SubOutput()
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

TSubScheme::~TSubScheme()
{
}


double TSubScheme::f( double t )
{
  double v = 0;
  for( auto in : inputs ) {
    in->readInput();
    // v = *in; // TMP: check
  }

  if( sch ) {
    sch->runOneLoop( t, iter_c );
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

int TSubScheme::do_preRun( int run_tp, int an,
                           int anx, int any, double atdt )
{
  inputs.clear();
  for( auto in : TCHILD(InputSimple*) ) {
    inputs.append( in );
  }

  subouts.clear();
  for( auto so : TCHILD(SubOutput*) ) {
    subouts.append( so );
  }

  if( !sch_proto ) {
    qWarning() << "Subscheme prototype is not available" << sch_name <<NWHE;
    return 0;
  }
  if( getObj( sch_ename ) ) {
    delObj( sch_ename, true ); // ignoreMod
  }

  sch = addObjT<Scheme>( sch_ename, true );
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

  for( auto so : subouts ) {
    so->set_link();
  }
  sch->handleStructChanged();

  return sch->preRun( run_tp, an, anx, any, atdt );
}

int TSubScheme::do_postRun( int /*good*/ )
{
  sch->postRun();
  delObj( sch_ename, true ); // tmp nodel
  return 1;
}

int TSubScheme::do_startLoop( int acnx, int acny )
{
  if( sch ) {
    return sch->allStartLoop( acnx, acny );
  }
  return 0;
}

int TSubScheme::do_endLoop()
{
  if( sch ) {
    sch->allEndLoop();
    return 1;
  }
  return 0;
}

void TSubScheme::do_post_set()
{
  sch_proto = getObjOfAncessorT<Scheme*>( sch_name );
  // if( !sch_proto ) {
  //   qWarning() << "Fail to find prototype cheme " << sch_name << NWHE; // Not here!
  // }
  return LinkedObj::do_post_set();
}

DEFAULT_FUNCS_REG(TSubScheme)


// end of tsubscheme.cpp


