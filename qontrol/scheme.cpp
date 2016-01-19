/***************************************************************************
                          scheme.cpp
                             -------------------
    begin                : 2013.09.01
    copyright            : (C) 2000-2016 by atu
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

#include <boost/chrono/chrono.hpp>
#include <boost/bind.hpp>

#include <QStandardItem>

#include "miscfun.h"
#include "scheme.h"
#include "tmodel.h"

using namespace std;

const char* Scheme::helpstr = "<H1>Scheme</H1>\n"
 "Hold all active elements";

STD_CLASSINFO(Scheme,clpSpecial | clpContainer);

CTOR(Scheme,LinkedObj)
{
  allowed_types = "TMiso";
  const int ELM_RES = 64;
  v_el.reserve( ELM_RES );
  needReadInputsRecurse = true;
}


QVariant Scheme::dataObj( int col, int role ) const
{
  if( role == Qt::BackgroundRole ) {
    if( col != 0 ) {
      return TDataSet::dataObj( col, role );
    }
    if( hasAllFlags( efTmp )  ) { // transient schemes
      return QBrush( QColor(128,128,128) ) ;
    }
    return TDataSet::dataObj( col, role );

  }
  // else if ( role == Qt::ToolTipRole ) {
  // }
  // else if( role == Qt::StatusTipRole ) { // used for button labels in dialogs
  // }

  return TDataSet::dataObj( col, role );
}


// must be similar to fillComplModelForInputs
const double* Scheme::getSchemeDoublePtr( const QString &nm, int *lt,
        const LinkedObj **src_ob, int lev) const
{
  static int clt;
  static const double fake_src = 0.123456; // ptr here for non- main_s schemes uplinks
  int *plt = lt ? lt : &clt;

  if( nm.startsWith( '^' ) ) { // direct to parent
    QString nnm = nm.mid( 1 );
    LinkedObj *pds = qobject_cast<LinkedObj*>( par );
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



int Scheme::runOneLoop( IterType itype )
{
  // readInputs(); // <-- too slow here
  for( auto cur_el : v_el ) {
     cur_el->readInputs(); // optimization
  }

  for( auto cur_el : v_el ) {
    if( end_loop ) {
      break;
    }

    cur_el->fun( itype );  // <============ main action
  };

  return 1;
}



void Scheme::do_reset()
{
  // linkNames();
  prepared = false; n_th = 0; v_elt.clear(); vth.clear();
  barr0 = nullptr;  barr1 = nullptr;
  state = stateGood; run_type = -1;
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
  for( auto ob : TCHILD(TMiso*) ) {
    int ox = ob->getDataD( "vis_x", -1 );
    int oy = ob->getDataD( "vis_y", -1 );
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
    int ox = ob->getDataD( "vis_x", 0 );
    int oy = ob->getDataD( "vis_y", 0 );
    if( ox > mx ) {
      mx = ox;
    }
    if( oy > my ) {
      my = oy;
    }
  }
  return QSize( mx, my );
}


TMiso* Scheme::addElem( const QString &cl_name, const QString &ob_name,
                     int aord, int avis_x, int avis_y )
{
  // not addObjT, downcast
  TMiso *ob = qobject_cast<TMiso*>( addObjP( cl_name, ob_name ) );
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
  int rc = delObj( ename );
  if( rc ) {
    reset();
  }
  return rc;
}


int Scheme::newOrder( const QString &name, int new_ord )
{
  TMiso *ob = getObjT<TMiso*>( name );
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
  ob = getObjT<TMiso*>( nm );
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


void Scheme::do_structChanged()
{
  LinkedObj::do_structChanged();
  v_el.clear();

  for( auto ob : TCHILD(TMiso*) ) {
    int ign = 0;
    ob->getData( "ignored", &ign, false );
    if( ign ) {
      continue;
    }
    v_el.push_back( ob );
  };

  sortOrd();
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
    int mc = ob->getDataD( QSL("ord"), 0 );
    if( mc > m ) {
      m = mc;
    }
  };
  int m1 = ( (m+10) / 10 ) * 10;
  return m1;
}

int Scheme::th_prep()
{
  auto vsz = v_el.size();
  unsigned el_per_th = ( vsz + n_th - 1 ) / n_th;
  unsigned nt = n_th * el_per_th; // total number of elements, counting nulls
  v_elt.clear();
  v_elt.resize( n_th ); // TODO: each::reserve( el_per_th );
  for( auto vt : v_elt ) { vt.reserve( el_per_th ); };

  for( unsigned i=0; i<nt; ++i ) {
    unsigned tn = i % n_th;
    v_elt[tn].push_back( ( i < vsz ) ? v_el[i] : nullptr );
  }

  prepared = true;
  qWarning() << "Prep: vsz= " << vsz << " n_th= " << n_th
             << " el_per_th " << el_per_th << " nt= " << nt << NWHE;

  return 1;
}

int Scheme::th_run( unsigned n_th_ )
{
  L_GUARD( run_mutex );

  n_th = n_th_;
  if( n_th < 1 ) { n_th = 1; }

  th_prep();
  barri a_ba0( n_th + 1 ); barr0 = &a_ba0;
  barri a_ba1( n_th + 1 ); barr1 = &a_ba0;

  vth.clear();

  for( unsigned i=0; i<n_th; ++i ) {
    vth.push_back( boost::thread( boost::bind( &Scheme::th_stage0, this, i ) ) );
  }
  vth.push_back( boost::thread( boost::bind( &Scheme::th_stage1, this ) ) );

  for( auto &th : vth ) {
    th.join();
  }

  qWarning() << "*** END work ***" << WHE;
  vth.clear();
  barr0 = nullptr;
  barr1 = nullptr;

  return 1;
}


int Scheme::th_stage0( unsigned nt )
{
  if( !barr0 || !barr1 ) {
    qWarning() << "No barriers!!!" << NWHE;
    th_interrupt_all();
    return 0;
  }
  if( !prepared ) {
    qWarning() << "not prepared " << NWHE;
    th_interrupt_all();
    return 0;
  }
  if( nt >= v_elt.size() ) {
    qWarning() << "bad nt: " << nt << NWHE;
    th_interrupt_all();
    return 0;
  }

  int nr = 0;
  IterType itype = IterFirst;
  for( unsigned i=0; i<2 /*N*/; ++i ) {
    for( auto v : v_elt[nt] ) {
      v->readInputs();
      v->fun( itype );
      if( end_loop ) {
        th_interrupt_all();
        return -1;
      }
    }
    boost::this_thread::interruption_point();
    barr0->wait();
    barr1->wait();
    ++nr;
  }
  return nr;
}

int Scheme::th_stage1()
{
  using namespace boost::chrono;
  if( !barr0 || !barr1 ) {
    qWarning() << "\nNo barriers!!!" << NWHE;
    th_interrupt_all();
    return 0;
  }

  // system_clock::time_point tm = system_clock::now();
  for( unsigned i=0; i<2 /*N*/; ++i ) {
    barr0->wait();
    // if( wait_ms > 0 ) {
    //   tm += milliseconds( wait_ms );
    //   boost::this_thread::sleep_until( tm );
    // }
    // -------------- main work here -----------------
    // t += tdt;
    barr1->wait();
  }
  return 1;
}

void Scheme::th_interrupt_all()
{
  for( auto &th : vth ) {
    th.interrupt();
  }
}

DEFAULT_FUNCS_REG(Scheme)


// ------------------ container of Schemes --------------


const char* ContScheme::helpstr = "<H1>ContScheme</H1>\n"
 "Container of simulations";

STD_CLASSINFO(ContScheme,clpSpecial | clpContainer);

CTOR(ContScheme,LinkedObj)
{
  allowed_types = "Scheme,+SPECIAL";
  needReadInputsRecurse = true;
}

DEFAULT_FUNCS_REG(ContScheme)



// end of scheme.cpp

