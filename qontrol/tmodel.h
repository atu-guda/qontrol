#pragma once
/***************************************************************************
                          tmodel.h  -  description
                             -------------------
    begin                : Tue Aug 1 2000
    copyright            : (C) 2000-2022 by atu
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


#include "defs.h"
#include "contout.h"
#include "contgraph.h"
#include "tmiso.h"
#include "scheme.h"
#include "toutarr.h"
#include "tgraph.h"
#include "simul.h"
#include "askedparam.h"


class QSemaphore;
class QScriptEngine;

//* contains information about the script results
struct ScriptResult {
  int rc;       //* return code = inInt( last action ); in error -> 0
  int err_line; //* line with error: 0=no error
  QString str;  //* result in the string form // what about QScriptValue?
  QString err;  //* Error string
  QStringList bt; //* backtrace
};

/**Contains all elements of model
  *@author atu
  */

class TModel : public LinkedObj  {
  Q_OBJECT
 public:
  DCL_CTOR(TModel);
  virtual ~TModel() override;
  DCL_CREATE;
  DCL_STD_INF;

  //* redirects request to active scheme, simulation, self params
  const double* getSchemeDoublePtr( const QString &nm, int *lt,
        const LinkedObj **src_ob, int lev ) const override;
  // to automatically copy prm[10] to other parameters
  double* getMapDoublePtr( const QString &nm );
  Q_INVOKABLE QString getOutValue( const QString &nm ) const;

  Scheme* getActiveScheme() const { return schems->getObjT<Scheme*>( "main_s" ); }
  //* returns active Simulation
  Simulation* getActiveSimulation() const { return sims->getActiveObjT<Simulation*>(); }


  TOutArr* getOutArr( const QString &oname );
  TGraph*  getGraph( const QString &name );

  // Simulation* getSimul( int idx );
  Simulation* getSimul( const QString &name );
  Scheme* getScheme( const QString &name );

  int startRun();
  int stopRun();
  long run( QSemaphore *sem );
  Q_INVOKABLE long run_bg(); // startRun and stopRun to use w/o iface
  Q_INVOKABLE bool runPreScript();
  // pass to real TGraph
  Q_INVOKABLE void plotToPng( const QString &gname, const QString &fn );

  // for fast access in RunView
  double   get_t() const { return t; }
  long get_i_tot() const { return i_tot; }
  long   get_il1() const { return il1; }
  long   get_il2() const { return il2; }
  QList<AskedParam>& getAskedParams() { return asked_params; };
  void applyAskedParams(); // and save old
  const QStringList& getScriptNames() const { return scriptNames; }
 protected:
  virtual void do_reset() override;
  int runOneLoop( IterType itype );
  void fillAskedParams( const QString &names );
  void restoreAskedParams();
  bool setCurrentSimSchem();
  bool copyParamsFromSim();
  bool addProprsToEngine();

  bool addScriptObject( const QString &nm, QObject *ob );
  bool addScriptFunc(   const QString &nm, QScriptEngine::FunctionSignature fun );

  // interface to commands like above, but with names - to use from JS
 public Q_SLOTS:
  //* adds new out array
  TOutArr* addOut( const  QString &outname, const QString &objname );
  //* delete outs by name, return !=0 id success
  int delOut( const QString &name );

  //* adds new graph @returns: 0 - bad, !=0 Ok
  int addGraph( const QString &gname );
  //* delete graph by name, returns !=0 -  success
  int delGraph( const QString &name );
  //* add output array to graph
  int addOutToGraph( const QString &o_name, const QString &g_name );
  bool cloneGraph( const QString &old_name, const QString &new_name );

  int addSimul( const QString &name );
  int delSimul( const QString &name );
  QString getSimulName( int idx );
  bool cloneSimul( const QString &old_name, const QString &new_name );
  bool setActiveSimul( const QString &name );

  int addScheme( const QString &name );
  int delScheme( const QString &name );
  QString getSchemeName( int idx );
  bool cloneScheme( const QString &old_name, const QString &new_name );

  void initEngine();
  int runScript( const QString& script, ScriptResult *r ); // r may be nullptr
  int runFileScript( const QString& sfile, ScriptResult *r  );
  //* run inner model script
  int runModelScript( ScriptResult *r );

  Q_INVOKABLE bool importScheme( const QString &fn, const QString &schName );
  Q_INVOKABLE bool importAllSchemes();

  virtual void fillComplModelForInputs( QStandardItemModel *mdl ) const override;
  virtual void fillComplModelForOuts( QStandardItemModel *mdl ) const override;


