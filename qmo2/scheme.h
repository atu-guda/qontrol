/***************************************************************************
                          scheme.h
                             -------------------
    begin                : 2013.09.01
    copyright            : (C) 2013-2014 by atu
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

#include "tmiso.h"
#include "toutarr.h"
#include "simul.h"


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

  const double* getSchemeDoublePtr( const QString &nm, ltype_t *lt,
        const TDataSet **src_ob, int lev ) const override;
  /** prepare to run */
  virtual int startRun( Simulation *a_sim );
  /** run csteps next steps */
  virtual int nextSteps( int csteps );
  /** terminates run: 0 - normal, 1.. - break */
  virtual int stopRun( int reason );
  /** function to call from elem.f() to signal something */
  virtual int fback( int code, int aord, const QString &tdescr );

  /** returns element by its order */
  TMiso* ord2Miso( int aord ) const;
  /** visual coords -> TMiso @returns: nullptr - not found */
  TMiso* xy2Miso( int avis_x, int avis_y ) const;
  //* return max (x,y) of all elements
  QSize getMaxXY() const;
  /** inserts active element @returns: nulltpr - bad  elese -ptr to inserted element */
  TMiso* insElem( const QString &cl_name, const QString &ob_name,
                       int aord, int avis_x, int avis_y );
  /** delete active element by name !0 = sucess */
  virtual int delElem( const QString &ename );
  /** moves element to new position if free */
  int moveElem( const QString &nm, int newx, int newy );
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

  // copy of simulation vars - but w/o onject access - just for speed;
  double T = 1;
  int N = 10, N1 = 1, N2 = 1, syncRT = 0;
  double prm0s = 0, prm1s = 0, prm2s = 0, prm3s = 0;
  double prm0d = 0,  prm1d = 0;
  // caches for fast access
  /** vector of ptrs to active elements, my be sorted on ord */
  std::vector<TMiso*> v_el;

  //* current simulation, valid only while run
  Simulation *sim = nullptr;

  DCL_DEFAULT_STATIC;

};

// ------------------ container of Schemes --------------

class ContScheme : public TDataContainer {
   Q_OBJECT
  public:
   DCL_CTOR(ContScheme);
   virtual ~ContScheme() override;
   DCL_CREATE;
   DCL_STD_INF;
  private:
   DCL_DEFAULT_STATIC;
};


#endif

