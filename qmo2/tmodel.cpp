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
  rtime = t = 0; tdt = 1; // fake

  schems = addObj<ContScheme>( "schems" );
  main_s = schems->addObj<Scheme>( "main_s" );
  schems->setActiveElem( "main_s" );
  outs = addObj<ContOut>( "outs" );
  plots = addObj<ContGraph>( "plots" );
  sims = addObj<ContSimul>( "sims" );
  sims->addObj<Simulation>( "sim0" );
  sims->setActiveElem( "sim0" );

}

TModel::~TModel()
{
}

const double* TModel::getSchemeDoublePtr( const QString &nm, ltype_t *lt,
        const TDataSet **src_ob, int lev) const
{
  Scheme *sch = getActiveScheme();
  const double *rv = nullptr;
  if( sch ) {
    rv =  sch->getDoublePtr( nm, lt, src_ob, lev );
  };
  if( rv ) {
    return rv;
  };
  // may be some model params?
  return getDoublePtr( nm, lt, src_ob, lev );
}




int TModel::reset()
{
  state = stateGood; run_type = -1; sgnt = int( time(0) );
  if( c_sch ) {
    c_sch->reset();
  }
  resetOutArrs( 1000 ); // all?
  return 0;
}

int TModel::startRun()
{
  int rc;
  if( run_type >= 0 ) { // in progress now
    DBGx( "warn: run_type = %d during startRun", run_type );
    return 0;
  }
  reset();

  c_sim = getActiveSimulation();
  if( !c_sim ) {
    DBG1( "warn: No active simulation" );
    return 0;
  }

  c_sch = getActiveScheme();
  if( !c_sch ) {
    DBG1( "warn: No active scheme" ); c_sim = nullptr;
    return 0;
  }

  // copy of simulation data (need tmp var)
  double x; int xi;
  c_sim->getData( "T", &x ); T = x;
  c_sim->getData( "N", &xi ); N = xi;
  c_sim->getData( "N1", &xi ); N1 = xi;
  c_sim->getData( "N2", &xi ); N2 = xi;
  c_sim->getData( "n1_eff", &xi ); n1_eff = xi;
  c_sim->getData( "n2_eff", &xi ); n2_eff = xi;
  c_sim->getData( "n_tot", &xi ); n_tot = xi;
  c_sim->getData( "syncRT", &xi ); syncRT = xi;
  c_sim->getData( "prm0s", &x ); prm0s = x;
  c_sim->getData( "prm1s", &x ); prm1s = x;
  c_sim->getData( "prm2s", &x ); prm2s = x;
  c_sim->getData( "prm3s", &x ); prm3s = x;
  c_sim->getData( "prm0d", &x ); prm0d = x;
  c_sim->getData( "prm1d", &x ); prm1d = x;
  c_sim->getData( "seed",  &xi ); seed  = xi;
  c_sim->getData( "seedType",  &xi ); seedType  = xi;

  int type = Simulation::runSingle;
  c_sim->getData( "runType", &type );

  if( type     !=  Simulation::runSingle
      &&  type !=  Simulation::runLoop
      &&  type !=  Simulation::run2DLoop ) {
    type = Simulation::runSingle;
  }

  run_type = type;

  i_tot = ii = il1 = il2 = 0;
  sgnt = int( time( 0 ) );
  tdt = T / N;
  prm2 = (double)prm2s; prm3 = (double)prm3s;

  allocOutArrs( run_type );

  rc = c_sch->preRun( run_type, N, n1_eff, n2_eff, tdt );
  if( !rc ) {
    DBG1( "warn: scheme preRun failed" );
    reset();
    c_sim = nullptr; c_sch = nullptr;
    return 0;
  }
  state = stateRun;

  return 1;
}

