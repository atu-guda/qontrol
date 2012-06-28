/***************************************************************************
                          tmodel.h  -  description
                             -------------------
    begin                : Tue Aug 1 2000
    copyright            : (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
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

#include "../config.h"
#include "datacont.h"
#include "tmiso.h"
#include "toutarr.h"
#include "tgraph.h"

struct li_el { // structure for links representation in arrays;
  int maxl; // not count, may be gaps
  int l[4]; // -1 -- unused, all else - see xout(int)
  li_el() { // default constructor 
    maxl = 0; l[0] = l[1] = l[2] = l[3] = -1;
  }; 
};

#define CLASS_ID_TModel 500

/**Contains all elements of model
  *@author atu
  */

class TModel : public TDataContainer  {
  Q_OBJECT
 public:
  /** constructor */
  explicit TModel( TDataSet* aparent );
  /** destructor */
  virtual ~TModel();
  /** creator */
  static TDataSet* create( TDataSet* apar );
  /** class id */
  virtual int getClassId(void) const ;
  /** class name - for check & human purpose */
  virtual const char* getClassName(void) const;
  /** return ptr to static class_info, must be implemented in each class */
  virtual const TClassInfo* getClassInfo(void) const;
  /** return ptr to static class_info, static version */
  static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
  /** returns help string */
  virtual const char* getHelp(void) const;
  /** prepare to run */
  virtual int startRun( int type );
  /** run csteps next steps */
  virtual int nextSteps( int csteps );
  /** terminates run: 0 - normal, 1.. - break */
  virtual int stopRun( int reason );
  /** call f() for single element */
  double runOneElem( int elnu, const double *u, double t );
  /** gets input value by number >=0 -- elems, <0 -special */
  virtual double xout( int inu );
  /** gets input value by name. Some names may not have int num, but correct. */
  virtual double xout( const char *iname );
  /** function to call from elem.f() to signal something */
  virtual int fback( int code, int aord, const char *tdescr );
  
  /** converts name of output to it's elnu, if possible; none=-1;bad=-2 ... */
  virtual int oname2elnu( const char *iname ) const;
  /** converts name output array to it's out_nu, bad=-1 ... */
  virtual int  outname2out_nu( const char *iname ) const;
  /** converts elnu to index in ptr or -1 if bad */
  virtual int elnu2idx( int elnu ) const;
  /** converts order of element to it's position in v_el */
  virtual int ord2elnu( int aord ) const;
  /** visual coords -> TMiso number. @returns: <0 - not found, >=0 -- elnu */
  virtual int xy2elnu( int avis_x, int avis_y ); /* must be const. but.. */
  /** returns number of TMiso ( max value of elnu + 1 ) */
  virtual int getNMiso(void) const;
  /** returns ptr to TMiso by elnu */
  virtual TMiso* getMiso( int elnu );
  /** returns number of TOutArr */
  virtual int getNOutArr(void) const;
  /** returns ptr to TOutArr by out_nu */
  virtual TOutArr* getOutArr( int out_nu );
  /** returns number of TGraph */
  virtual int getNGraph(void) const;
  /** returns ptr to TGraph gra_nu */
  virtual TGraph* getGraph( int gra_nu );
  /** inserts active element @returns: <0 - bad, >0= index in ptrs[] */
  virtual int insElem( const TDataInfo* cdi, int aord, int avis_x, int avis_y );
  /** inserts new out array @returns: <0 - bad, >0= index in ptrs[] */
  virtual int insOut( const char *outname, const char *objname );
  /** inserts new graph @returns: <0 - bad, >0= index in ptrs[] */
  virtual int insGraph( const char *gname );
  /** delete active element by name 0 = sucess */
  virtual int delElem( const char *ename );
  /** delete active element by elnu 0 = sucess */
  virtual int delElem( int elnu );
  /** moves element to new position if free */
  int moveElem( int elnu, int newx, int newy );
  /** delete outs by out index 0 = sucess */
  virtual int delOut( int out_nu );
  /** delete graph by graph index 0 = sucess */
  virtual int delGraph( int gr_nu );
  /** new order of element ny name */
  virtual int newOrder( const char *name, int new_ord );
  /** new order of element ny elnu */
  virtual int newOrder( int elnu, int new_ord );
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
  /** access to all vars as array. Its BAD!!! */
  double* getVars(void);
  
