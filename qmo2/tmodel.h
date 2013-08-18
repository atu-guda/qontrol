/***************************************************************************
                          tmodel.h  -  description
                             -------------------
    begin                : Tue Aug 1 2000
    copyright            : (C) 2000-2013 by atu
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

#include <vector>

#include "defs.h"
#include "datacont.h"
#include "tmiso.h"
#include "toutarr.h"
#include "tgraph.h"


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

  /** reimplemented from TDataSet to provide access to '#nvar' TODO: drop*/
  virtual const double* getDoublePtr( const QString &nm, ltype_t *lt = 0, int lev = 0 ) const;
  /** prepare to run */
  virtual int startRun( int type );
  /** run csteps next steps */
  virtual int nextSteps( int csteps );
  /** terminates run: 0 - normal, 1.. - break */
  virtual int stopRun( int reason );
  /** function to call from elem.f() to signal something */
  virtual int fback( int code, int aord, const QString &tdescr );
  
  /** converts name of output to it's elnu, if possible; none=-1;bad=-2 ... */
  virtual int oname2elnu( const QString &iname ) const;
  /** converts name output array to it's out_nu, bad=-1 ... */
  virtual int  outname2out_nu( const QString &iname ) const;
  /** converts order of element to it's position in v_el */
  virtual int ord2elnu( int aord ) const;
  /** visual coords -> TMiso number. @returns: <0 - not found, >=0 -- elnu */
  virtual int xy2elnu( int avis_x, int avis_y ); /* must be const. but.. */
  /** visual coords -> TMiso @returns: nullptr - not found */
  TMiso* xy2Miso( int avis_x, int avis_y ) const; 
  /** returns number of TMiso ( max value of elnu + 1 ) */
  virtual int getNMiso(void) const;
  /** returns ptr to TMiso by elnu */
  virtual TMiso* getMiso( int elnu );
  /** returns number of TOutArr */
  virtual int getNOutArr(void) const;
  /** returns ptr to TOutArr by out_nu */
  TOutArr* getOutArr( int out_nu );
  /** returns ptr to TOutArr by name */
  TOutArr* getOutArr( const QString &oname );
  /** returns number of TGraph */
  virtual int getNGraph(void) const;
  /** returns ptr to TGraph gra_nu */
  virtual TGraph* getGraph( int gra_nu );
  /** inserts active element @returns: nulltpr - bad  elese -ptr to inserted element */
  TMiso* insElem( const QString &cl_name, const QString &ob_name,  
                       int aord, int avis_x, int avis_y );
  /** inserts new out array @returns: <0 - bad, >0= index in ptrs[] */
  virtual int insOut( const  QString &outname, const QString &objname );
  /** inserts new graph @returns: <0 - bad, >0= index in ptrs[] */
  virtual int insGraph( const QString &gname );
  /** delete active element by name !0 = sucess */
  virtual int delElem( const QString &ename );
  /** moves element to new position if free */
  int moveElem( int elnu, int newx, int newy );
  /** delete outs by out index 0 = sucess */
  virtual int delOut( int out_nu );
  /** delete graph by graph index 0 = sucess */
  virtual int delGraph( int gr_nu );
  /** new order of element ny name */
  virtual int newOrder( const QString &name, int new_ord );
  /** suggest order value for new element */
  virtual int hintOrd(void) const;
  /** fills (8=2*4) array of LinkInfo structures for link visualization ... */
  virtual int getLinkInfos( int elnu, LinkInfo *li );
  /** frees output arrays and state: 2->1 */
  virtual int reset(void);
  /** read access to vars[] */
  double getVar( int n );
  /** write access to vars[] */
  void setVar( int n, double v );
  /** access to all vars as array. Its BAD!!!  remove! */
  dvector* getVars(void);
  
  /** reimplemented from TDataSet to ensure all data filled and linked */
  virtual int checkData( int i );
  /** reimplemented to allow add TMiso.. TOutArr... TGraph TODO: containers */
  virtual int isValidType( const QString &cl_name ) const;
  
 protected:
  /** reimplemented to real Elems, OutArr, Graphs: TODO: separate containers */
  virtual void do_structChanged();
  /** sorts {v_el,v_ord} on v_ord */
  void sortOrd(void);
  /** performs one loop */
  virtual int runOneLoop(void);
  /** fill tables & call preRun for elements */
  virtual int preRun( int run_tp, int anx, int any );
  /** call postRun for elements and dealloc inner buffers */
  virtual int postRun(void);
  /** calls startLoop for all elms */
  virtual int allStartLoop( int acnx, int acny );
  /** calls endLoop for all elms */
  virtual void allEndLoop(void);
  /** links input & parm names -> indexses in ptrs */
  virtual int linkNames(void);
  /** call to allocate out arrays for given type and below */
  virtual void allocOutArrs( int tp );
  /** resets all array with given level or below */
  virtual void resetOutArrs( int level );
  
 protected:
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
  PRM_DOUBLE( prm0s, efNoRunChange, "param. 0", "Initial prm0 value", "sep=block" );
  PRM_DOUBLE( prm1s, efNoRunChange, "param. 1", "Initial prm1 value", "" );
  PRM_DOUBLE( prm2s, efNoRunChange, "param. 2", "Initial prm2 value", ""  );
  PRM_DOUBLE( prm3s, efNoRunChange, "param. 3", "Initial prm3 value", ""  );
  PRM_DOUBLE( prm0d, efNoRunChange, "prm0+=", "Parameter 0 delta", "" );
  PRM_DOUBLE( prm1d, efNoRunChange, "prm1+=", "Parameter 1 delta", "" );
  PRM_DOUBLE( xval1, 0, "xval1", "Reserved 1", "props=DOUBLE,SPIN\nstep=0.2" );
  PRM_DOUBLE( xval2, 0, "xval2", "Reserved 2", "" );
  PRM_INT( seed, efNoRunChange, "Seed", "Seed for random generator" , "min=-1\ndef=117" );
  PRM_INT( useSameSeed, efNoRunChange, "use Same Seed - no", "Unused", "props=INT,SPIN\nstep=2\nmin=-100\nmax=100" ); // TODO: OBSOLETE
  /** type of seeding: 0 - every run, 1 - every 1d loop .. obj: 3 - as model */
  PRM_LIST( seedType, efNoRunChange, "Seed type",
      "type of seeding: 0 - every run... ", "",
      "Every Run\nStart 1d loop\nStart 2d loop" );
  // -------- input channels indexes -------
  /** input from mouse (x,y,btn1,btn2,btn3) abs(x,y) <= 1, 0 - center */
  PRM_INT( ic_mouse, efNoRunChange, "Mouse idx", "Mouse input index", "sep=col" );
  /** input from joystick (x,y,btn) abs(x,y) <= 1, 0 - center */
  PRM_INT( ic_joy, efNoRunChange, "Joystick idx", "Joystick input index", "" );
  /** input from soundcard (lert,right) */
  PRM_INT( ic_sound, efNoRunChange, "Sound idx", "Soundcard input index", "" );
  /** input from keyboard (left,right,top,bottom,space,enter) */
  PRM_INT( ic_key, efNoRunChange, "Keyboard idx", "Keyboard input index", "" );
  /** input from unknown device, up to 20 values */
  PRM_INT( ic_aux, efNoRunChange, "AUX idx", "Unknown input index", "" );
  // -------- output channels indexes -------
  /** indexes for output */
  PRM_INT( oc_0, efNoRunChange, "Out idx 0", "Output index 0", "" );
  PRM_INT( oc_1, efNoRunChange, "Out idx 1", "Output index 1", ""  );
  PRM_INT( oc_2, efNoRunChange, "Out idx 2", "Output index 2", ""  );
  PRM_INT( oc_3, efNoRunChange, "Out idx 3", "Output index 3", ""  );
  PRM_INT( oc_4, efNoRunChange, "Out idx 4", "Output index 4", ""  );
  PRM_INT( oc_5, efNoRunChange, "Out idx 5", "Output index 5", ""  );
  /** types of output 1-3=cross, 4-7=vbar, 8-11=gbar, 12-15=leds, 16,17=sound */
  const char *const och_type_str = 
    "None\nCross1\nCross2\nCross3\n"
    "Vbar0\nVbar1\nVbar2\nVbar3\n"
    "Gbar0\nGbar1\nGbar2\nGbar3\n"
    "LED0\nLED1\nLED2\nLED3\n"
    "Sound0\nSound1\n"
    "Aux0\nAux1";
  PRM_LIST( oct_0, efNoRunChange, "Out Type 0", "Output Type 0", "sep=col", och_type_str );
  PRM_LIST( oct_1, efNoRunChange, "Out Type 1", "Output Type 1", "", och_type_str  );
  PRM_LIST( oct_2, efNoRunChange, "Out Type 2", "Output Type 2", "", och_type_str );
  PRM_LIST( oct_3, efNoRunChange, "Out Type 3", "Output Type 3", "", och_type_str );
  PRM_LIST( oct_4, efNoRunChange, "Out Type 4", "Output Type 4", "", och_type_str );
  PRM_LIST( oct_5, efNoRunChange, "Out Type 5", "Output Type 5", "", och_type_str );
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
  /** vector of elems orders */
  std::vector<int> v_ord;
  /** general purpose vars[MODEL_NVAR] */
  dvector vars;
  DCL_DEFAULT_STATIC;

};

#endif

