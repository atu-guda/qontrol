/***************************************************************************
                          toutarr.h  -  description
                             -------------------
    begin                : Sat Aug 5 2000
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

#ifndef TOUTARR_H
#define TOUTARR_H

#include "dataset.h"

struct DatasInfo;

// ==================== TOutArr ======================================


/** Collector of data during simulation.
  * Depend on type, collect data from single loop, parm loop 1 & 2.
  * @author atu
  */

class TOutArr : public TDataSet  {
  Q_OBJECT
 public:
   DCL_CTOR(TOutArr);
   DCL_CREATE;
   DCL_STD_INF;
   virtual ~TOutArr() override;
   enum OutArrType {
     outSimple = 0,
     outParm1  = 1,
     outParm2  = 2,
     outSpec   = 3
   };
   Q_ENUMS(OutArrType);
   Q_CLASSINFO( "enum_OutArrType_0", "Simple" );     // outSimple
   Q_CLASSINFO( "enum_OutArrType_1", "Parm 1" );     // outParm1
   Q_CLASSINFO( "enum_OutArrType_2", "Parm 2" );     // outParm1
   Q_CLASSINFO( "enum_OutArrType_3", "Special" );    // outSpec

   virtual QVariant dataObj( int col, int role = Qt::DisplayRole ) const override;
   virtual QIcon getIcon() const override;

   /** access to array value */
   virtual const dvector* getArray() const { return &arr; }
   /** request to allocate array: only for special arrays now */
   virtual int alloc( int anx, int any = 1 );
   /** reset counter */
   virtual int reset();

   //* called before all runs
   int preRun( int run_tp, int an, int anx, int any, double adt );
   //* will be called after all actions
   int postRun( int good );
   //* called before each inner param loop
   int startLoop( int acnx, int acny );
   /** will be called after each inner loop */
   int endLoop( int acnx, int acny );
   /** get and push next value. pushed only if level >= type */
   virtual int take_val();

   /** dumps data to file */
   int dump( const QString &fn, const QString &delim );
   /** fills fields in DatasInfo structure, return number of elements (nn) */
   int fillDatasInfo( DatasInfo *di ) const;
 protected:
   /** type of array: 0:simple, 1:parm1, 2:parm2, 3:special */
   PRM_LIST( type, efNoRunChange, "Type", "Type of array", "enum=OutArrType" );
   /** name of element to use */
   PRM_STRING( name, efNoRunChange, "Source", "Name of element to use", "max=64" );
   /** label of data */
   PRM_STRING( label, efNoRunChange, "Label", "Label of data", "max=32" );
   /** size of first dimensions in arrays */
   PRM_INT( nx, efInner, "nx","size of x dimensions arrays", "def=1"  );
   /** size of x=const block in 2-d arrays */
   PRM_INT( ny, efInner, "ny","size of x=const block in 2-d arrays", "def=1"  );
   /** each n-th data collect. def=1 */
   PRM_INT( nq, efNoRunChange, "Every n", "each n-th data collect. ", "min=0\nmax=1000000\ndef=1" );
   /** latch value of counter */
   PRM_INT( lnq, efNoRunChange, "Catch at n=", "latch value of counter", "min=0\nmax=1000000" );
   /** current value of counter(0..nq-1) */
   PRM_INT( cnq, efInner, "Current n", "current value of counter(0..nq-1)", "" );
   /** min value */
   PRM_DOUBLE( dmin, efInner, "min", "min value", "" );
   /** max value */
   PRM_DOUBLE( dmax, efInner, "max", "max value", "" );
   /** array size */
   PRM_INT( arrsize, efInner, "full size", "Full array size", "" );
   /** current number of datas */
   PRM_INT( n, efInner, "current size", "Current number of datas", "" );
   /** data storage */
   dvector arr;
   /** fake source */
   double fake_so = 0;
   /** ptr to source */
   const double *so = &fake_so;

   void put_next_val();

   DCL_DEFAULT_STATIC;
};
typedef TOutArr *PTOutArr;

#endif

