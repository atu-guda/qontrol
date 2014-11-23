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
#include "tmodel.h"

using namespace std;

const char* Scheme::helpstr = "<H1>Scheme</H1>\n"
 "Hold all active elements";

STD_CLASSINFO(Scheme,clpSpecial | clpContainer);

CTOR(Scheme,TDataContainer)
{
  allowed_types = "TMiso";
  const int ELM_RES = 64;
  v_el.reserve( ELM_RES );
}

Scheme::~Scheme()
{
}

const double* Scheme::getSchemeDoublePtr( const QString &nm, ltype_t *lt,
        const TDataSet **src_ob, int lev) const
{
  // own names: elements (no local vars?)
  const double *p =  getDoublePtr( nm, lt, src_ob, lev );
  if( p ) {
    return p;
  }

  // then try model
  TModel *mod = getAncestorT<TModel>();
  if( !mod ) {
    DBG1( "No model detected" );
    return nullptr;
  }

  p =  mod->getDoublePtr( nm, lt, src_ob, lev );
  if( p ) {
    return p;
  }

  // then - active simulation
  Simulation *csim = mod->getActiveSimulation();
  if( csim ) {
    p =  csim->getDoublePtr( nm, lt, src_ob, lev );
  }

  return p;
}


int Scheme::runOneLoop( double t, IterType itype )
{
  for( auto cur_el : v_el ) {
    if( end_loop )
      break;

     cur_el->fun( t, itype );  // <============ main action
  };

  return 1;
}

int Scheme::preRun( int run_tp, int N, int anx, int any, double tdt )
{
  state = stateRun;
  for( auto ob : v_el ) {
    int rc = ob->preRun( run_tp, N, anx, any, tdt );
    if( !rc ) {
      DBGx( "warn: preRun failed for object \"%s\"", qP( ob->getFullName() ) );
      return 0;
    }
  };
  return 1;
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
  return 1;
}

int Scheme::reset()
{
  linkNames();

  state = stateGood; run_type = -1;
  return 0;
}

int Scheme::allStartLoop( int acnx, int acny )
{
  int rc;
  for( auto ob : v_el ) {
    rc = ob->startLoop( acnx, acny );
    if( !rc ) {
      return 0;
    }
  };
  return 1;
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
    if( !ob ) {
      continue;
    }
    ox = oy = -1;
    ob->getData( "vis_x", &ox );
    ob->getData( "vis_y", &oy );
    if( ox == avis_x && oy == avis_y ) {
      return ob;
    }
  }
  return nullptr;
}

QSize Scheme::getMaxXY() const
{
  int mx = 0, my = 0;
  for( auto ho : children() ) {
    if( !ho )
      continue;
    TMiso *ob = qobject_cast<TMiso*>( ho );
    if( !ob ) {
      continue;
    }
    int ox = 0, oy =0;
    ob->getData( "vis_x", &ox );
    ob->getData( "vis_y", &oy );
    if( ox > mx ) {
      mx = ox;
    }
    if( oy > my ) {
      my = oy;
    }
  }
  return QSize( mx, my );
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
    ob->getData( "ord", &mc );
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

