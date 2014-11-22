/***************************************************************************
                          tmodel.h  -  description
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


/**Contains all elements of model
  *@author atu
  */

class TModel : public TDataContainer  {
  Q_OBJECT
 public:
  DCL_CTOR(TModel);
  virtual ~TModel() override;
  DCL_CREATE;
  DCL_STD_INF;

  //* redirects requuest to active scheme (may be more)
  const double* getSchemeDoublePtr( const QString &nm, ltype_t *lt,
        const TDataSet **src_ob, int lev ) const override;

  //* returns active (main) Scheme
  Scheme* getActiveScheme() const { return schems->getActiveElemT<Scheme*>(); };
  //* returns active Simulation
  Simulation* getActiveSimulation() const { return sims->getActiveElemT<Simulation*>(); };


  /** returns ptr to TOutArr by name */
  TOutArr* getOutArr( const QString &oname );
  /** returns ptr to TGraph by name */
  TGraph* getGraph( const QString &name );

  // Simulation* getSimul( int idx );
  Simulation* getSimul( const QString &name );

  /** frees output arrays and state: 2->1 */
  int reset();
  int startRun();
  int nextSteps( int csteps );
  int stopRun( int reason );
 protected:
  int runOneLoop(); // TODO: to protected
  int postRun();
  int allStartLoop( int acnx, int acny );
  void allEndLoop();
  /** call to allocate out arrays for given type and below */
  void allocOutArrs( int tp );
  /** resets all array with given level or below */
  void resetOutArrs( int level );

  // interface to commands like above, but with names - to use from JS
 public slots:
  int getNOuts() const { return outs->size(); }
  //* inserts new out array @returns: 0 - bad, !=0 - Ok
  int insOut( const  QString &outname, const QString &objname );
  //* delete outs by name, return !=0 id success
  int delOut( const QString &name );

  //* return number of plots
  int getNGraph() const { return plots->size(); }
  //* inserts new graph @returns: 0 - bad, !=0 Ok
  int insGraph( const QString &gname );
  //* delete graph by name, retrns !=0 -  success
  int delGraph( const QString &name );

  int newSimul( const QString &name );
  int delSimul( const QString &name );
  QString getSimulName( int idx );
  int getNSimul() const { return sims->size(); }

  int getNSchems() const { return schems->size(); }


