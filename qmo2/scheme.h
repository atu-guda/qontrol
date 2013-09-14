/***************************************************************************
                          scheme.h
                             -------------------
    begin                : 2013.09.01
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

#ifndef SCHEME_H
#define SCHEME_H

#include "defs.h"
#include "datacont.h"
#include "tmiso.h"
#include "toutarr.h"
#include "tgraph.h"


/**Contains elements of scheme
  *@author atu
  */

class Scheme : public TDataContainer  {
  Q_OBJECT
 public:
  DCL_CTOR(Scheme);
  virtual ~Scheme() override;
  DCL_CREATE;
  DCL_STD_INF;

  /** prepare to run */
  virtual int startRun( int type );
  /** run csteps next steps */
  virtual int nextSteps( int csteps );
  /** terminates run: 0 - normal, 1.. - break */
  virtual int stopRun( int reason );
  /** function to call from elem.f() to signal something */
  virtual int fback( int code, int aord, const QString &tdescr );

  /** converts order of element to it's position in v_el */
  virtual int ord2elnu( int aord ) const;
  /** visual coords -> TMiso number. @returns: <0 - not found, >=0 -- elnu */
  virtual int xy2elnu( int avis_x, int avis_y ); /* must be const. but.. */
  /** visual coords -> TMiso @returns: nullptr - not found */
  TMiso* xy2Miso( int avis_x, int avis_y ) const;
  /** returns ptr to TMiso by elnu */
  virtual TMiso* getMiso( int elnu );
  /** returns ptr to TOutArr by name */
  TOutArr* getOutArr( const QString &oname );
  /** inserts active element @returns: nulltpr - bad  elese -ptr to inserted element */
  TMiso* insElem( const QString &cl_name, const QString &ob_name,
                       int aord, int avis_x, int avis_y );
  /** delete active element by name !0 = sucess */
  virtual int delElem( const QString &ename );
  /** moves element to new position if free */
  int moveElem( int elnu, int newx, int newy );
  /** new order of element ny name */
  virtual int newOrder( const QString &name, int new_ord );
  /** suggest order value for new element */
  virtual int hintOrd() const;
  /** resets elements and state: 2->1 */
  virtual int reset();

  /** reimplemented from TDataSet to ensure all data filled and linked */
  virtual int checkData( int i );

 protected:
  /** reimplemented to real Elems, TODO: separate containers */
  virtual void do_structChanged();
  /** sorts elements on its order */
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
  /** links input & parm names -> indexses in ptrs TODO: drop */
  virtual int linkNames();

 protected:
  // =============== iface objects ==============================
  // most of them - copy copy of current model or param blocks
  /** total model time, starts with 0 each inner loop */
  PRM_DOUBLE( t_full, efInner, "t_full", "Full Run Time", "min=0\nmax=1e300\ndef=100" );
  /** number of inner loop iterations */
  PRM_INT( nn, efInner, "nn", "Number of steps in one run",
      "min=1\nmax=200000000\nsep=col\ndef=10000"  );
  /** flag for real and model time syncronization */
  PRM_SWITCH( use_sync, efInner, "Sync RT",
      "flag for real and model time syncronization ", "sep=col" );
  /** number of inner parametric loops iterations */
  PRM_INT( nl1, efInner, "nl1",
       "Number of inner parametric loops iterations",
       "min=1\nmax=10000\nsep=block\ndef=1" );
  /** number of outer parametric loops iterations */
  PRM_INT( nl2, efInner, "N2",
       "Number of outer parametric loops iterations",
       "min=1\nmax=10000\nsep=col\ndef=1" );
  /** Initial parametrs values */
  PRM_DOUBLE( prm0s, efInner, "param. 0", "Initial prm0 value", "sep=block" );
  PRM_DOUBLE( prm1s, efInner, "param. 1", "Initial prm1 value", "" );
  PRM_DOUBLE( prm2s, efInner, "param. 2", "Initial prm2 value", ""  );
  PRM_DOUBLE( prm3s, efInner, "param. 3", "Initial prm3 value", ""  );
  PRM_INT( seed, efInner, "Seed", "Seed for random generator" , "min=-1\ndef=RND" );
  /** type of seeding: 0 - every run, 1 - every 1d loop .. obj: 3 - as model */
  PRM_LIST( seedType, efInner, "Seed type",
      "type of seeding: 0 - every run... ", "",
      "Every Run\nStart 1d loop\nStart 2d loop" );
  // ---------------------------------------
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
  /** constants: TODO: separate object */
  PRM_DOUBLE( m_sqrt2, efInner, "sqrt(2)", "\\sqrt{2}", "" );
  PRM_DOUBLE( m_sqrt1_2, efInner, "sqrt(1/2)", "\\sqrt{1/2}", "" );
  PRM_DOUBLE( one, efInner, "1", "1", "" );
  PRM_DOUBLE( m_PI, efInner, "\\Pi", "M_PI", "" );
  PRM_DOUBLE( m_E, efInner, "e", "base of natural logariphm", "" );

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
  /** vector of elems orders */
  std::vector<int> v_ord;
  DCL_DEFAULT_STATIC;

};

#endif

