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

  enum SeedType {
    everyRun = 0, startLoop, start2DLoop
  };
  Q_ENUMS(SeedType);
  Q_CLASSINFO( "enum_SeedType_0", "Every Run" );               // everyRun
  Q_CLASSINFO( "enum_SeedType_1", "On start on 1D loop" );     // startLoop
  Q_CLASSINFO( "enum_SeedType_2", "On start on 2D loop" );     // start2DLoop

  //* make main work via getDoublePtr
  virtual const double* getSchemeDoublePtr( const QString &nm, ltype_t *lt = nullptr,
        const TDataSet **src_ob = nullptr, int lev = 0 ) const override;

  /** prepare to run */
  virtual int startRun( int type );
  /** run csteps next steps */
  virtual int nextSteps( int csteps );
  /** terminates run: 0 - normal, 1.. - break */
  virtual int stopRun( int reason );
  /** function to call from elem.f() to signal something */
  virtual int fback( int code, int aord, const QString &tdescr );

  /** visual coords -> TMiso number. @returns: <0 - not found, >=0 -- elnu */
  virtual int xy2elnu( int avis_x, int avis_y ) const;
  /** visual coords -> TMiso @returns: nullptr - not found */
  TMiso* xy2Miso( int avis_x, int avis_y ) const;
  /** returns number of TMiso ( max value of elnu + 1 ) */
  virtual int getNMiso() const;
  /** returns ptr to TMiso by elnu */
  virtual TMiso* getMiso( int elnu );
  /** returns number of TOutArr */
  virtual int getNOutArr() const;
  /** returns ptr to TOutArr by out_nu */
  TOutArr* getOutArr( int out_nu );
  /** returns ptr to TOutArr by name */
  TOutArr* getOutArr( const QString &oname );
  /** returns number of TGraph */
  virtual int getNGraph() const;
  /** returns ptr to TGraph gra_nu */
  virtual TGraph* getGraph( int gra_nu );
  /** inserts active element @returns: nulltpr - bad  elese -ptr to inserted element */
  TMiso* insElem( const QString &cl_name, const QString &ob_name,
                       int aord, int avis_x, int avis_y );
  /** inserts new out array @returns: <0 - bad, >0= index in ptrs[] */
  virtual int insOut( const  QString &outname, const QString &objname );
  /** inserts new graph @returns: <0 - bad, >0= index in ptrs[] */
  virtual int insGraph( const QString &gname );
  /** moves element to new position if free */
  int moveElem( int elnu, int newx, int newy );
  /** delete outs by out index 0 = sucess */
  virtual int delOut( int out_nu );
  /** delete graph by graph index 0 = sucess */
  virtual int delGraph( int gr_nu );
  /** new order of element ny name */
  virtual int newOrder( const QString &name, int new_ord );

  Simulation* getSimul( int idx );
  Simulation* getSimul( const QString &name );

  /** suggest order value for new element */
  virtual int hintOrd() const;
  /** frees output arrays and state: 2->1 */
  virtual int reset();

  /** reimplemented from TDataSet to ensure all data filled and linked */
  virtual int checkData( int i );

  // interface to commands like above, but with names - to use from JS
 public slots:
   bool insElem( const QString &tp, const QString &nm, const QString &params );
   bool delElem( const QString &nm );
   bool setElem( const QString &nm, const QString &params );

   int newSimul( const QString &name );
   int delSimul( const QString &name );
   QString getSimulName( int idx );
   int getNSimul() const { return sims->size(); }

 protected:
  /** reimplemented to real Elems, OutArr, Graphs: TODO: separate containers */
  virtual void do_structChanged();
  /** sorts {v_el} on el.ord */
  void sortOrd();
  /** performs one loop */
  virtual int runOneLoop();
  /** fill tables & call preRun for elements */
  virtual int preRun( int run_tp, int anx, int any );
  /** call postRun for elements and dealloc inner buffers */
  virtual int postRun();
  /** calls startLoop for all elms */
  virtual int allStartLoop( int acnx, int acny );
  /** calls endLoop for all elms */
  virtual void allEndLoop();
  /** links input & parm names -> indexses in ptrs */
  virtual int linkNames();
  /** call to allocate out arrays for given type and below */
  virtual void allocOutArrs( int tp );
  /** resets all array with given level or below */
  virtual void resetOutArrs( int level );

 protected:
  // --------------- convinience ptrs to obligatory elements
  // init not in init list - special handling?
  // ContSchem *schs  = nullptr;
  ContOut   *outs  = nullptr;
  ContGraph *plots = nullptr;
  ContSimul *sims = nullptr;
  // =============== iface objects ==============================
  /** total model time, starts with 0 each inner loop */
  PRM_DOUBLE( tt, efNoRunChange, "T", "Full Run Time", "min=0\nmax=1e300\ndef=100" );
  /** number of inner loop iterations */
  PRM_INT( nn, efNoRunChange, "N", "Number of steps in one run",
      "min=1\nmax=200000000\nsep=col\ndef=10000"  );
  /** flag for real and model time syncronization */
  PRM_SWITCH( use_sync, efNoRunChange, "Sync RT",
      "flag for real and model time syncronization ", "sep=col" );
  /** number of inner parametric loops iterations */
  PRM_INT( nl1, efNoRunChange, "N1",
       "Number of inner parametric loops iterations",
       "min=1\nmax=10000\nsep=block\ndef=1" );
  /** number of outer parametric loops iterations */
  PRM_INT( nl2, efNoRunChange, "N2",
       "Number of outer parametric loops iterations",
       "min=1\nmax=10000\nsep=col\ndef=1" );
  /** number of steps per i/o action */
  PRM_INT( n_steps, efNoRunChange, "N steps",
      "number of steps per i/o action ",
      "min=1\nmax=100000\nsep=col\ndef=1000" );
  /** Initial parametrs values */
  PRM_DOUBLE( prm0s, efNoRunChange, "prm0s", "Initial prm0 value", "sep=block" );
  PRM_DOUBLE( prm1s, efNoRunChange, "prm1s", "Initial prm1 value", "" );
  PRM_DOUBLE( prm2s, efNoRunChange, "prm2s", "Initial prm2 value", ""  );
  PRM_DOUBLE( prm3s, efNoRunChange, "prm3s", "Initial prm3 value", ""  );
  PRM_DOUBLE( prm0d, efNoRunChange, "prm0+=", "Parameter 0 delta", "sep=col" );
  PRM_DOUBLE( prm1d, efNoRunChange, "prm1+=", "Parameter 1 delta", "" );
  PRM_DOUBLE( xval1, efOld, "xval1", "Reserved 1", "props=DOUBLE,SPIN\nstep=0.2" );
  PRM_DOUBLE( xval2, efOld, "xval2", "Reserved 2", "" );
  PRM_INT( seed, efNoRunChange, "Seed", "Seed for random generator" , "min=-1\ndef=RND" );
  PRM_INT( useSameSeed, efOld, "use Same Seed - no", "Unused", "props=INT,SPIN\nstep=2\nmin=-100\nmax=100" ); // TODO: OBSOLETE
  /** type of seeding: 0 - every run, 1 - every 1d loop .. obj: 3 - as model */
  PRM_LIST( seedType, efNoRunChange, "Seed type", "type of seeding", "enum=SeedType" );
  PRM_SWITCH( autoStart, efNoRunChange, "auto start",
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
  PRM_STRING( long_descr, 0, "Description", "Model description",
      "props=STRING,MLINE\nncol=-1\nsep=block");
  // ======================= invisible vars ======================
  /** loops counters */
  PRM_INT( ii, efInner,  "ii", "Inner index", "" );
  PRM_INT( il1, efInner, "il1", "Param 0 index", "" );
  PRM_INT( il2, efInner, "il2", "Param 1 index", "" );
  /** current time and time step, real time */
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
  /** constants: todo: separate object */
  PRM_DOUBLE( m_sqrt2, efInner, "sqrt(2)", "\\sqrt{2}", "" );
  PRM_DOUBLE( m_sqrt1_2, efInner, "sqrt(1/2)", "\\sqrt{1/2}", "" );
  PRM_DOUBLE( one, efInner, "1", "1", "" );
  PRM_DOUBLE( m_PI, efInner, "\\Pi", "M_PI", "" );
  PRM_DOUBLE( m_E, efInner, "e", "base of natural logarifm", "" );

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
  // TODO: separated objects
  /** vector of ptrs to active elements, my be sorted on ord */
  std::vector<TMiso*> v_el;
  /** vector indexes of outputs */
  std::vector<TOutArr*> v_out;
  /** vector types of outputs */
  std::vector<int> v_outt;
  /** vector indexes of graph */
  std::vector<TGraph*> v_graph;
  DCL_DEFAULT_STATIC;

};

#endif

