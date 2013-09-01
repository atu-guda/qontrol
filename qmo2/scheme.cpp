/***************************************************************************
                          scheme.cpp
                             -------------------
    begin                : 2013.09.01
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

#include <ctime>
#include <cmath>
// unistd for usleep: TODO: replace with threads
#include <unistd.h>
#include <algorithm>
#include "miscfun.h"
#include "scheme.h"

using namespace std;

const char* Scheme::helpstr = "<H1>Scheme</H1>\n"
 "Hold all active elements";

STD_CLASSINFO(Scheme,clpSpecial | clpContainer);

CTOR(Scheme,TDataContainer)
{
  allowed_types = "TMiso"; 
  rtime = t = 0; tdt = t_full / nn; 
  m_sqrt2 = sqrt(2.0);
  m_sqrt1_2 = sqrt(0.5);
  one = 1.0;
  m_PI = M_PI;
  m_E = M_E;
  const int ELM_RES = 64;
  v_el.reserve( ELM_RES ); v_ord.reserve( ELM_RES );
}

Scheme::~Scheme()
{
}


int Scheme::startRun( int type )
{
  int rc;
  if( run_type >= 0 ) // in progress now
    return -1;
  if( type < 0 || type > 2 )
    type = 0;
  reset(); 
  run_type = type; 
  n1_eff = n2_eff = 1;
  if( run_type > 0 ) n1_eff = nl1;
  if( run_type > 1 ) n2_eff = nl2;
  n_tot = nn * n1_eff * n2_eff; i_tot = ii = il1 = il2 = 0;
  sgnt = int( time( 0 ) );
  prm2 = (double)prm2s; prm3 = (double)prm3s;
  rc = preRun( run_type, n1_eff, n2_eff );
  if( rc != 0 ) {
    end_loop = 1; 
    postRun();
    return rc;
  };
  t = rtime = 0;

  return 0;
}

int Scheme::nextSteps( int csteps )
{
  int i, rc;
  double prm0d { 0.1 }, prm1d {0.1}; // FIXME: read from parent, but not here
  if( csteps < 1 ) 
    csteps = 1;  

  for( i=0; i < csteps && i_tot < n_tot && end_loop == 0; i++, i_tot++ ) {
    prm0 = prm0s + il1 * prm0d; prm1 = prm1s + il2 * prm1d;
    if( ii == 0 ) {    // --------------------- start inner loop --

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

int Scheme::stopRun( int reason )
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
  
int Scheme::runOneLoop(void)
{
  unsigned long wait_ms;
  rtime = get_real_time() - start_time;
  if( use_sync ) {
     if( t > rtime ) {
       wait_ms = (unsigned long)( 1000000 * ( t - rtime ) );
       usleep( wait_ms );
       // return 1;
     };
  };  
  IterType itype = IterMid;

  if( ii == 0 ) {
    itype = IterFirst;
  } else if( ii == nn-1 ) {
    itype = IterLast;
  };

  int elnu = 0;
  for( TMiso* cur_el : v_el ) {
    if( end_loop )
      break;
     
     cur_el->fun( t, itype );  // <============ main action
     ++elnu;
  };  // end element loop;
  
  t += tdt; ii++;
  return 0;
}

int Scheme::preRun( int run_tp, int anx, int any )
{
  int rc;
  tdt = t_full / nn;
  
  state = stateRun;
  for( TMiso *ob : v_el ) {
    if( ob != 0 ) {
      rc = ob->preRun( run_tp, nn, anx, any, tdt );
      if( rc != 0 )
	return rc;
    };
  };
  return 0;
}

int Scheme::postRun()
{
  int cm = 0;
  for( TMiso *ob : v_el ) {
    if( ob != 0 ) {
      ob->postRun( end_loop );
      cm |= ob->getModified();
    };
  };
  if( modified == 0 || cm != 0 )
    modified |= 2;
  return 0;
}/*}}}1*/

int Scheme::reset()
{
  linkNames();

  state = stateGood; run_type = -1; sgnt = int( time(0) );
  return 0;
}

int Scheme::allStartLoop( int acnx, int acny )
{
  int rc;
  for( TMiso *ob : v_el ) {
    if( ob == 0 ) 
      continue;
    rc = ob->startLoop( acnx, acny );
    if( rc != 0 )
      return rc;
  };
  return 0;
}

void Scheme::allEndLoop()
{
  for( TMiso *ob : v_el ) {
    if( ob == nullptr ) 
      continue;
    ob->endLoop();
  };
}



int Scheme::ord2elnu( int aord ) const
{
  for( unsigned i=0; i<v_ord.size(); i++ ) {
    if( v_ord[i] == aord )
      return i;
  };	  
  return -1;
}

int Scheme::fback( int code, int /* aord */, const QString & /* tdescr */ )
{
  if( code ) {
    end_loop = code;
  };    
  return 0;
}

int Scheme::checkData( int n )
{
  if( n < 0 )
    linkNames();
  return TDataContainer::checkData( n );
}


