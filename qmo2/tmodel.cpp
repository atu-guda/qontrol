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
                  "ContSchems,ContSimul,Scheme,ContOuts,ContPlots";
  rtime = t = 0; tdt = tt / nn;
  m_sqrt2 = sqrt(2.0);
  m_sqrt1_2 = sqrt(0.5);
  one = 1.0;
  m_PI = M_PI;
  m_E = M_E;
  const int ELM_RES = 64; const int OUT_RES = 32;
  v_el.reserve( ELM_RES );
  v_out.reserve( OUT_RES );
  v_outt.reserve( OUT_RES );
  v_graph.reserve( 16 );

  sims = addObj<ContSimul>( "sims" );
  if( sims ) {
    Simulation *sim0 = sims->addObj<Simulation>( "sim0" );
    if( ! sim0 ) {
      DBGx( "ERR: fail to create sim0" );
    }
  } else {
    DBGx( "ERR: fail to create sims" );
  }

}

TModel::~TModel()
{
}

const double* TModel::getSchemeDoublePtr( const QString &nm, ltype_t *lt,
        const TDataSet **src_ob, int lev) const
{
  return getDoublePtr( nm, lt, src_ob, lev );
}



int TModel::startRun( int type )
{
  int rc;
  if( run_type >= 0 ) // in progress now
    return -1;
  if( type < 0 || type > 2 )
    type = 0;
  reset();
  run_type = type;

  n1_eff = n2_eff = 1;
  if( run_type > 0 ) 
    n1_eff = nl1;
  if( run_type > 1 ) 
    n2_eff = nl2;

  n_tot = nn * n1_eff * n2_eff; i_tot = ii = il1 = il2 = 0;
  sgnt = int( time( 0 ) );
  prm2 = (double)prm2s; prm3 = (double)prm3s;

  allocOutArrs( run_type );
  rc = preRun( run_type, n1_eff, n2_eff );
  if( rc != 0 ) {
    end_loop = 1;
    postRun();
    return rc;
  };
  t = rtime = 0;

  return 0;
}

int TModel::nextSteps( int csteps )
{
  int i, rc;
  if( csteps < 1 )
    csteps = 1;

  for( i=0; i < csteps && i_tot < n_tot && end_loop == 0; i++, i_tot++ ) {
    prm0 = prm0s + il1 * prm0d;
    prm1 = prm1s + il2 * prm1d;
    if( ii == 0 ) {    // --------------------- start inner loop --
      if( il1 == 0 ) { // start prm0 loop
        resetOutArrs( 1 );
      };

      resetOutArrs( 0 );
      start_time = get_real_time(); rtime = t = 0;
      // set start param
      allStartLoop( il1, il2 );
    };// end start inner loop

    rc = runOneLoop();
    if( rc == 1 )
      return 0;

    if( ii >= nn ) {
      allEndLoop();
      ii = 0; il1++;
    };
    if( il1 >= nl1 ) {
      il1 = 0; il2++;
    };
  };
  if( i_tot >= n_tot )
    stopRun(0);
  return 0;
}

int TModel::stopRun( int reason )
{
  if( end_loop || reason ) {
    reset();
    state = stateGood;
  } else {
    postRun();
    state = stateDone;
  };
  return 0;
}

int TModel::runOneLoop()
{
  int out_level;
  unsigned long wait_ms;
  rtime = get_real_time() - start_time;
  if( use_sync ) {
     if( t > rtime ) {
       wait_ms = (unsigned long)( 1000000 * ( t - rtime ) );
       usleep( wait_ms ); // ------------------- TODO: redesign ------
       // return 1;
     };
  };
  IterType itype = IterMid;
  out_level = 0;
  if( ii == 0 ) {
    itype = IterFirst;
  } else if( ii == nn-1 ) {
    itype = IterLast;
    out_level = run_type;
  };

  for( TMiso* cur_el : v_el ) {
    if( end_loop )
      break;

     cur_el->fun( t, itype );  // <============ main action
  };  // end element loop;

  for( TOutArr* arr : v_out ) {
    arr->take_val( out_level );
  };
  t += tdt; ii++;
  return 0;
}

