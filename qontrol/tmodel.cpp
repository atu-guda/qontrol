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
#include <unistd.h>
#include <algorithm>
#include <QSemaphore>
#include <QScriptEngine>
#include <QStandardItem>

#include "miscfun.h"
#include "autoact.h"
#include "tmodel.h"
#include "scriptfuncs.h"
#include "labodoc.h"

using namespace std;

// ------------------------- TModel --------------------------------------

const char* TModel::helpstr = "<H1>TModel</H1>\n"
 "Hold all schems, output arrays, graph descriptions and simulations";

STD_CLASSINFO(TModel,clpSpecial | clpContainer);

CTOR(TModel,LinkedObj)
{
  allowed_types = "HolderValue,"
                  "ContScheme,ContSimul,ContOut,ContGraph"; // +SPECIAL
  rtime =0; t = 0; tdt =1; // fake

  schems = addObjT<ContScheme>( "schems" );
  schems->setParm( "sep", "tab" );
  schems->setParm( "tabname", "Containers" );
  schems->setImmutable();
  main_s = schems->addObjT<Scheme>( "main_s" );
  main_s->setImmutable();
  schems->setActiveObj( "main_s" );

  outs = addObjT<ContOut>( "outs" );
  outs->setImmutable();

  plots = addObjT<ContGraph>( "plots" );
  plots->setParm( "sep", "col" );
  plots->setImmutable();

  sims = addObjT<ContSimul>( "sims" );
  sims->setParm( "sep", "tabend" );
  sims->setParm( "tabname", "Params" );
  sims->setImmutable();
  Simulation *sim0 = sims->addObjT<Simulation>( "sim0" );
  sim0->setImmutable();
  sims->setActiveObj( "sim0" );

  initEngine();
}

TModel::~TModel()
{
  delete eng; eng = nullptr; // just for...
}


// must be in correspondence with fillComplModelForInputs
const double* TModel::getSchemeDoublePtr( const QString &nm, int *lt,
        const LinkedObj **src_ob, int lev) const
{
  const double *rv = nullptr;

  if( nm.isEmpty() ) { // special null-case: empty string is no error
    return rv;
  }

  Scheme *sch = getActiveScheme(); // for upside call, double useless work if called from scheme.
  if( sch ) {
    rv =  sch->getDoublePtr( nm, lt, src_ob, lev );
    if( rv ) {
      return rv;
    };
  };

  // may be some model params? // overkill ? called by scheme?
  rv =  getDoublePtr( nm, lt, src_ob, lev );

  if( !rv ) {
    qWarning() << "fail to find target " << nm << " in model" << WHE;  // TODO: only users
  }

  return rv;
}

double* TModel::getMapDoublePtr( const QString &nm )
{
  double *r = &fake_map_target;
  if( nm.isEmpty() ) {
    return r;
  }
  QString nm1 = nm;
  QString prm_map = nm;
  int target_flag = 0; // unused?

  Simulation *sim = getActiveSimulation();
  if( sim ) {
    if( nm1[0] == '>' ) {
      nm1.remove( 0, 1 );
      sim->getData( nm1, prm_map );
    }
    if( prm_map.isEmpty() ) {
      return r;
    }
    r = sim->getDoublePrmPtr( prm_map, &target_flag );
    if( r ) { // Good
      return r;
    }
  }

  r = getDoublePrmPtr( prm_map, &target_flag );
  if( r ) { // Good
    return r;
  }

  Scheme *sch = getActiveScheme();
  r = sch->getDoublePrmPtr( prm_map, &target_flag );
  if( r ) { // Good too
    return r;
  }

  r = &fake_map_target;
  qWarning() << "ptr not found for " << nm << " map target " << prm_map << NWHE;
  return r;
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
  prm0_targ = &fake_map_target; prm1_targ = &fake_map_target;
}