int TModel::nextSteps( int csteps )
{
  int i, rc;
  if( !c_sch ) {
    DBG1( "warn: No active scheme" );
    return 0;
  }

  if( csteps < 1 ) {
    csteps = 1;
  }

  for( i=0; i < csteps && i_tot < n_tot && end_loop == 0; ++i, ++i_tot ) {
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
    if( !rc ) {
      return 0;
    }

    if( ii >= N ) {
      allEndLoop();
      ii = 0; ++il1;
    };
    if( il1 >= N1 ) {
      il1 = 0; ++il2;
    };
    if( i_tot >= n_tot ) {
      stopRun(0);
    }
  }

  return 1;
}

int TModel::stopRun( int reason )
{
  if( !c_sch ) {
    DBG1( "warn: No active scheme" );
    return 0;
  }

  if( end_loop || reason ) {
    reset();
    state = stateGood;
  } else {
    postRun();
    state = stateDone;
  }
  return 1;
}

int TModel::runOneLoop()
{
  if( !c_sch ) {
    DBG1( "warn: No active scheme" );
    return 0;
  }

  rtime = get_real_time() - start_time;
  if( use_sync ) {
     if( t > rtime ) {
       unsigned long wait_ms = (unsigned long)( 1000000 * ( t - rtime ) );
       usleep( wait_ms ); // ------------------- TODO: redesign ------
     };
  };

  IterType itype = IterMid;
  int out_level = 0;
  if( ii == 0 ) {
    itype = IterFirst;
  } else if( ii == N-1 ) {
    itype = IterLast;
    out_level = run_type;
  };

  int rc = c_sch->runOneLoop( t, itype );
  if( !rc ) {
    end_loop = 1;
  }

  outs->takeAllVals( out_level );

  t += tdt; ++ii;
  return 1;
}


int TModel::postRun()
{
  if( !c_sch ) {
    return 0;
  }

  int rc = c_sch->postRun();

  int cm = c_sch->getModified();
  if( modified == 0 || cm != 0 ) {
    modified |= 2;
  }
  return rc;
}

int TModel::allStartLoop( int acnx, int acny )
{
  int rc = 0;

  if( c_sch ) {
    rc = c_sch->allStartLoop( acnx, acny );
  }
  // TODO: outs
  return rc;
}

void TModel::allEndLoop()
{
  if( c_sch ) {
    c_sch->allEndLoop();
  }
  // TODO: outs
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
    switch( out_tp ) { // TODO: move logic to TOutArr
      case 0: arr->alloc( N, 1 ); break;
      case 1: arr->alloc( n1_eff, 1 ); break;
      case 2: arr->alloc( n1_eff * n2_eff, n1_eff ); break;
      default: ; // don't allocate special arrays
    };
  };
}

void TModel::resetOutArrs( int level )
{
  for( QObject *o: outs->children() ) { // free output array
    TOutArr *arr = qobject_cast<TOutArr*>( o );
    if( !arr ) {
      continue;
    }
    arr->reset( level );
  };
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
  if( !arr ) {
    return 0;
  }

  arr->setData( "name", objname );

  QString lbl = objname;
  if( lbl.left(4) == "out_" )
    lbl.remove(0,4);
  if( lbl.left(1) == ":" )
    lbl.remove(0,1);

  arr->setData( "label", lbl );

  reset();
  modified |= 1;
  return 1;
}

int TModel::insGraph( const QString &gname )
{
  TGraph *gra = plots->addObj<TGraph>( gname );
  if( !gra ) {
    return 0;
  }
  reset();
  modified |= 1;
  return 1;
}



int TModel::delOut( const QString &name )
{
  return outs->del_obj( name );
}

int TModel::delGraph( const QString &name )
{
  return plots->del_obj( name );
}

int TModel::addOutToGraph( const QString &o_name, const QString &g_name )
{
  TGraph *gra = getGraph( g_name );
  if( !gra ) {
    return 0;
  }
  int rc = gra->addOutArr( o_name );
  if( rc ) {
    reset();
  }
  return rc;
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

Simulation* TModel::getSimul( const QString &name )
{
  return sims->getElemT<Simulation*>( name );
}






DEFAULT_FUNCS_REG(TModel)


// end of tmodel.cpp