 protected:
  // --------------- convinience ptrs to obligatory elements
  // init not in init list - special handling?
  ContScheme *schems  = nullptr;
  ContOut   *outs  = nullptr;
  ContGraph *plots = nullptr;
  ContSimul *sims = nullptr;
  Scheme* main_s = nullptr;
  // =============== iface objects ==============================
  /** total model time, starts with 0 each inner loop */
  PRM_DOUBLE( tt, efOld|efNoRunChange, "T", "Full Run Time", "min=0\nmax=1e300\ndef=100" );
  /** number of inner loop iterations */
  PRM_INT( nn, efOld|efNoRunChange, "N", "Number of steps in one run",
      "min=1\nmax=200000000\nsep=col\ndef=10000"  );
  /** flag for real and model time syncronization */
  PRM_SWITCH( use_sync, efOld|efNoRunChange, "Sync RT",
      "flag for real and model time syncronization ", "sep=col" );
  /** number of inner parametric loops iterations */
  PRM_INT( nl1, efOld|efNoRunChange, "N1",
       "Number of inner parametric loops iterations",
       "min=1\nmax=10000\nsep=block\ndef=1" );
  /** number of outer parametric loops iterations */
  PRM_INT( nl2, efOld|efNoRunChange, "N2",
       "Number of outer parametric loops iterations",
       "min=1\nmax=10000\nsep=col\ndef=1" );
  /** number of steps per i/o action */
  PRM_INT( n_steps, efOld|efNoRunChange, "N steps",
      "number of steps per i/o action ",
      "min=1\nmax=100000\nsep=col\ndef=1000" );
  /** Initial parametrs values */
  PRM_DOUBLE( prm0s, efOld|efNoRunChange, "prm0s", "Initial prm0 value", "sep=block" );
  PRM_DOUBLE( prm1s, efOld|efNoRunChange, "prm1s", "Initial prm1 value", "" );
  PRM_DOUBLE( prm2s, efOld|efNoRunChange, "prm2s", "Initial prm2 value", ""  );
  PRM_DOUBLE( prm3s, efOld|efNoRunChange, "prm3s", "Initial prm3 value", ""  );
  PRM_DOUBLE( prm0d, efOld|efNoRunChange, "prm0+=", "Parameter 0 delta", "sep=col" );
  PRM_DOUBLE( prm1d, efOld|efNoRunChange, "prm1+=", "Parameter 1 delta", "" );
  PRM_DOUBLE( xval1, efOld, "xval1", "Reserved 1", "props=DOUBLE,SPIN\nstep=0.2" );
  PRM_DOUBLE( xval2, efOld, "xval2", "Reserved 2", "" );
  PRM_INT( useSameSeed, efOld|efOld, "use Same Seed - no", "Unused", "props=INT,SPIN\nstep=2\nmin=-100\nmax=100" ); // TODO: OBSOLETE
  PRM_SWITCH( autoStart, efOld|efNoRunChange, "auto start",
      "Start simulation without key awaitng", "sep=col" );
  // -------- input channels indexes -------
  /** input from mouse (x,y,btn1,btn2,btn3) abs(x,y) <= 1, 0 - center */
  PRM_INT( ic_mouse, efOld, "Mouse idx", "Mouse input index", "sep=col" );
  /** input from joystick (x,y,btn) abs(x,y) <= 1, 0 - center */
  PRM_INT( ic_joy, efOld, "Joystick idx", "Joystick input index", "" );
  /** input from soundcard (lert,right) */
  PRM_INT( ic_sound, efOld, "Sound idx", "Soundcard input index", "" );
  /** input from keyboard (left,right,top,bottom,space,enter) */
  PRM_INT( ic_key, efOld, "Keyboard idx", "Keyboard input index", "" );
  /** input from unknown device, up to 20 values */
  PRM_INT( ic_aux, efOld, "AUX idx", "Unknown input index", "" );
  // -------- output channels indexes -------
  /** indexes for output: obsoleted */
  PRM_INT( oc_0, efOld, "Out idx 0", "Output index 0", "" );
  PRM_INT( oc_1, efOld, "Out idx 1", "Output index 1", ""  );
  PRM_INT( oc_2, efOld, "Out idx 2", "Output index 2", ""  );
  PRM_INT( oc_3, efOld, "Out idx 3", "Output index 3", ""  );
  PRM_INT( oc_4, efOld, "Out idx 4", "Output index 4", ""  );
  PRM_INT( oc_5, efOld, "Out idx 5", "Output index 5", ""  );
  /** types of output: obsoleted  */
  PRM_INT( oct_0, efOld, "Out Type 0", "Output Type 0", "" );
  PRM_INT( oct_1, efOld, "Out Type 1", "Output Type 1", "" );
  PRM_INT( oct_2, efOld, "Out Type 2", "Output Type 2", "" );
  PRM_INT( oct_3, efOld, "Out Type 3", "Output Type 3", "" );
  PRM_INT( oct_4, efOld, "Out Type 4", "Output Type 4", "" );
  PRM_INT( oct_5, efOld, "Out Type 5", "Output Type 5", "" );
  // ---------------------------------------
  /** long description */
  PRM_STRING( long_descr, efOld, "Description", "Model description",
      "props=STRING,MLINE\nncol=-1\nsep=block");
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
  double start_time;
  // copy of simulation vars - but w/o onject access - just for speed;
  double T = 1;
  int N = 10, N1 = 1, N2 = 1, syncRT = 0;
  // TODO: uncomment after migration
  // double prm0s = 0, prm1s = 0, prm2s = 0, prm3s = 0;
  // double prm0d = 0,  prm1d = 0;

  //* current scheme during run, else - 0
  Scheme *c_sch = nullptr;
  //* current simulation during run, else - 0
  Simulation *c_sim = nullptr;
  DCL_DEFAULT_STATIC;

};

#endif