  /** reimplemented from TDataSet to ensure all data filled and linked */
  virtual int checkData( int i );
  /** reimplemented from TDataSet for debug purpose */
  virtual int saveDatas( ostream *os );
  
 protected:
  /** sorts {v_el,v_ord} on v_ord */
  void sortOrd(void);
  /** fills common part of v_i & ptrs */
  void fillCommon(void);
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
  PRM_DOUBLE1( tt, efNoRunChange, "T", "Full Run Time", "min=0\nmax=1e300" );
  /** number of inner loop iterations */
  PRM_INT1( nn, efNoRunChange, "N", "Number of steps in one run", 
      "min=1\nmax=200000000\nsep=col"  );
  /** flag for real and model time syncronization */
  PRM_SWITCH1( use_sync, efNoRunChange, "Sync RT", 
      "flag for real and model time syncronization ", "sep=col" );
  /** number of inner parametric loops iterations */
  PRM_INT1( nl1, efNoRunChange, "N1",
       "Number of inner parametric loops iterations", 
       "min=1\nmax=10000\nsep=block" );
  /** number of outer parametric loops iterations */
  PRM_INT1( nl2, efNoRunChange, "N2",
       "Number of outer parametric loops iterations",
       "min=1\nmax=10000\nsep=col" );
  /** number of steps per i/o action */
  PRM_INT1( n_steps, efNoRunChange, "N steps",
      "number of steps per i/o action ",
      "min=1\nmax=100000\nsep=col" );
  /** Initial parametrs values */
  PRM_DOUBLE1( prm0s, efNoRunChange, "param. 0", "Initial prm0 value", "sep=block" );
  PRM_DOUBLE1( prm1s, efNoRunChange, "param. 1", "Initial prm1 value", "" );
  PRM_DOUBLE1( prm2s, efNoRunChange, "param. 2", "Initial prm2 value", ""  );
  PRM_DOUBLE1( prm3s, efNoRunChange, "param. 3", "Initial prm3 value", ""  );
  PRM_DOUBLE1( prm0d, efNoRunChange, "prm0+=", "Parameter 0 delta", "" );
  PRM_DOUBLE1( prm1d, efNoRunChange, "prm1+=", "Parameter 1 delta", "" );
  PRM_DOUBLE1( xval1, 0, "xval1", "Reserved 1", "" );
  PRM_DOUBLE1( xval2, 0, "xval2", "Reserved 2", "" );
  PRM_INT1( seed, efNoRunChange, "Seed", "Seed for random generator" , "min=-1" );
  PRM_INT1( useSameSeed, efNoDial | efRO, "use Same Seed", "", "" ); // OBSOLETE
  /** type of seeding: 0 - every run, 1 - every 1d loop .. obj: 3 - as model */
  PRM_LIST1( seedType, efNoRunChange, "Seed type",
      "type of seeding: 0 - every run... ", "",
      "Every Run\nStart 1d loop\nStart 2d loop" );
  // -------- input channels indexes -------
  /** input from mouse (x,y,btn1,btn2,btn3) abs(x,y) <= 1, 0 - center */
  PRM_INT1( ic_mouse, efNoRunChange, "Mouse idx", "Mouse input index", "sep=col" );
  /** input from joystick (x,y,btn) abs(x,y) <= 1, 0 - center */
  PRM_INT1( ic_joy, efNoRunChange, "Joystick idx", "Joystick input index", "" );
  /** input from soundcard (lert,right) */
  PRM_INT1( ic_sound, efNoRunChange, "Sound idx", "Soundcard input index", "" );
  /** input from keyboard (left,right,top,bottom,space,enter) */
  PRM_INT1( ic_key, efNoRunChange, "Keyboard idx", "Keyboard input index", "" );
  /** input from unknown device, up to 20 values */
  PRM_INT1( ic_aux, efNoRunChange, "AUX idx", "Unknown input index", "" );
  // -------- output channels indexes -------
  /** indexes for output */
  PRM_INT1( oc_0, efNoRunChange, "Out idx 0", "Output index 0", "" );
  PRM_INT1( oc_1, efNoRunChange, "Out idx 1", "Output index 1", ""  );
  PRM_INT1( oc_2, efNoRunChange, "Out idx 2", "Output index 2", ""  );
  PRM_INT1( oc_3, efNoRunChange, "Out idx 3", "Output index 3", ""  );
  PRM_INT1( oc_4, efNoRunChange, "Out idx 4", "Output index 4", ""  );
  PRM_INT1( oc_5, efNoRunChange, "Out idx 5", "Output index 5", ""  );
  /** types of output 1-3=cross, 4-7=vbar, 8-11=gbar, 12-15=leds, 16,17=sound */
  const char *const och_type_str = 
    "None\nCross1\nCross2\nCross3\n"
    "Vbar0\nVbar1\nVbar2\nVbar3\n"
    "Gbar0\nGbar1\nGbar2\nGbar3\n"
    "LED0\nLED1\nLED2\nLED3\n"
    "Sound0\nSound1\n"
    "Aux0\nAux1";
  PRM_LIST1( oct_0, efNoRunChange, "Out Type 0", "Output Type 0", "sep=col", och_type_str );
  PRM_LIST1( oct_1, efNoRunChange, "Out Type 1", "Output Type 1", "", och_type_str  );
  PRM_LIST1( oct_2, efNoRunChange, "Out Type 2", "Output Type 2", "", och_type_str );
  PRM_LIST1( oct_3, efNoRunChange, "Out Type 3", "Output Type 3", "", och_type_str );
  PRM_LIST1( oct_4, efNoRunChange, "Out Type 4", "Output Type 4", "", och_type_str );
  PRM_LIST1( oct_5, efNoRunChange, "Out Type 5", "Output Type 5", "", och_type_str );
  // ---------------------------------------
  /** long description */
  PRM_STRING1( long_descr, 0, "Description", "Model description", 
      "props=STRING,MLINE\nncol=-1\nsep=block");
  // ======================= invisible vars ======================
  /** loops counters */
  PRM_INT1( ii, efInner,  "ii", "Inner index", "" );
  PRM_INT1( il1, efInner, "il1", "Param 0 index", "" );
  PRM_INT1( il2, efInner, "il2", "Param 1 index", "" );
  /** current time and time step, real time */ 
  PRM_DOUBLE1( t, efInner, "time", "model time", "" );
  PRM_DOUBLE1( tdt, efInner, "\\tau", "time step", "" );
  PRM_DOUBLE1( rtime, efInner, "rtime", "real world time", "" );
  /** parametrs */
  PRM_DOUBLE1( prm0, efInner, "prm0", "Current prm0 value", "" );
  PRM_DOUBLE1( prm1, efInner, "prm1", "Current prm1 value", "" );
  PRM_DOUBLE1( prm2, efInner, "prm2", "Current prm2 value", "" );
  PRM_DOUBLE1( prm3, efInner, "prm3", "Current prm3 value", "" );
  /** signature to check from plot painters, etc... */
  PRM_INT1( sgnt, efInner, "sgnt", "signature to check", "" );

