#pragma once
/***************************************************************************
                          scheme.h
                             -------------------
    begin                : 2013.09.01
    copyright            : (C) 2013-2022 by atu
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

  /** visual coords -> TMiso @returns: nullptr - not found */
  TMiso* xy2Miso( int avis_x, int avis_y ) const;
  //* return max (x,y) of all elements
  QSize getMaxXY() const;
  //* return number of elements of TMiso-alike
  Q_INVOKABLE int getNMiso() const { return v_el.size(); }
  /** inserts active element, not for iface, just for JS */
  Q_INVOKABLE bool addElem( const QString &cl_name, const QString &ob_name, int avis_x, int avis_y );
  /** delete active element by name !0 = sucess */
  Q_INVOKABLE int delElem( const QString &ename );
  /** moves element to new position if free */
  Q_INVOKABLE int moveElem( const QString &nm, int newx, int newy );


  virtual void do_structChanged() override;
  /** performs one loop */
  virtual int runOneLoop( IterType itype );

  virtual void fillComplModelForInputs( QStandardItemModel *mdl ) const override;

 protected:
  virtual void do_reset() override;
  virtual int do_preRun() override;
  // virtual int do_startLoop( long acnx, long acny ) override; // = parent
  virtual int post_startLoop() override; // for preCalc


  PRM_STRING( descr, efNRC, "description", "Scheme description", "max=1024\nprops=STRING,MLINE\nncol=-1\nsep=tabend\ntabname=objs");
  PRM_STRING( breakObjNm, efInner, "Break objname", "Name of object, which called fback", "");
  // ======================= invisible vars ======================
  /** run type */
  int run_type = -1; // reset
  /** end loop flag: to be set by fback() */
  int end_loop = 0;

  // caches for fast access
  /** vector of ptrs to active elements */
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