int TModel::preRun( int run_tp, int anx, int any )
{
  int rc;
  tdt = tt / nn;

  state = stateRun;
  for( TMiso *ob : v_el ) {
    if( ob ) {
      rc = ob->preRun( run_tp, nn, anx, any, tdt );
      if( rc != 0 )
        return rc;
    };
  };
  return 0;
}

int TModel::postRun()
{
  int cm = 0;
  for( TMiso *ob : v_el ) {
    if( ob ) {
      ob->postRun( end_loop );
      cm |= ob->getModified();
    };
  };
  if( modified == 0 || cm != 0 )
    modified |= 2;
  return 0;
}

int TModel::reset()
{
  linkNames();
  for( TOutArr *arr : v_out ) {
    if( ! arr )
      continue; // never be!
    arr->free();
  };
  state = stateGood; run_type = -1; sgnt = int( time(0) );
  return 0;
}

int TModel::allStartLoop( int acnx, int acny )
{
  int rc;
  for( TMiso *ob : v_el ) {
    if( ! ob )
      continue;
    rc = ob->startLoop( acnx, acny );
    if( rc != 0 )
      return rc;
  };
  return 0;
}

void TModel::allEndLoop()
{
  for( TMiso *ob : v_el ) {
    if( ! ob )
      continue;
    ob->endLoop();
  };
}

