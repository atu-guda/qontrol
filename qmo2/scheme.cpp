/***************************************************************************
                          scheme.cpp
                             -------------------
    begin                : 2013.09.01
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
#include "scheme.h"

using namespace std;

const char* Scheme::helpstr = "<H1>Scheme</H1>\n"
 "Hold all active elements";

STD_CLASSINFO(Scheme,clpSpecial | clpContainer);

CTOR(Scheme,TDataContainer)
{
  allowed_types = "TMiso,double,HolderDouble,HolderInt";
  rtime = t = 0;
  m_sqrt2 = sqrt(2.0);
  m_sqrt1_2 = sqrt(0.5);
  one = 1.0;
  m_PI = M_PI;
  m_E = M_E;
  const int ELM_RES = 64;
  v_el.reserve( ELM_RES );
}

Scheme::~Scheme()
{
}


int Scheme::startRun( Simulation *a_sim )
{
  if( ! a_sim ) {
    return -1;
  }
  sim = a_sim;

  if( run_type >= 0 ) { // in progress now
    return -1;
  }

  // copy of simulation data
  sim->getData( "T", &T );
  sim->getData( "N", &N );  sim->getData( "N1", &N1 );  sim->getData( "N2", &N2 );
  sim->getData( "syncRT", &syncRT );
  sim->getData( "prm0s", &prm0s );
  sim->getData( "prm1s", &prm0s );
  sim->getData( "prm2s", &prm0s );
  sim->getData( "prm3s", &prm0s );
  sim->getData( "prm0d", &prm0d );
  sim->getData( "prm1d", &prm0s );

  int type = Simulation::runSingle;
  sim->getData( "type", &type );

  if( type     !=  Simulation::runSingle
      &&  type !=  Simulation::runLoop
      &&  type !=  Simulation::run2DLoop ) {
    type = Simulation::runSingle;
  }

  reset();

  run_type = type;
  n1_eff = n2_eff = 1;
  if( run_type >  Simulation::runSingle )
    n1_eff = N1;
  if( run_type > Simulation::runLoop )
    n2_eff = N2;

  n_tot = N * n1_eff * n2_eff;
  i_tot = ii = il1 = il2 = 0;
  sgnt = int( time( 0 ) );
  prm2 = (double)prm2s; prm3 = (double)prm3s;

  // TODO: fill v_el

  int rc = preRun( run_type, n1_eff, n2_eff );
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
  if( ! sim ) {
    return 0;
  }

  if( csteps < 1 )
    csteps = 1;

  for( i=0; i < csteps && i_tot < n_tot && end_loop == 0; i++, i_tot++ ) {
    prm0 = prm0s + il1 * prm0d;
    prm1 = prm1s + il2 * prm1d;
    if( ii == 0 ) {    // --------------------- start inner loop --

      start_time = get_real_time(); rtime = t = 0;
      // set start param
      allStartLoop( il1, il2 );
    };// end start inner loop

    rc = runOneLoop();
    if( rc == 1 )
      return 0;

    if( ii >= N ) {
      allEndLoop();
      ii = 0; il1++;
    };
    if( il1 >= N1 ) {
      il1 = 0; il2++;
    };
  };

  if( i_tot >= n_tot ) {
    stopRun(0);
  }

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
  sim = nullptr;
  return 0;
}

int Scheme::runOneLoop(void)
{
  unsigned long wait_ms;
  rtime = get_real_time() - start_time;
  if( syncRT ) {
     if( t > rtime ) {
       wait_ms = (unsigned long)( 1000000 * ( t - rtime ) );
       usleep( wait_ms );
       // return 1;
     };
  };
  IterType itype = IterMid;

  if( ii == 0 ) {
    itype = IterFirst;
  } else if( ii == N-1 ) {
    itype = IterLast;
  };

  int elnu = 0;
  for( auto cur_el : v_el ) {
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
  // tdt = t_full / nn;

  state = stateRun;
  for( auto ob : v_el ) {
    rc = ob->preRun( run_tp, N, anx, any, tdt );
    if( rc != 0 ) {
      return rc;
    }
  };
  return 0;
}

int Scheme::postRun()
{
  int cm = 0;
  for( auto ob : v_el ) {
    ob->postRun( end_loop );
    cm |= ob->getModified();
  };

  if( modified == 0 || cm != 0 )
    modified |= 2;
  return 0;
}

int Scheme::reset()
{
  linkNames();

  state = stateGood; run_type = -1; sgnt = int( time(0) );
  return 0;
}

int Scheme::allStartLoop( int acnx, int acny )
{
  int rc;
  for( auto ob : v_el ) {
    rc = ob->startLoop( acnx, acny );
    if( rc != 0 ) {
      return rc;
    }
  };
  return 0;
}

void Scheme::allEndLoop()
{
  for( auto ob : v_el ) {
    ob->endLoop();
  };
}



TMiso* Scheme::ord2Miso( int aord ) const
{
  for( auto c : children() ) {
    TMiso *ob = qobject_cast<TMiso*>(c);
    if( !ob ) {
      continue;
    }
    int oord = -1;
    ob->getData( "ord", &oord );
    if( aord == oord ) {
      return ob;
    }
  };

  return nullptr;
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
  if( n < 0 ) {
    linkNames();
  }
  return TDataContainer::checkData( n );
}



TMiso* Scheme::xy2Miso( int avis_x, int avis_y ) const
{
  int  ox, oy;
  for( auto ho : children() ) {
    if( !ho )
      continue;
    TMiso *ob = qobject_cast<TMiso*>( ho );
    if( !ob )
      continue;
    ox = oy = -1;
    ob->getData( "vis_x", &ox );
    ob->getData( "vis_y", &oy );
    if( ox == avis_x && oy == avis_y ) {
      return ob;
    }
  }
  return nullptr;
}



TMiso* Scheme::insElem( const QString &cl_name, const QString &ob_name,
                     int aord, int avis_x, int avis_y )
{
  // not add_obj, downcast
  TMiso *ob = qobject_cast<TMiso*>( add_obj( cl_name, ob_name ) ); 
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
  if( ord2Miso( new_ord ) != nullptr ) {
    return -1;
  }
  ob->setData( "ord", new_ord );
  int k;
  ob->getData( "ord", &k );

  reset();
  modified |= 1;
  return ( k == new_ord ) ? 0 : -1;
}

int Scheme::moveElem( const QString &nm, int newx, int newy )
{
  TMiso *ob, *ob1;
  if( newx < 0 || newy < 0 )
    return -1;
  ob = getElemT<TMiso*>( nm );
  if( !ob )
    return -1;

  ob1 = xy2Miso( newx, newy );
  if( ob1 ) {
    return -1; // busy
  }

  ob->setData( "vis_x", newx );
  ob->setData( "vis_y", newy );
  reset();
  return 0;
}

int Scheme::linkNames(void)
{
  QString lname, pname, nname, oname;
  v_el.clear();

  for( auto c : children() ) {
    TMiso *ob = qobject_cast<TMiso*>(c);
    if( !ob ) {
      continue;
    }
    v_el.push_back( ob );
  };

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
  std::sort( v_el.begin(), v_el.end(),
      []( const TMiso *a, const TMiso*b) { return *a < *b; } );
}

int Scheme::hintOrd() const
{
  int m = 0, mc;
  for( auto c : children() ) {
    TMiso *ob = qobject_cast<TMiso*>(c);
    if( !ob ) {
      continue;
    }
    getData( "ord", &mc );
    if( mc > m ) {
      m = mc;
    }
  };
  int m1 = ( (m+10) / 10 ) * 10;
  return m1;
}




DEFAULT_FUNCS_REG(Scheme)


// ------------------ container of Schemes --------------


const char* ContScheme::helpstr = "<H1>ContScheme</H1>\n"
 "Container of simulations";

STD_CLASSINFO(ContScheme,clpSpecial | clpContainer);

CTOR(ContScheme,TDataContainer)
{
  allowed_types = "Scheme";
}

ContScheme::~ContScheme()
{
}

DEFAULT_FUNCS_REG(ContScheme)



// end of scheme.cpp