int Scheme::xy2elnu( int avis_x, int avis_y )
{
  int i = 0, ox, oy;
  for( TMiso *ob : v_el ) {
    if( ob == 0 ) 
      continue;
    ox = oy = -1;
    ob->getData( "vis_x", &ox );
    ob->getData( "vis_y", &oy );
    if( ox == avis_x && oy == avis_y ) 
      return i;
    ++i;
  };	  
  return -1;
}

TMiso* Scheme::xy2Miso( int avis_x, int avis_y ) const
{
  int  ox, oy;
  for( TMiso *ob : v_el ) {
    if( !ob )
      continue;
    ox = oy = -1;
    ob->getData( "vis_x", &ox );
    ob->getData( "vis_y", &oy );
    if( ox == avis_x && oy == avis_y ) 
      return ob;
  }
  return nullptr;
}


TMiso* Scheme::getMiso( int elnu )
{
  if( elnu < 0 || elnu >= (int)v_el.size() )
    return nullptr;
  return v_el[elnu];
}


TOutArr* Scheme::getOutArr( const QString &oname )
{
  if( oname.isEmpty() )
    return nullptr;
  return par->getElemT<TOutArr*>( "outs." + oname );
}


TMiso* Scheme::insElem( const QString &cl_name, const QString &ob_name,
                     int aord, int avis_x, int avis_y )
{
  TMiso *ob;
  ob = qobject_cast<TMiso*>( add_obj( cl_name, ob_name ) );
  if( !ob )
    return nullptr;
  ob->setData( "ord", aord );
  ob->setData( "vis_x", avis_x );  
  ob->setData( "vis_y", avis_y );
  reset();
  modified |= 1;
  return ob;
}

int Scheme::delElem( const QString &ename )
{
  TMiso *ob = getElemT<TMiso*>( ename );
  if( !ob ) {
    DBG2q( "err: fail to find TMiso", ename );
    return 0;
  }
  int rc = del_obj( ename ); // TODO: or here??
  if( rc ) {
    reset();
    modified |= 1; // TODO: auto by reportStructChanged or ???
  }
  return rc;
}


int Scheme::newOrder( const QString &name, int new_ord )
{
  TMiso *ob = getElemT<TMiso*>( name );
  if( !ob )
    return -1;
  int k = ord2elnu( new_ord );
  if( k >= 0 )
    return -1;
  ob->setData( "ord", new_ord );
  ob->getData( "ord", &k );
  reset();
  modified |= 1;
  return ( k == new_ord ) ? 0 : -1;
}

int Scheme::moveElem( int elnu, int newx, int newy )
{
  int cx, cy;
  TMiso *ob, *ob1;
  if( newx < 0 || newy < 0 )
    return -1;
  ob = getMiso( elnu );
  if( ob == 0 ) return -1;
  for( unsigned i=0; i<v_el.size(); i++ ) {
    if( (int)i == elnu ) continue;
    cx = cy = -2;
    ob1 = getMiso( i );
    if( ob1 == 0 ) continue;
    ob1->getData( "vis_x", &cx ); ob1->getData( "vis_y", &cy );
    if( newx == cx && newy == cy ) return -1;
  };  
  ob->setData( "vis_x", newx );
  ob->setData( "vis_y", newy );
  reset();
  return 0;
}

int Scheme::linkNames(void)
{
  int oord;
  QString lname, pname, nname, oname;
  v_el.clear(); v_ord.clear();
  
  for( auto c : children() ) { // find elements of given types: TODO:
    HolderData *ho = qobject_cast<HolderData*>(c);
    if( !ho || !ho->isObject()  )
      continue;
    TDataSet *ds = qobject_cast<TDataSet*>(ho);
    if( !ds )
      continue;
    
    if( ds->isChildOf( "TMiso" )) {
      v_el.push_back( qobject_cast<TMiso*>(ds) );
      oord = -1;
      ds->getData( "ord", &oord );
      v_ord.push_back( oord );
      continue;
    };

  }

  sortOrd();

  return 0;
}

void Scheme::do_structChanged()
{
  TDataContainer::do_structChanged();
  linkNames();
}

void Scheme::sortOrd()
{
  int i, n, en, t;
  TMiso *tob;
  if( v_el.size() < 2 )
    return;
  en = 0; n = v_el.size();
  while( en == 0 ) { // simple bubble sort TODO: use STL sort
     en = 1;
     for( i=0; i<n-1; i++ ) {
        if( v_ord[i] > v_ord[i+1] ) {
          t = v_ord[i];   v_ord[i] = v_ord[i+1]; v_ord[i+1] = t;
          tob = v_el[i];  v_el[i] = v_el[i+1];   v_el[i+1] = tob;
          en = 0;
        };
     };
     n--;
  };
}

int Scheme::hintOrd() const
{
  auto pm = max_element( v_ord.begin(), v_ord.end() );
  int m = 0;
  if( pm != v_ord.end() )
    m = *pm;
  int m1 = ( (m+10) / 10 ) * 10;
  return m1;
}




DEFAULT_FUNCS_REG(Scheme)


// end of scheme.cpp