void TModel::allocOutArrs( int tp ) // TODO: common code
{
  int out_tp;
  if( tp < 0 || tp > 2 )
    return;
  for( TOutArr *arr: v_out ) { // alloc output array
    if( !arr )
      continue;
    out_tp = -1;
    arr->getData( "type", &out_tp );
    if( out_tp < 0 || out_tp > tp )
      continue;
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
  for( TOutArr *arr : v_out ) {
    if( !arr )
      continue;
    arr->reset( level );
  };
}





int TModel::fback( int code, int /* aord */, const QString & /* tdescr */ )
{
  if( code ) {
    end_loop = code;
  };
  return 0;
}

int TModel::checkData( int n )
{
  if( n < 0 )
    linkNames();
  return TDataContainer::checkData( n );
}


int TModel::xy2elnu( int avis_x, int avis_y ) const // TODO: todel ?
{
  int i = 0;
  for( const TMiso *ob : v_el ) {
    if( !ob )
      continue;
    if( ob->isAtCoord( avis_x, avis_y ) ) {
      return i;
    }
    ++i;
  };
  return -1;
}

TMiso* TModel::xy2Miso( int avis_x, int avis_y ) const
{
  for( TMiso *ob : v_el ) {
    if( !ob )
      continue;
    if( ob->isAtCoord( avis_x, avis_y ) ) {
      return ob;
    }
  }
  return nullptr;
}

int TModel::getNMiso() const
{
  return v_el.size();
}

TMiso* TModel::getMiso( int elnu )
{
  if( elnu < 0 || elnu >= (int)v_el.size() )
    return nullptr;
  return v_el[elnu];
}

int TModel::getNOutArr() const
{
  return v_out.size();
}

TOutArr* TModel::getOutArr( int out_nu )
{
  if( out_nu < 0 || out_nu >= (int)v_out.size() )
    return nullptr;
  return v_out[out_nu];
}

TOutArr* TModel::getOutArr( const QString &oname )
{
  if( oname.isEmpty() )
    return nullptr;
  TOutArr *arr = getElemT<TOutArr*>( oname );
  if( !arr ) {
    DBG2q( "warn: fail to find TOutArr", oname );
  }
  return arr;
}

int TModel::getNGraph() const
{
  return v_graph.size();
}

TGraph* TModel::getGraph( int gra_nu )
{
  if( gra_nu < 0 || gra_nu >= (int)v_graph.size() )
    return nullptr;
  return v_graph[gra_nu];
}

TMiso* TModel::insElem( const QString &cl_name, const QString &ob_name,
                     int aord, int avis_x, int avis_y )
{
  // no addObj: downcast
  TMiso *ob = qobject_cast<TMiso*>( add_obj( cl_name, ob_name ) );
  if( !ob ) // FIXME: leak?
    return nullptr;
  ob->setData( "ord", aord );
  ob->setData( "vis_x", avis_x );
  ob->setData( "vis_y", avis_y );
  reset();
  modified |= 1;
  return ob;
}

int TModel::insOut( const QString &outname, const QString &objname )
{
  TOutArr *arr = addObj<TOutArr>( outname );
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
  TGraph *gra = addObj<TGraph>( gname );
  if( !gra )
    return -1;
  reset();
  modified |= 1;
  return 0;
}

bool TModel::insElem( const QString &tp, const QString &nm, const QString &params )
{
  int order = hintOrd();
  TMiso *ob = insElem( tp, nm, order, 0, 0 );
  if( !ob )
    return false;
  ob->setParams( params );
  return true;
}

bool TModel::delElem( const QString &nm )
{
  TMiso *ob = getElemT<TMiso*>( nm ); // chesk for TMiso, TODO: remove in scheme
  if( !ob ) {
    DBG2q( "err: fail to find TMiso", nm );
    return 0;
  }
  bool rc = del_obj( nm );
  if( rc ) {
    reset();
    modified |= 1;
  }
  return rc;
}

bool TModel::setElem( const QString &nm, const QString &params )
{
  TMiso *ob = getElemT<TMiso*>( nm ); // chesk for TMiso, TODO: remove in scheme
  if( !ob ) {
    DBG2q( "err: fail to find TMiso", nm );
    return false;
  }
  return ob->setParams( params );
}


int TModel::delOut( int out_nu )
{
  int k;
  if( out_nu < 0 || out_nu >= (int)v_out.size() )
    return -1;
  k = del_obj( v_out[out_nu]->objectName() );
  reset();
  modified |= 1;
  return k;
}

int TModel::delGraph( int gr_nu )
{
  int k;
  if( gr_nu < 0 || gr_nu >= (int)v_graph.size() )
    return -1;
  k = del_obj( v_graph[gr_nu]->objectName() );
  reset();
  modified |= 1;
  return k;
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
  DBGx( "dbg: removing simulation \"%s\"", qP(name) );
  return 0; // TODO:
}

QString TModel::getSimulName( int idx )
{
  DBGx( "dbg: requiest simulation name by idx %d", idx );
  return QString(); // TODO:
}

Simulation* TModel::getSimul( int idx )
{
  DBGx( "dbg: requiest simulation ptr by idx %d", idx );
  return nullptr; // TODO:
}

Simulation* TModel::getSimul( const QString &name )
{
  DBGx( "dbg: requiest simulation ptr by name \"%s\"", qP(name) );
  return nullptr; // TODO:
}

int TModel::newOrder( const QString &name, int new_ord )
{
  TMiso *ob = getElemT<TMiso*>( name );
  if( !ob )
    return -1;

  auto p = find_if( v_el.begin(), v_el.end(),
      [new_ord](const TMiso *el) { return new_ord == el->getOrder(); } );
  if( p != v_el.end() ) {
    DBGx( "warn: order %d have element \"%s\"",
          new_ord, qP((*p)->getFullName()) );
    return -1;
  }

  ob->setData( "ord", new_ord );
  int real_ord;
  ob->getData( "ord", &real_ord );
  reset();
  modified |= 1;
  sortOrd();
  return ( real_ord == new_ord ) ? 0 : -1;
}

int TModel::moveElem( int elnu, int newx, int newy )
{
  TMiso *ob, *ob1;
  if( newx < 0 || newy < 0 )
    return -1;
  ob = getMiso( elnu );
  if( ob == 0 ) return -1;
  for( unsigned i=0; i<v_el.size(); i++ ) {
    if( (int)i == elnu ) 
      continue;
    ob1 = getMiso( i );
    if( ! ob1 )
      continue;
    if( ob1->isAtCoord( newx, newy ) )
      return -1;
  };
  ob->setData( "vis_x", newx );
  ob->setData( "vis_y", newy );
  reset();
  return 0;
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

  sortOrd();

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

void TModel::sortOrd()
{
  sort( v_el.begin(), v_el.end(),
      []( const TMiso *a, const TMiso*b) { return *a < *b; } );
}

int TModel::hintOrd() const
{
  int m = 0;
  for_each( v_el.begin(), v_el.end(),
      [&m](TMiso *el) { m = max(m,el->getOrder()); } );
  int m1 = ( (m+10) / 10 ) * 10;
  return m1;
}



DEFAULT_FUNCS_REG(TModel)


// end of tmodel.cpp

