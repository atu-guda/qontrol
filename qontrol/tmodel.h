/***************************************************************************
                          tmodel.h  -  description
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

#ifndef TMODEL_H
#define TMODEL_H

#include "defs.h"
#include "contout.h"
#include "contgraph.h"
#include "tmiso.h"
#include "scheme.h"
#include "toutarr.h"
#include "tgraph.h"
#include "simul.h"


class QSemaphore;
class QScriptEngine;

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

  //* redirects requuest to active scheme (may be more)
  const double* getSchemeDoublePtr( const QString &nm, int *lt,
        const LinkedObj **src_ob, int lev ) const override;
  // to automaticaly copy prm[10] to other parameters
  double* getMapDoublePtr( const QString &nm );
  Q_INVOKABLE QString getOutValue( const QString &nm ) const;

  Scheme* getActiveScheme() const { return schems->getObjT<Scheme*>( "main_s" ); };
  //* returns active Simulation
  Simulation* getActiveSimulation() const { return sims->getActiveObjT<Simulation*>(); };


  /** returns ptr to TOutArr by name */
  TOutArr* getOutArr( const QString &oname );
  /** returns ptr to TGraph by name */
  TGraph* getGraph( const QString &name );

  // Simulation* getSimul( int idx );
  Simulation* getSimul( const QString &name );
  Scheme* getScheme( const QString &name );

  int startRun();
  int stopRun( int reason );
  int run( QSemaphore *sem );
  Q_INVOKABLE int run_bg(); // startRun and stopRun to use w/o iface
  // pass to real TGraph
  Q_INVOKABLE void plotToPng( const QString &gname, const QString &fn );

  // for fast access in RunView
  double get_t() const { return t; }
  int get_i_tot() const { return i_tot; }
  int get_il1() const { return il1; }
  int get_il2() const { return il2; }
 protected:
  virtual void do_reset() override;
  int runOneLoop( IterType itype );
  int postRun();
  int allStartLoop( int acnx, int acny );
  void allEndLoop( int acnx, int acny );

  // interface to commands like above, but with names - to use from JS
 public slots:
  //* adderts new out array
  TOutArr* addOut( const  QString &outname, const QString &objname );
  //* delete outs by name, return !=0 id success
  int delOut( const QString &name );

  //* adds new graph @returns: 0 - bad, !=0 Ok
  int addGraph( const QString &gname );
  //* delete graph by name, retrns !=0 -  success
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
  QString runScript( const QString& script );
  QString runFileScript( const QString& sfile );
  //* run inner model script
  QString runModelScript();

  virtual void fillComplModelForInputs( QStandardItemModel *mdl ) const override;
  virtual void fillComplModelForOuts( QStandardItemModel *mdl ) const override;


 protected:
  // --------------- convinience ptrs to obligatory elements
  // init not in init list - special handling?
  ContScheme *schems  = nullptr;
  ContOut   *outs  = nullptr;
  ContGraph *plots = nullptr;
  ContSimul *sims = nullptr;
  Scheme* main_s = nullptr;
  // =============== iface objects ==============================
  // copy of simulation vars - but w/o onject access - just for speed;
  PRM_DOUBLE(     T, efInner, "T", "Total simulation time", "def=1" );
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
      "props=STRING,MLINE\nncol=-1\nsep=block");
  PRM_STRING( script, efNRC, "Script", "Model scripts",
      "props=STRING,EXT,LARGETEXT");
  // ======================= invisible vars ======================
  /** loops counters */
  PRM_INT( ii, efInner,  "ii", "Inner index", "" );
  PRM_INT( il1, efInner, "il1", "Param 0 index", "" );
  PRM_INT( il2, efInner, "il2", "Param 1 index", "" );
  //* copies the current simulation
  /** current time and time step, real time */
  PRM_INT( seed, efInner|efNoRunChange, "Seed", "Seed for random generator" , "min=-1\ndef=RND" );
  /** type of seeding: 0 - every run, 1 - every 1d loop .. obj */
  PRM_INT( seedType, efInner|efNoRunChange, "Seed type", "type of seeding", "" ); // no list - copy
  PRM_DOUBLE( t, efInner, "time", "model time", "" );
  PRM_DOUBLE( tdt, efInner, "\\tau", "time step", "" );
  PRM_DOUBLE( rtime, efInner, "rtime", "real world time", "" );
  /** parametrs */
  PRM_DOUBLE( prm0, efInner, "prm0", "Current prm0 value", "" );
  PRM_DOUBLE( prm1, efInner, "prm1", "Current prm1 value", "" );
  PRM_DOUBLE( prm2, efInner, "prm2", "Current prm2 value", "" );
  PRM_DOUBLE( prm3, efInner, "prm3", "Current prm3 value", "" );
  /** signature to check from plot painters, etc... */
  PRM_INT( sgnt, efInner, "sgnt", "signature to check", "" );

  /** total number of iterations */
  PRM_INT( n_tot, efInner, "n_tot", "total number of iterations", "" );
  /** total counter */
  PRM_INT( i_tot, efInner, "i_tot", "total counter", "" );
  /** run type */
  int run_type = -1; // reset
  /** effective number of loop on prm0 */
  int n1_eff = 0;
  /** effective number of loop on prm1 */
  int n2_eff = 0;
  /** end loop flag: to be set by fback() */
  int end_loop = 0;
  /** real start time */
  double start_time = 0;
  // copy of simulation vars - but w/o onject access - just for speed;
  int N = 10, N1 = 1, N2 = 1, syncRT = 0, n_iosteps = 1;

  // saved parameters
  double prm0_save = 0, prm1_save = 1;
  // fake map target
  double fake_map_target = 0;
  // ptr to mapped params
  double *prm0_targ = &fake_map_target, *prm1_targ = &fake_map_target;

  //* current scheme during run, else - 0
  Scheme *c_sch = nullptr;
  //* current simulation during run, else - 0
  Simulation *c_sim = nullptr;

  //* script engine for currne model
  QScriptEngine *eng = nullptr;

  Q_CLASSINFO( "nameHintBase",  "model" );
  DCL_DEFAULT_STATIC;

};

// -------------------------- ModelRunner -----------------------------
// class to run TModel async in sepatate thread

class ModelRunner : public QThread
{
  Q_OBJECT;
  public:
   ModelRunner( TModel *a_model, QSemaphore *a_sem, QObject *parent = nullptr );
   virtual void run() override;
  signals:
   void resultReady( int rc );
  protected:
   TModel *model;
   QSemaphore *sem;
};

#endif

