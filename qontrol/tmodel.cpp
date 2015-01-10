/***************************************************************************
                          tmodel.cpp  -  description
                             -------------------
    begin                : Tue Aug 1 2000
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
#include <QSemaphore>
#include <QScriptEngine>

#include "miscfun.h"
#include "tmodel.h"

using namespace std;

// -------------------- helper JS functions ---------------------------------------
static QScriptValue script_int2str( QScriptContext *ctx, QScriptEngine * /*eng*/ )
{
  int narg = ctx->argumentCount();
  int n = 0;
  if( narg > 0 ) {
    n = ctx->argument(0).toInt32();
  }
  int fw = 1;
  if( narg > 1 ) {
    fw = ctx->argument(1).toInt32();
  }
  QChar fc = ' ';
  if( narg > 2 ) {
    fc = ctx->argument(2).toString()[0];
  }
  int base = 10;
  if( narg > 3 ) {
    base = ctx->argument(3).toInt32();
  }
  QString s = QString( "%1" ).arg( n, fw, base, fc );
  return QScriptValue( s );
}

QString TripleF::toString() const
{
  return QString( "[ %1; %2; $3 ]").arg(a).arg(b).arg(c);
}

QScriptValue TripleFtoScriptValue( QScriptEngine *eng, const TripleF &s )
{
  QScriptValue obj = eng->newObject();
  obj.setProperty( "a", s.a );
  obj.setProperty( "b", s.b );
  obj.setProperty( "c", s.c );
  return obj;
}

void fromScriptValuetoTripleF( const QScriptValue &obj, TripleF &s )
{
  s.a = obj.property("a").toNumber();
  s.b = obj.property("b").toNumber();
  s.c = obj.property("c").toNumber();
}

// ------------------------- TModel --------------------------------------

const char* TModel::helpstr = "<H1>TModel</H1>\n"
 "Hold all active elements, output arrays and graph descriptions";

STD_CLASSINFO(TModel,clpSpecial | clpContainer);

CTOR(TModel,TDataContainer)
{
  allowed_types = "HolderValue,"
                  "ContScheme,ContSimul,ContOut,ContGraph"; // +SPECIAL
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

  initEngine();
}

