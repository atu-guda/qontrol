/***************************************************************************
                          scheme.cpp
                             -------------------
    begin                : 2013.09.01
    copyright            : (C) 2000-2015 by atu
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
#include <QStandardItem>

#include "miscfun.h"
#include "scheme.h"
#include "tmodel.h"

using namespace std;

const char* Scheme::helpstr = "<H1>Scheme</H1>\n"
 "Hold all active elements";

STD_CLASSINFO(Scheme,clpSpecial | clpContainer);

CTOR(Scheme,TDataSet)
{
  allowed_types = "TMiso";
  const int ELM_RES = 64;
  v_el.reserve( ELM_RES );
}

Scheme::~Scheme()
{
}

// must be similar to fillComplModelForInputs
const double* Scheme::getSchemeDoublePtr( const QString &nm, ltype_t *lt,
        const TDataSet **src_ob, int lev) const
{
  static ltype_t clt;
  static const double fake_src = 0.123456; // ptr here for non- main_s schemes uplinks
  ltype_t *plt = lt ? lt : &clt;

  if( nm.startsWith( '^' ) ) { // direct to parent
    QString nnm = nm.mid( 1 );
    TDataSet *pds = qobject_cast<TDataSet*>( par );
    if( !pds ) {
      *plt = LinkBad; return nullptr;
    }
    if( pds->isChildOf( "ContScheme" ) ) {
      *plt = LinkSpec;
      return &fake_src;
    }
    // qWarning() << "^ detected in " << nm << " nnm= " << nnm << NWHE;
    const double* rv =  pds->getDoublePtr( nnm, lt, src_ob, lev );
    // qWarning() << "lt= " << *lt << "rv=" << ( (rv==nullptr) ? "nullptr" : "V") << " pds: " << pds->getFullName() << NWHE;
    return rv;
    // return pds->getDoublePtr( nnm, lt, src_ob, lev );
  }

  // own names: elements (no local vars?)
  const double *p =  getDoublePtr( nm, lt, src_ob, lev );
  if( p ) {
    return p;
  }

  // get model - for simulation and model itself
  TModel *mod = getAncestorT<TModel>();
  if( !mod ) {
    qWarning() << "No model detected" << NWHE;
    return nullptr;
  }

  // try active simulation
  Simulation *csim = mod->getActiveSimulation();
  if( csim ) {
    p = csim->getDoublePtr( nm, lt, src_ob, lev );
    if( p ) {
      return p;
    }
  }

  // then try model
  p =  mod->getDoublePtr( nm, lt, src_ob, lev );
  if( p ) {
    return p;
  }


  if( !p ) {
    qWarning() << "fail to find target " << nm << NWHE;
  }

  return p;
}

// must be in correspondence with getSchemeDoublePtr
void Scheme::fillComplModelForInputs( QStandardItemModel *mdl ) const
{
  if( ! mdl ) { return; }
  auto root_item = mdl->invisibleRootItem();


  fillComplForInputs( root_item );

  // if( !par ) { return; }
  // if( par->isObject( "TSubScheme" ) ) {  // ^ - TSubScheme -- no way to get anchor element
  //    par->fillComplForInputs( root_item, "^" );
  // }


  // get model - for simulation and model itself
  TModel *mod = getAncestorT<TModel>();
  if( !mod ) { return;  }

  // try active simulation
  Simulation *csim = mod->getActiveSimulation();
  if( csim ) {
     csim->fillComplForInputs( root_item );
  }

  // then model
  mod->fillComplForInputs( root_item );
}



int Scheme::runOneLoop( double t, IterType itype )
{
  for( auto cur_el : v_el ) {
    if( end_loop ) {
      break;
    }

     cur_el->fun( t, itype );  // <============ main action
  };

  return 1;
}

int Scheme::preRun( int run_tp, int N, int anx, int any, double tdt )
{
  reset();
  state = stateRun;
  for( auto ob : v_el ) {
    int rc = ob->preRun( run_tp, N, anx, any, tdt );
    if( !rc ) {
      qWarning() << "preRun failed for object " << ob->getFullName() << NWHE;
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

  if( modified == 0 && cm != 0 ) {
    modified |= modifAuto;
  }
  state = stateDone; // TODO: or state Bad?
  return 1;
}

void Scheme::do_reset()
{
  linkNames();
  state = stateGood; run_type = -1;
}

int Scheme::allStartLoop( int acnx, int acny )
{
  for( auto ob : v_el ) {
    int rc = ob->startLoop( acnx, acny );
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
  for( auto ob : TCHILD(TMiso*) ) {
    int oord = ob->getDataD( "ord", -1 );
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



TMiso* Scheme::xy2Miso( int avis_x, int avis_y ) const
{
  int  ox, oy;
  for( auto ob : TCHILD(TMiso*) ) {
    ox = ob->getDataD( "vis_x", -1 );
    oy = ob->getDataD( "vis_y", -1 );
    if( ox == avis_x && oy == avis_y ) {
      return ob;
    }
  }
  return nullptr;
}

QSize Scheme::getMaxXY() const
{
  int mx = 0, my = 0;
  for( auto ob : TCHILD(TMiso*) ) {
    int ox = 0, oy =0;
    ox = ob->getDataD( "vis_x", 0 );
    oy = ob->getDataD( "vis_y", 0 );
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
  // not addElemT, downcast
  TMiso *ob = qobject_cast<TMiso*>( addElemP( cl_name, ob_name ) );
  if( !ob ) {
    return nullptr;
  }
  ob->setData( "ord", aord );
  ob->setData( "vis_x", avis_x );
  ob->setData( "vis_y", avis_y );
  reset();
  return ob;
}

int Scheme::delElem( const QString &ename )
{
  TMiso *ob = getElemT<TMiso*>( ename );
  if( !ob ) {
    qWarning() << "fail to find TMiso " << ename << NWHE;
    return 0;
  }
  int rc = del_obj( ename );
  if( rc ) {
    reset();
  }
  return rc;
}


int Scheme::newOrder( const QString &name, int new_ord )
{
  TMiso *ob = getElemT<TMiso*>( name );
  if( !ob ) {
    return -1;
  }
  if( ord2Miso( new_ord ) != nullptr ) {
    return -1;
  }
  ob->setData( "ord", new_ord );
  int k = ob->getDataD( "ord", -1 );

  reset();
  return ( k == new_ord ) ? 0 : -1;
}

int Scheme::moveElem( const QString &nm, int newx, int newy )
{
  TMiso *ob, *ob1;
  if( newx < 0 || newy < 0 ) {
    return -1;
  }
  ob = getElemT<TMiso*>( nm );
  if( !ob ) {
    return -1;
  }

  ob1 = xy2Miso( newx, newy );
  if( ob1 ) {
    return -1; // busy
  }

  ob->setData( "vis_x", newx );
  ob->setData( "vis_y", newy );
  reset();
  return 0;
}

int Scheme::linkNames()
{
  v_el.clear();

  for( auto ob : TCHILD(TMiso*) ) {
    v_el.push_back( ob );
  };

  sortOrd();

  return 0;
}

void Scheme::do_structChanged()
{
  TDataSet::do_structChanged();
  linkNames();
}

void Scheme::sortOrd()
{
  std::sort( v_el.begin(), v_el.end(),
      []( const TMiso *a, const TMiso*b) { return *a < *b; } );
}

int Scheme::hintOrd() const
{
  int m = 0;
  for( auto ob : TCHILD(TMiso*) ) {
    int mc = ob->getDataD( "ord", 0 );
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

CTOR(ContScheme,TDataSet)
{
  allowed_types = "Scheme,+SPECIAL";
}

ContScheme::~ContScheme()
{
}

DEFAULT_FUNCS_REG(ContScheme)



// end of scheme.cpp

