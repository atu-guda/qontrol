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
  schems->setImmutable();
  main_s = schems->addObj<Scheme>( "main_s" );
  main_s->setImmutable();
  schems->setActiveElem( "main_s" );

  outs = addObj<ContOut>( "outs" );
  outs->setImmutable();
  plots = addObj<ContGraph>( "plots" );
  plots->setImmutable();

  sims = addObj<ContSimul>( "sims" );
  sims->setImmutable();
  Simulation *sim0 = sims->addObj<Simulation>( "sim0" );
  sim0->setImmutable();
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
  rv =  getDoublePtr( nm, lt, src_ob, lev );

  if( !rv ) {
    DBGx( "warn: fail to find target \"%s\" in model", qP(nm) );
  }

  return rv;
}




int TModel::reset()
{
  state = stateGood; run_type = -1; sgnt = int( time(0) );
  if( c_sch ) {
    c_sch->reset();
  }
  if( outs ) {
    outs->reset();
  }
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

  T   = c_sim->getDataD( "T", 0.0 );
  N   = c_sim->getDataD( "N", 1 );
  N1  = c_sim->getDataD( "N1", 1 );
  N2  = c_sim->getDataD( "N2", 1 );
  n1_eff = c_sim->getDataD( "n1_eff", N1 );
  n2_eff = c_sim->getDataD( "n2_eff", N2 );
  n_tot  = c_sim->getDataD( "n_tot", N );
  syncRT = c_sim->getDataD( "syncRT", 0 );
  prm0s = c_sim->getDataD( "prm0s", 0.0 );
  prm1s = c_sim->getDataD( "prm1s", 0.0 );
  prm2s = c_sim->getDataD( "prm2s", 0.0 );
  prm3s = c_sim->getDataD( "prm3s", 0.0 );
  prm0d = c_sim->getDataD( "prm0d", 0.0 );
  prm1d = c_sim->getDataD( "prm1d", 0.0 );
  seed = c_sim->getDataD( "seed", 1 );
  seedType = c_sim->getDataD( "seedType",  0 );

  int type = c_sim->getDataD( "runType", (int)Simulation::runSingle );

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

  rc = outs->preRun( run_type, N, n1_eff, n2_eff, tdt );
  if( !rc ) {
    DBG1( "warn: output arrays preRun failed" );
    reset();
    c_sim = nullptr; c_sch = nullptr;
    return 0;
  }

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

      start_time = get_real_time(); rtime = t = 0;
      // set start param, reset arrays(if need);
      allStartLoop( il1, il2 );
    };// end start inner loop

    rc = runOneLoop();
    if( !rc ) {
      return 0;
    }

    if( ii >= N ) {
      allEndLoop( il1, il2 );
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
  if( syncRT ) {
     if( t > rtime ) {
       unsigned long wait_ms = (unsigned long)( 1000000 * ( t - rtime ) );
       usleep( wait_ms ); // ------------------- TODO: redesign ------
     };
  };

  IterType itype = IterMid;

  int rc = c_sch->runOneLoop( t, itype );
  if( !rc ) {
    end_loop = 1;
  }

  outs->takeAllVals();

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
  if( outs ) {
    rc = outs->startLoop( acnx, acny );
  }
  return rc;
}

void TModel::allEndLoop( int acnx, int acny )
{
  if( c_sch ) {
    c_sch->allEndLoop();
  }
  if( outs ) {
    outs->endLoop( acnx, acny );
  }
}




// ------------------------------------------

TOutArr* TModel::getOutArr( const QString &oname )
{
  if( !outs ) {
    return nullptr;
  }
  return outs->getElemT<TOutArr*>( oname );
}

TGraph* TModel::getGraph( const QString &name )
{
  if( !plots ) {
    return nullptr;
  }
  return plots->getElemT<TGraph*>( name );
}


int TModel::insOut( const QString &outname, const QString &objname )
{
  if( !outs ) {
    return 0;
  }
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

// ------------------------------------------

int TModel::insGraph( const QString &gname )
{
  if( !plots ) {
    return 0;
  }
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
  if( !outs ) {
    return 0;
  }
  return outs->del_obj( name );
}

int TModel::delGraph( const QString &name )
{
  if( !plots ) {
    return 0;
  }
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

bool TModel::cloneGraph( const QString &old_name, const QString &new_name )
{
  if( !plots ) {
    return 0;
  }
  TGraph *old_gra = plots->getElemT<TGraph*>( old_name );
  if( !old_gra ) {
    DBGx( "warn: old plot \"%s\" not exist", qP( old_name ) );
    return 0;
  }

  QString s = old_gra->toString();

  TGraph *new_gra = plots->addObj<TGraph>( new_name );
  if( !new_gra ) {
    DBGx( "warn: fail to create new plot \"%s\"", qP( new_name ) );
    return 0;
  }
  return new_gra->fromString( s );
}

// ------------------------------------------

int TModel::newSimul( const QString &name )
{
  if( !sims ) {
    return 0;
  }
  Simulation *sim = sims->addObj<Simulation>( name );
  if( ! sim ) {
    DBGx( "ERR: fail to create simulation \"%s\"", qP(name) );
    return 0;
  }
  return 1;
}

int TModel::delSimul( const QString &name )
{
  if( !sims ) {
    return 0;
  }
  return sims->del_obj( name );
}

QString TModel::getSimulName( int idx )
{
  if( !sims ) {
    return QString();
  }
  Simulation* sim =  sims->getElemT<Simulation*>( idx );
  if( !sim ) {
    return QString();
  }
  return sim->objectName();
}

Simulation* TModel::getSimul( const QString &name )
{
  if( !sims ) {
    return nullptr;
  }
  return sims->getElemT<Simulation*>( name );
}


bool TModel::cloneSimul( const QString &old_name, const QString &new_name )
{
  if( !sims ) {
    return false;
  }
  Simulation *old_sim = sims->getElemT<Simulation*>( old_name );
  if( !old_sim ) {
    DBGx( "warn: old simulation \"%s\" not exist", qP( old_name ) );
    return 0;
  }

  QString s = old_sim->toString();

  Simulation *new_sim = sims->addObj<Simulation>( new_name );
  if( !new_sim ) {
    DBGx( "warn: fail to create new simulation \"%s\"", qP( new_name ) );
    return 0;
  }
  return new_sim->fromString( s );
}




DEFAULT_FUNCS_REG(TModel)


// end of tmodel.cpp