  /** total number of loops */
  int n_tot;
  /** total counter */
  int i_tot;
  /** run type */
  int run_type;
  /** effective number of loop on prm0 */
  int n1_eff;
  /** effective number of loop on prm1 */
  int n2_eff;
  /** end loop flag: to be set by fback() */
  int end_loop;
  /** number of active elements */
  int n_el;
  /** number of outputs */
  int n_out;
  /** number of graphs */
  int n_graph;
  /** real start time */
  double start_time;
  /** vector of indexes active elements in ptrs[], my be sorted on ord */
  vector<int> v_el;
  /** vector indexes of outputs */
  vector<int> v_out;
  /** vector elnu for outputs */
  vector<int> v_oute;
  /** vector types of outputs */
  vector<int> v_outt;
  /** vector indexes of graph */
  vector<int> v_graph;
  /** vector of elems orders */
  vector<int> v_ord;
  /** vector of elems flags: 1 - locked,  2 - noauto */
  vector<int> v_flg;
  /** vector of input indexes  */
  vector<li_el> inps;
  /** vector of param. input indexes *[4] */
  vector<li_el> pinps;
  /** vector of param input name indexes *[4] */
  vector<li_el> pnames;
  /** vector of param input flags */
  vector<li_el> pflags;
  /** general purpose vars[MODEL_NVAR] */
  vector<double> vars;
  /** array of element outputs --runtime sz=n_el */
  vector<double> outs;
  /** class decription */
  static TClassInfo class_info;
  /** help str */
  static const char* helpstr;

};

#endif

