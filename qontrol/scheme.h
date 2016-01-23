#pragma once
/***************************************************************************
                          scheme.h
                             -------------------
    begin                : 2013.09.01
    copyright            : (C) 2013-2016 by atu
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


#include <vector>

class TMiso;
#include "linkedobj.h"

/**Contains elements of scheme
  *@author atu
  */

class Scheme : public LinkedObj  {
  Q_OBJECT
 public:
  DCL_CTOR(Scheme);
  // virtual ~Scheme() override;
  DCL_CREATE;
  DCL_STD_INF;

  virtual QVariant dataObj( int col, int role = Qt::DisplayRole ) const override;

  const double* getSchemeDoublePtr( const QString &nm, int *lt,
        const LinkedObj **src_ob, int lev ) const override;
  /** function to call from elem.f() to signal something */
  int fback( int code, TMiso *obj );
  virtual int do_preRun() override;

  /** returns element by its order */
  TMiso* ord2Miso( int aord ) const;
  /** visual coords -> TMiso @returns: nullptr - not found */
  TMiso* xy2Miso( int avis_x, int avis_y ) const;
  //* return max (x,y) of all elements
  QSize getMaxXY() const;
  //* return number of elements of TMiso-alike
  Q_INVOKABLE int getNMiso() const { return v_el.size(); }
  /** inserts active element @returns: nullptr - bad  else - ptr to inserted element */
  TMiso* addElem( const QString &cl_name, const QString &ob_name,
                       int aord, int avis_x, int avis_y );
  /** delete active element by name !0 = sucess */
  Q_INVOKABLE int delElem( const QString &ename );
  /** moves element to new position if free */
  Q_INVOKABLE int moveElem( const QString &nm, int newx, int newy );
  /** new order of element ny name */
  Q_INVOKABLE int newOrder( const QString &name, int new_ord );
  /** suggest order value for new element */
  Q_INVOKABLE int hintOrd() const;

  /** reimplemented to real Elems */
  virtual void do_structChanged();
  /** sorts elements on its order */
  void sortOrd();
  /** performs one loop */
  virtual int runOneLoop( IterType itype );

  virtual void fillComplModelForInputs( QStandardItemModel *mdl ) const override;

 protected:
  virtual void do_reset() override;


  PRM_STRING( descr, efNoRunChange, "description", "Scheme description", "max=1024\nprops=STRING,MLINE\nncol=-1\nsep=blockend");
  PRM_STRING( breakObjNm, efInner, "Break objname", "Name of object, which call fback", "");
  // ======================= invisible vars ======================
  /** run type */
  int run_type = -1; // reset
  /** end loop flag: to be set by fback() */
  int end_loop = 0;

  // caches for fast access
  /** vector of ptrs to active elements, my be sorted on ord */
  std::vector<TMiso*> v_el;

  TMiso *obj_brk = nullptr;

  Q_CLASSINFO( "nameHintBase",  "sch_" );
  DCL_DEFAULT_STATIC;

};

// ------------------ container of Schemes --------------

class ContScheme : public LinkedObj {
   Q_OBJECT
  public:
   DCL_CTOR(ContScheme);
   // virtual ~ContScheme() override;
   DCL_CREATE;
   DCL_STD_INF;
  private:
   DCL_DEFAULT_STATIC;
};



