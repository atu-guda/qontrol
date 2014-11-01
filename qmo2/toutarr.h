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

struct GraphInfo;

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

   /** access to array value */
   virtual const dvector* getArray();
   /** request to allocate array */
   virtual int alloc( int sz, int a_ny = 1 );
   /** request to dealloc array */
   virtual int free();
   /** reset counter,  if level >= type */
   virtual int reset( int level);
   /** push next value. pushed only if level >= type */
   virtual int push_val( double v, int level );
   /** get and push next value. pushed only if level >= type */
   virtual int take_val( int level );
   /** dumps data to file */
   int dump( const char *fn, char delim = ' ' );
   /** fills fields in GraphInfo structure */
   int fillGraphInfo( GraphInfo *gi ) const;
 protected:
   /** type of array: 0:simple, 1:parm1, 2:parm2, 3:special */
   PRM_LIST( type, efNoRunChange, "Type", "Type of array", "enum=OutArrType" );
   /** name of element to use */
   PRM_STRING( name, efNoRunChange, "Source", "Name of element to use", "max=64" );
   /** label of data */
   PRM_STRING( label, efNoRunChange, "Label", "Label of data", "max=32" );
   /** size of x=const block in 2-d arrays */
   PRM_INT( ny, efInner, "ny","size of x=const block in 2-d arrays", ""  );
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

   DCL_DEFAULT_STATIC;
};
typedef TOutArr *PTOutArr;

#endif