 protected:
  // --------------- convenience ptrs to obligatory elements
  // init not in init list - special handling?
  ContScheme *schems  = nullptr;
  ContOut    *outs    = nullptr;
  ContGraph  *plots   = nullptr;
  ContSimul  *sims    = nullptr;
  Scheme     *main_s  = nullptr;
  // =============== iface objects ==============================
  // copy of simulation vars - just for speed;
  PRM_DOUBLE(     T, efInner, "T", "Total simulation time (copy)", "def=1" );
  PRM_DOUBLE(   t_0, efInner, "t_0", "Start time", "def=100" );
  PRM_DOUBLE( T_brk, efInner, "T_{brk}", "Break time (copy)", "min=0\nmax=1e300\ndef=1e100" );
  PRM_INT(   fakeRT, efInner, "Fake RT", " (copy)", "" );
  /** Initial parametrs values */
  PRM_DOUBLE( prm0s, efInner, "prm0s", "Initial prm0 value", "sep=block" );
  PRM_DOUBLE( prm1s, efInner, "prm1s", "Initial prm1 value", "" );
  PRM_DOUBLE( prm2s, efInner, "prm2s", "Initial prm2 value", ""  );
  PRM_DOUBLE( prm3s, efInner, "prm3s", "Initial prm3 value", ""  );
  PRM_DOUBLE( prm0d, efInner, "prm0+=", "Parameter 0 delta", "sep=col" );
  PRM_DOUBLE( prm1d, efInner, "prm1+=", "Parameter 1 delta", "" );
  // ---------------------------------------
  /** long description */
  PRM_STRING( long_descr, efNRC, "Description", "Model description",
      "props=STRING,MLINE\nncol=-1\nsep=block" );
  PRM_STRING( script, efNRC, "Script", "Model scripts",
      "props=STRING,EXT,LARGETEXT\nfileext=.js" );
  PRM_STRING( scriptpad, efNRC | efNoDial, "script pad", "Script scratch pad",
      "props=STRING,EXT,LARGETEXT\nfileext=.js\ndef=model.lsf();\r//" );
  PRM_SWITCH( autoImport, efNRC, "Auto Import", "Automatically import schemes before run",
      "sep=tab\ntabname=libs");
  PRM_STRING( imports, efNRC, "Imports", "From where to get schemes: file:scheme",
      "props=STRING,MLINE\nncol=-1");
  // ======================= invisible vars ======================
  /** loops counters */
  PRM_LONG( ii, efInner,  "ii", "Inner index", "" );
  PRM_LONG( il1, efInner, "il1", "Param 0 index", "" );
  PRM_LONG( il2, efInner, "il2", "Param 1 index", "" );
  //* copies the current simulation
  /** current time and time step, real time */
  PRM_INT( seed, efInner|efNRC, "Seed", "Seed for random generator" , "min=-1\ndef=RND" );
  /** type of seeding: 0 - every run, 1 - every 1d loop .. obj */
  PRM_INT( seedType, efInner|efNRC, "Seed type", "type of seeding", "" ); // no list - copy
  PRM_DOUBLE( tdt, efInner, "\\tau", "time step", "def=1" ); // just copy for speed
  PRM_DOUBLE( t, efInner, "t", "model time (from t_0)", "" );
  PRM_DOUBLE( t_r, efInner, "t_r", "model relative time (from 0)", "" );
  PRM_DOUBLE( rtime, efInner, "rtime", "real world time", "" );
  /** parameters */
  PRM_DOUBLE( prm0,      efInner, "prm0", "Current prm0 value", "" );
  PRM_DOUBLE( prm1,      efInner, "prm1", "Current prm1 value", "" );
  PRM_DOUBLE( prm2,      efInner, "prm2", "Current prm2 value", "" );
  PRM_DOUBLE( prm3,      efInner, "prm3", "Current prm3 value", "" );
  PRM_DOUBLE( time_pre,  efInner, "time_{pre}", "time for prep state", "" );
  PRM_DOUBLE( time_run,  efInner, "time_{run}", "time for run state", "" );
  PRM_DOUBLE( time_post, efInner, "time_{run}", "time for post state", "" );
  /** signature to check from plot painters, etc... */
  PRM_INT( sgnt, efInner, "sgnt", "signature to check", "" );

  /** total number of iterations */
  PRM_LONG( n_tot, efInner, "n_tot", "total number of iterations", "" );
  /** total counter */
  PRM_LONG( i_tot, efInner, "i_tot", "total counter", "" );
  /** run type */
  int run_type = -1; // reset
  /** effective number of loop on prm0 */
  long n1_eff = 0;
  /** effective number of loop on prm1 */
  long n2_eff = 0;
  /** end loop flag: to be set by fback() */
  int end_loop = 0;
  /** real start time */
  double start_time = 0;
  // copy of simulation vars - but w/o object access - just for speed;
  long N = 10, N1 = 1, N2 = 1;
  int syncRT = 0;
  RunInfo rinf; // information about current run (passed to preRun)

  // saved parameters
  double prm0_save = 0, prm1_save = 1;
  // fake map target
  double fake_map_target = 0;
  // ptr to mapped parameters
  double *prm0_targ = &fake_map_target, *prm1_targ = &fake_map_target;
  QList<AskedParam> asked_params;

  //* current scheme during run, else - 0
  Scheme *c_sch = nullptr;
  //* current simulation during run, else - 0
  Simulation *c_sim = nullptr;

  //* script engine for current model
  QScriptEngine *eng = nullptr;

  QStringList scriptNames;

  Q_CLASSINFO( "nameHintBase",  "model" );
  DCL_DEFAULT_STATIC;

};

// -------------------------- ModelRunner -----------------------------
// class to run TModel async in separate thread

class ModelRunner : public QThread
{
  Q_OBJECT;
  public:
   ModelRunner( TModel *a_model, QSemaphore *a_sem, QObject *parent = nullptr );
   virtual void run() override;
  Q_SIGNALS:
   void resultReady( int rc );
  protected:
   TModel *model;
   QSemaphore *sem;
};


