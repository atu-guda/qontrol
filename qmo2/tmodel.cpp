/***************************************************************************
                          tmodel.cpp  -  description
                             -------------------
    begin                : Tue Aug 1 2000
    copyright            : (C) 2000-2014 by atu
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
// unistd for usleep: TODO: replace with threads
#include <unistd.h>
#include <algorithm>
#include "miscfun.h"
#include "tmodel.h"

using namespace std;

const char* TModel::helpstr = "<H1>TModel</H1>\n"
 "Hold all active elements, output arrays and graph descriptions";

STD_CLASSINFO(TModel,clpSpecial | clpContainer);

CTOR(TModel,TDataContainer)
{
  allowed_types = "TMiso,TGraph,TOutArr,HolderValue,InputSimple,"
                  "ContScheme,ContSimul,Scheme,ContOut,ContGraph";
  rtime = t = 0; tdt = tt / nn;
  // TODO: remove after conversion
  const int ELM_RES = 64; const int OUT_RES = 32;
  v_el.reserve( ELM_RES );
  v_out.reserve( OUT_RES );
  v_outt.reserve( OUT_RES );
  v_graph.reserve( 16 );

  schems = addObj<ContScheme>( "schems" );
  sch_main = schems->addObj<Scheme>( "main" );
  outs = addObj<ContOut>( "outs" );
  plots = addObj<ContGraph>( "plots" );
  sims = addObj<ContSimul>( "sims" );
  sims->addObj<Simulation>( "sim0" );

}

TModel::~TModel()
{
}

// const double* TModel::getSchemeDoublePtr( const QString &nm, ltype_t *lt,
//         const TDataSet **src_ob, int lev) const
// {
//   return getDoublePtr( nm, lt, src_ob, lev );
// }




int TModel::reset()
{
  linkNames();
  state = stateGood; run_type = -1; sgnt = int( time(0) );
  return 0;
}


void TModel::allocOutArrs( int tp ) // TODO: common code
{
  int out_tp;
  if( tp < 0 || tp > 2 )
    return;
  for( QObject *o: outs->children() ) { // alloc output array
    TOutArr *arr = qobject_cast<TOutArr*>( o );
    if( !arr ) {
      continue;
    }
    out_tp = -1;
    arr->getData( "type", &out_tp );
    if( out_tp < 0 || out_tp > tp ) {
      continue;
    }
    switch( out_tp ) {
      case 0: arr->alloc( nn, 1 ); break;
      case 1: arr->alloc( nl1, 1 ); break;
      case 2: arr->alloc( nl1 * nl2, nl1 ); break;
      default: ; // don't allocate special arrays
    };
  };
}

void TModel::resetOutArrs( int level )
{
  for( QObject *o: outs->children() ) { // alloc output array
    TOutArr *arr = qobject_cast<TOutArr*>( o );
    if( !arr ) {
      continue;
    }
    arr->reset( level );
  };
}




int TModel::checkData( int n )
{
  if( n < 0 )
    linkNames();
  return TDataContainer::checkData( n );
}


TOutArr* TModel::getOutArr( const QString &oname )
{
  return outs->getElemT<TOutArr*>( oname );
}

TGraph* TModel::getGraph( const QString &name )
{
  return plots->getElemT<TGraph*>( name );
}


int TModel::insOut( const QString &outname, const QString &objname )
{
  TOutArr *arr = outs->addObj<TOutArr>( outname );
  if( !arr )
    return -1;
  arr->setData( "name", objname );

  QString lbl = objname;
  if( lbl.left(4) == "out_" )
    lbl.remove(0,4);
  if( lbl.left(1) == ":" )
    lbl.remove(0,1);

  arr->setData( "label", lbl );

  reset();
  modified |= 1;
  return 0;
}

int TModel::insGraph( const QString &gname )
{
  TGraph *gra = plots->addObj<TGraph>( gname );
  if( !gra )
    return -1;
  reset();
  modified |= 1;
  return 0;
}



int TModel::delOut( const QString &name )
{
  return outs->del_obj( name );
}

int TModel::delGraph( const QString &name )
{
  return plots->del_obj( name );
}

int TModel::newSimul( const QString &name )
{
  DBGx( "dbg: creating new simulation \"%s\"", qP(name) );
  Simulation *sim = sims->addObj<Simulation>( name );
  if( ! sim ) {
    DBGx( "ERR: fail to create simulation \"%s\"", qP(name) );
    return 0;
  }
  return 1;
}

int TModel::delSimul( const QString &name )
{
  return sims->del_obj( name );
}

QString TModel::getSimulName( int idx )
{
  DBGx( "dbg: requiest simulation name by idx %d", idx );
  Simulation* sim =  sims->getElemT<Simulation*>( idx );
  if( !sim ) {
    QString();
  }
  return sim->objectName();
}

// Simulation* TModel::getSimul( int idx )
// {
//   return sims->getElemT<Simulation*>( idx );
// }

Simulation* TModel::getSimul( const QString &name )
{
  return sims->getElemT<Simulation*>( name );
}

int TModel::linkNames()
{
  QString lname, pname, nname, oname;
  v_el.clear();
  v_out.clear(); v_outt.clear();
  v_graph.clear();

  for( auto c : children() ) { // find elements of given types: TODO:
    HolderData *ho = qobject_cast<HolderData*>(c);
    if( !ho || !ho->isObject()  )
      continue;
    TDataSet *ds = qobject_cast<TDataSet*>(ho);
    if( !ds )
      continue;

    if( ds->isChildOf( "TMiso" )) {
      v_el.push_back( qobject_cast<TMiso*>(ds) );
      continue;
    };
    if( ds->isChildOf( "TOutArr" )) {
      v_out.push_back( qobject_cast<TOutArr*>(ds) );
      continue;
    };
    if( ds->isChildOf( "TGraph" )) {
      v_graph.push_back( qobject_cast<TGraph*>(ds) );
      continue;
    };

  }

  // fill outs elnus and types
  int out_tp;
  for( TOutArr *arr : v_out ) {
    if( !arr ) { // FIXME
      v_outt.push_back(-1);
      continue;
    };
    oname = ""; out_tp = -1;
    arr->getData( "type", &out_tp );
    v_outt.push_back( out_tp );
  };
  return 0;
}

void TModel::do_structChanged()
{
  linkNames();
}




DEFAULT_FUNCS_REG(TModel)


// end of tmodel.cpp