int TModel::startRun()
{
  int rc;
  if( run_type >= 0 ) { // in progress now
    qWarning() << "bad run_type during startRun " << run_type << NWHE;
    return 0;
  }
  if( autoImport ) {
    importAllSchemes(); // TODO: check?
  }
  reset();

  c_sim = getActiveSimulation();
  if( !c_sim ) {
    qWarning() << "No active simulation" << NWHE;
    return 0;
  }

  c_sch = getActiveScheme();
  if( !c_sch ) {
    qWarning() << "No active scheme" << NWHE;
    c_sim = nullptr;
    return 0;
  }


  // TODO: remove debug?
  c_sim->post_set();
  reportStructChanged();

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
  // qWarning() << "pre: n2_eff= " << n2_eff << " n1_eff= " << n1_eff << " N= " << N << WHE;

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

  prm0_targ = getMapDoublePtr( ">prm0_map" );
  prm1_targ = getMapDoublePtr( ">prm1_map" );

  rc = outs->preRun( run_type, N, n1_eff, n2_eff, tdt );
  if( !rc ) {
    qWarning() << "warn: output arrays preRun failed" << NWHE;
    reset();
    c_sim = nullptr; c_sch = nullptr;
    return 0;
  }

  rc = c_sch->preRun( run_type, N, n1_eff, n2_eff, tdt );
  if( !rc ) {
    qWarning() << " scheme preRun failed " << c_sch->getFullName() << WHE;
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
    qCritical() << "bad init!" << NWHE;
    return 0;
  }

  prm0_save = *prm0_targ; prm1_save = *prm1_targ;

  QString scriptPreRun, scriptPostRun, scriptStartLoop, scriptEndLoop;
  QScriptValue v_il1, v_il2;

  int useScripts  = c_sim->getDataD( "useScripts", 0 );
  if( useScripts ) {
    scriptPreRun    = c_sim->getDataD( "scriptPreRun", QString() );
    scriptPostRun   = c_sim->getDataD( "scriptPostRun", QString() );
    scriptStartLoop = c_sim->getDataD( "scriptStartLoop", QString() );
    scriptEndLoop   = c_sim->getDataD( "scriptEndLoop", QString() );
  }

  runScript( scriptPreRun ); // test for empty - inside

  int rc = 0;
  for( il2 = 0; il2 < n2_eff; ++il2 ) {
    prm1 = prm1s + il2 * prm1d;
    *prm1_targ = prm1;
    for( il1 = 0; il1 < n1_eff; ++il1 ) {


      prm0 = prm0s + il1 * prm0d;
      *prm0_targ = prm0;
      start_time = get_real_time(); rtime = t = 0;

      if( ! allStartLoop( il1, il2 ) ) {
        stopRun( 0 );
        return 0;
      }
      if( useScripts ) {
        eng->globalObject().setProperty( "il1", eng->newVariant( (int)(il1) ) );
        eng->globalObject().setProperty( "il2", eng->newVariant( (int)(il2) ) );
        eng->globalObject().setProperty( "prm0", eng->newVariant( (double)(prm0) ) );
        eng->globalObject().setProperty( "prm1", eng->newVariant( (double)(prm1) ) );
        runScript( scriptStartLoop );
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
      runScript( scriptEndLoop );
    } // -- inner loop (il1)
  }   // -- outer loop (il2)

  runScript( scriptPostRun );

  stopRun( 0 );
  return i_tot;
}

int TModel::stopRun( int reason )
{
  if( !c_sch ) {
    qWarning() << "No active scheme" << NWHE;
    return 0;
  }

  if( end_loop || reason ) {
    reset();
    state = stateGood;
  } else {
    postRun();
    state = stateDone;
  }

  int saveParams = c_sim->getDataD( "saveParams", 1 );
  if( saveParams ) {
    *prm0_targ = prm0_save;  *prm1_targ = prm1_save;
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
    qWarning() << "No active scheme" << NWHE;
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
  if( modified == 0 && cm != 0 ) {
    modified |= modifAuto;
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
  if( !rc ) {
    return 0;
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
  return outs->getObjT<TOutArr*>( oname );
}

TGraph* TModel::getGraph( const QString &name )
{
  if( !plots ) {
    return nullptr;
  }
  return plots->getObjT<TGraph*>( name );
}


TOutArr* TModel::addOut( const QString &outname, const QString &objname )
{
  if( !outs ) {
    return nullptr;
  }
  TOutArr *arr = outs->addObjT<TOutArr>( outname );
  if( !arr ) {
    return nullptr;
  }

  arr->setData( "name", objname );

  QString lbl = objname;
  if( lbl.left( 4 ) == QSL("out_") ) {
    lbl.remove( 0, 4 );
  }
  if( lbl.left( 1 ) == ":" ) {
    lbl.remove( 0, 1 );
  }

  arr->setData( "label", lbl );

  reset();
  return arr;
}

// ------------------------------------------

int TModel::addGraph( const QString &gname )
{
  if( addObjToSub( "plots", "TGraph", gname ) ) {
    reset();
    return 1;
  }
  return 0;
}



int TModel::delOut( const QString &name )
{
  return delObjFromSub( "outs", name );
}

int TModel::delGraph( const QString &name )
{
  return delObjFromSub( "plots", name );
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
  return cloneObjInSub( "plots", old_name, new_name );
}

// ------------------------------------------

int TModel::addSimul( const QString &name )
{
  return addObjToSub( "sims", "Simulation", name );
}

int TModel::delSimul( const QString &name )
{
  return delObjFromSub( "sims", name );
}

QString TModel::getSimulName( int idx )
{
  if( !sims ) {
    return QString();
  }
  return sims->childName( idx );
}

Simulation* TModel::getSimul( const QString &name )
{
  if( !sims ) {
    return nullptr;
  }
  return sims->getObjT<Simulation*>( name );
}


bool TModel::cloneSimul( const QString &old_name, const QString &new_name )
{
  return cloneObjInSub( "sims", old_name, new_name );
}

bool TModel::setActiveSimul( const QString &name )
{
  if( !sims || name.isEmpty() ) {
    return false;
  }

  bool ok = sims->setActiveObj( name );
  if( ok ) {
    handleStructChanged();
  }
  return ok;
}

// ------------------------------------------

int TModel::addScheme( const QString &name )
{
  return addObjToSub( "schems", "Scheme", name );
}

int TModel::delScheme( const QString &name )
{
  return delObjFromSub( "schems", name );
}

QString TModel::getSchemeName( int idx )
{
  if( !schems ) {
    return QString();
  }
  return schems->childName( idx );
}

Scheme* TModel::getScheme( const QString &name )
{
  if( !schems ) {
    return nullptr;
  }
  return schems->getObjT<Scheme*>( name );
}


bool TModel::cloneScheme( const QString &old_name, const QString &new_name )
{
  return cloneObjInSub( "schems", old_name, new_name );
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
  eng->globalObject().setProperty( "print", eng->newFunction( script_print ) );
  eng->globalObject().setProperty( "isNear", eng->newFunction( script_isNear ) );
  eng->globalObject().setProperty( "include", eng->newFunction( script_include ) );
}

QString TModel::runScript( const QString& script )
{
  if( script.isEmpty() ) {
    return QString();
  }
  QScriptValue res = eng->evaluate( script );
  QString r;
  if( eng->hasUncaughtException() ) {
    int line = eng->uncaughtExceptionLineNumber();
    r = "Error: uncaught exception at line " % QSN( line ) % " : \n";
  }
  r += res.toString();
  return r;
}

QString TModel::runFileScript( const QString& sfile )
{
  QString f = QSL( SCRIPT_DIR ":" ) + sfile;
  if( ! QFile::exists( f ) ) {
    return QString::null;
  }

  QFile sf( f );
  if( ! sf.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
    qWarning() << "Fail to open script file " << sfile << WHE;
    return QString::null;
  }

  QByteArray scr = sf.readAll();
  if( scr.isEmpty() ) {
    return QString::null;
  }

  return runScript( scr );
}

QString TModel::runModelScript()
{
  return runScript( script );
}

bool TModel::importScheme( const QString &fn, const QString &schName )
{
  IGNORE_MOD_HERE;
  LaboDoc incDoc;

  if( !incDoc.openDocument( fn ) ) {
    return false;
  }

  TRootData *iroot = incDoc.getRoot();
  if( ! iroot ) {
    return false;
  }
  Scheme *sch = iroot->getObjT<Scheme*>( QSL("model.schems.") % schName );
  if( ! sch ) {
    qWarning() << "Fail to find scheme " << schName << " in " << fn << WHE;
    return false;
  }
  QString s = sch->toString();

  Scheme *old_sch = getObjT<Scheme*>( QSL("schems.") % schName );
  if( old_sch && old_sch->hasAllFlags( efTmp ) ) {
    delObjFromSub( QSL("schems"), schName );
  }

  Scheme *newScheme = addObjToSubT<Scheme>( QSL("schems"), schName );
  if( ! newScheme ) {
    qWarning() << "Fail to add scheme " << schName << " from " << fn << WHE;
    return false;
  }
  if( ! newScheme->fromString( s ) ) {
    qWarning() << "Fail to copy scheme " << schName << " from " << fn << WHE;
    return false;
  }
  reportStructChanged();
  // handleStructChanged();
  newScheme->addFlags( efRO | efNoSave );

  return true;
}

bool TModel::importAllSchemes()
{
  // TODO: remove old imported schemes
  bool only_good = true;
  QStringList incs = imports.cval().split( QSL("\n" ) );
  for( auto s : incs ) {
    QStringList sl = s.split( QSL(":") );
    if( sl.size() < 2 ) {
      qWarning() << "Bad import string: " << s << NWHE;
      only_good = false;
      continue;
    }
    QString fn = QSL( "lib:" ) % sl[0];
    QString schName = sl[1];
    qWarning() << "import: file: " << fn << " scheme: " << schName << NWHE;
    if( ! importScheme( fn, schName ) ) {
      only_good = false;
    }
  }
  return only_good;
}

// must be in correspondence with getSchemeDoublePtr
void TModel::fillComplModelForInputs( QStandardItemModel *mdl ) const
{
  if( ! mdl ) { return; }
  auto root_item = mdl->invisibleRootItem();

  Scheme *sch = getActiveScheme();
  if( sch ) {
    sch->fillComplForInputs( root_item );
  };

  //  simulation params
  Simulation *csim = getActiveSimulation();
  if( csim ) {
    csim->fillComplForInputs( root_item );
  }

  // self vars
  fillComplForInputs( root_item );
}


void TModel::fillComplModelForOuts( QStandardItemModel *mdl ) const
{
  if( ! outs ) { return; }

  for( auto out: outs->TCHILD(TOutArr*) ) {
    auto it = new QStandardItem( out->objectName() );
    mdl->appendRow( it );
  }
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

