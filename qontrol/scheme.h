/***************************************************************************
                          scheme.h
                             -------------------
    begin                : 2013.09.01
    copyright            : (C) 2013-2015 by atu
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
  /** function to call from elem.f() to signal something */
  virtual int fback( int code, int aord, const QString &tdescr );

  /** returns element by its order */
  TMiso* ord2Miso( int aord ) const;
  /** visual coords -> TMiso @returns: nullptr - not found */
  TMiso* xy2Miso( int avis_x, int avis_y ) const;
  //* return max (x,y) of all elements
  QSize getMaxXY() const;
  //* return number of elements of TMiso-alike
  Q_INVOKABLE int getNMiso() const { return v_el.size(); }
  /** inserts active element @returns: nullptr - bad  else - ptr to inserted element */
  TMiso* insElem( const QString &cl_name, const QString &ob_name,
                       int aord, int avis_x, int avis_y );
  /** delete active element by name !0 = sucess */
  Q_INVOKABLE int delElem( const QString &ename );
  /** moves element to new position if free */
  Q_INVOKABLE int moveElem( const QString &nm, int newx, int newy );
  /** new order of element ny name */
  Q_INVOKABLE int newOrder( const QString &name, int new_ord );
  /** suggest order value for new element */
  Q_INVOKABLE int hintOrd() const;

  /** reimplemented from TDataSet to ensure all data filled and linked */
  virtual int checkData( int i );

  /** reimplemented to real Elems, TODO: separate containers */
  virtual void do_structChanged();
  /** sorts elements on its order */
  void sortOrd();
  /** performs one loop */
  virtual int runOneLoop( double t, IterType itype );
  /** fill tables & call preRun for elements */
  virtual int preRun( int run_tp, int N, int anx, int any, double tdt );
  /** call postRun for elements and dealloc inner buffers */
  virtual int postRun();
  /** calls startLoop for all elms */
  virtual int allStartLoop( int acnx, int acny );
  /** calls endLoop for all elms */
  virtual void allEndLoop();
  /** links input & parm names -> indexses in ptrs TODO: drop */
  virtual int linkNames();

 protected:
  virtual void do_reset() override;
  // ======================= invisible vars ======================
  /** run type */
  int run_type = -1; // reset
  /** end loop flag: to be set by fback() */
  int end_loop = 0;

  // caches for fast access
  /** vector of ptrs to active elements, my be sorted on ord */
  std::vector<TMiso*> v_el;

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