TModel::~TModel()
{
  delete eng; eng = nullptr; // just for...
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

QString TModel::getOutValue( const QString &nm ) const
{
  QString r;
  Scheme *sch = getActiveScheme();
  if( sch  &&  sch->getData( nm + ".out0", r, false ) ) {
    return r;
  };
  Simulation *cs = getActiveSimulation();
  if( cs  &&  cs->getData( nm, r, false ) ) {
    return r;
  };
  // may be some model params?
  if( getData( nm, r, false ) ) {
    return r;
  }
  return r;
}





void TModel::do_reset()
{
  state = stateGood; run_type = -1; sgnt = int( time(0) );
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
  n_iosteps = c_sim->getDataD( "n_iosteps",  1 );
  if( n_iosteps >= N ) {
    n_iosteps = N/10;
  }
  if( n_iosteps < 1 ) {
    n_iosteps = 1;
  }

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

  int useScripts  = c_sim->getDataD( "useScripts", 0 );
  int initEng = 0, execModelScript = 0;
  if( useScripts ) {
    initEng         = c_sim->getDataD( "initEng", 1 );
    execModelScript = c_sim->getDataD( "execModelScript", 1 );
  }
  if( initEng ) {
    initEngine();
  }
  if( useScripts ) {
    eng->globalObject().setProperty( "T", eng->newVariant( (double)T ) );
    eng->globalObject().setProperty( "tdt", eng->newVariant( (double)tdt ) );
    eng->globalObject().setProperty( "N", eng->newVariant( (int)N ) );
    eng->globalObject().setProperty( "N1", eng->newVariant( (int)n1_eff ) );
    eng->globalObject().setProperty( "N2", eng->newVariant( (int)n2_eff ) );
    // alias objs
    eng->globalObject().setProperty( "c_sim", eng->newQObject( c_sim ) );
  }
  if( execModelScript ) {
    runModelScript();
  }

  return 1;
}


int TModel::run( QSemaphore *sem )
{
  if( !sem || ! c_sch || !c_sim ) {
    DBG1( "warn: bad init!" );
    return 0;
  }

  QString scriptPreRun, scriptPostRun, scriptStartLoop, scriptEndLoop;
  QScriptValue v_il1, v_il2;

  int useScripts  = c_sim->getDataD( "useScripts", 0 );
  if( useScripts ) {
    scriptPreRun    = c_sim->getDataD( "scriptPreRun", QString() );
    scriptPostRun   = c_sim->getDataD( "scriptPostRun", QString() );
    scriptStartLoop = c_sim->getDataD( "scriptStartLoop", QString() );
    scriptEndLoop   = c_sim->getDataD( "scriptEndLoop", QString() );
  }

  if( ! scriptPreRun.isEmpty() ) {
    runScript( scriptPreRun );
  }

  int rc = 0;
  for( il2 = 0; il2 < n2_eff; ++il2 ) {
    prm1 = prm1s + il2 * prm1d;
    for( il1 = 0; il1 < n1_eff; ++il1 ) {


      prm0 = prm0s + il1 * prm0d;
      start_time = get_real_time(); rtime = t = 0;

      allStartLoop( il1, il2 );
      if( useScripts ) {
        eng->globalObject().setProperty( "il1", eng->newVariant( (int)(il1) ) );
        eng->globalObject().setProperty( "il2", eng->newVariant( (int)(il2) ) );
        eng->globalObject().setProperty( "prm0", eng->newVariant( (double)(prm0) ) );
        eng->globalObject().setProperty( "prm1", eng->newVariant( (double)(prm1) ) );
        if( ! scriptStartLoop.isEmpty() ) {
          runScript( scriptStartLoop );
        }
      }

      for( int i=0; i<N; ++i, ++i_tot ) {

        if ( QThread::currentThread()->isInterruptionRequested() ) { // check for break
          stopRun( 0 );
          return 0;
        }

        IterType itype = IterMid;
        if( i == 0 ) {
          itype = IterFirst;
        } else if ( i == (N-1) ) {
          itype = IterLast;
        }

        rtime = get_real_time() - start_time;
        if( syncRT ) {
          if( t > rtime ) {
            unsigned long wait_ms = (unsigned long)( 1000000 * ( t - rtime ) );
            usleep( wait_ms ); // ------------------- TODO: redesign ?? ------
          };
        };
        sem->acquire( 1 );

        rc = runOneLoop( itype );

        sem->release( 1 );

        if( !rc ) {
          stopRun( 0 );
          return 0;
        }
      } // -- main loop (i)

      allEndLoop( il1, il2 );
      if( plots ) {
        plots->reset();
      }
      if( ! scriptEndLoop.isEmpty() ) {
        runScript( scriptEndLoop );
      }
    } // -- inner loop (il1)
  }   // -- outer loop (il2)

  if( ! scriptPostRun.isEmpty() ) {
    runScript( scriptPostRun );
  }

  stopRun( 0 );
  return i_tot;
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
  run_type = -1;
  return 1;
}

int TModel::run_bg()
{
  QSemaphore sem_io( 1 ); // really not need, may be more?
  int rc = startRun();
  if( !rc ) {
    return 0;
  }

  rc = run( &sem_io );
  return rc;
}

void TModel::plotToPng( const QString &gname, const QString &fn )
{
  TGraph *gra = getGraph( gname );
  if( !gra ) {
    return;
  }
  gra->plotToPng( fn );
}

int TModel::runOneLoop( IterType itype )
{
  if( !c_sch ) {
    DBG1( "warn: No active scheme" );
    return 0;
  }

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
  if( outs ) {
    outs->postRun( 1 );
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

bool TModel::setActiveSimul( const QString &name )
{
  if( !sims || name.isEmpty() ) {
    return false;
  }

  bool ok = sims->setActiveElem( name );
  if( ok ) {
    handleStructChanged();
  }
  return ok;
}


void TModel::initEngine()
{
  delete eng;
  eng = new QScriptEngine( this );

  qScriptRegisterMetaType( eng, TripleFtoScriptValue, fromScriptValuetoTripleF );

  eng->globalObject().setProperty( "model", eng->newQObject( this ) );
  // aliases
  eng->globalObject().setProperty( "main_s", eng->newQObject( main_s ) );
  eng->globalObject().setProperty( "outs", eng->newQObject( outs ) );
  eng->globalObject().setProperty( "plots", eng->newQObject( plots ) );
  // funcs
  eng->globalObject().setProperty( "int2str", eng->newFunction( script_int2str ) );
}

QString TModel::runScript( const QString& script )
{
  QScriptValue res = eng->evaluate( script );
  QString r;
  if( eng->hasUncaughtException() ) {
    int line = eng->uncaughtExceptionLineNumber();
    r = "Error: uncaught exception at line " % QSN( line ) % " : \n";
  }
  r += res.toString();
  return r;
}

QString TModel::runModelScript()
{
  return runScript( script );
}


DEFAULT_FUNCS_REG(TModel)

// ------------------------------ ModelRunner --------------------------


ModelRunner::ModelRunner( TModel *a_model, QSemaphore *a_sem, QObject *parent )
  : QThread( parent ), model( a_model ), sem( a_sem )
{
}

void ModelRunner::run()
{
  if( !model || !sem ) {
    return;
  }
  int rc = model->run( sem );
  emit resultReady( rc );
}


// end of tmodel.cpp

