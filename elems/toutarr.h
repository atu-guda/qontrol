/***************************************************************************
                          toutarr.h  -  description
                             -------------------
    begin                : Sat Aug 5 2000
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

#ifndef TOUTARR_H
#define TOUTARR_H

#include "dataset.h"

struct GraphInfo;

// ==================== TOutArr ======================================

#define CLASS_ID_TOutArr 600

/** Collector of data during simulation.
  * Depend on type, collect data from single loop, parm loop 1 & 2.
  * @author atu
  */

class TOutArr : public TDataSet  {
  Q_OBJECT
 public:
   /** constructor */ 
   explicit TOutArr( TDataSet* apar );
   /** destructor */
   virtual ~TOutArr();
   /** creator */
   static TDataSet* create( TDataSet* apar );
   /** class id */
   virtual int getClassId(void) const ;
   /** class name - for check & human purpose */
   virtual const char* getClassName(void) const;
   /** return ptr to static class_info, nust be implemented in each class */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
   /** redefined from TDataSet to access array data as '#123' */
   virtual int getDataSD( const char *nm, double *da, int allowConv );
   /** redefined from TDataSet to access array data as '#123' */
   virtual int setDataSD( const char *nm, double da, int allowConv );
   /** access to array value */
   virtual const double* getArray(void);
   /** request to allocate array */
   virtual int alloc( int sz, int a_ny = 1 );
   /** request to dealloc array */
   virtual int free(void);
   /** reset counter,  if level >= type */
   virtual int reset( int level);
   /** push next value. pushed only if level >= type */
   virtual int push_val( double v, int level );
   /** dumps data to file */
   int dump( const char *fn, char delim = ' ' );
   /** fills fields in GraphInfo structure */
   int fillGraphInfo( GraphInfo *gi ) const;
 protected:
   /** type of array: 0:simple, 1:parm1, 2:parm2, 3:special */
   PRM_LIST( type, efNoRunChange );
   /** name of element to use */
   PRM_STRING( name, efNoRunChange );
   /** label of data */
   PRM_STRING( label, 0 );
   /** size of x=const block in 2-d arrays */
   PRM_INT( ny, efInner );
   /** each n-th data collect. def=1 */
   PRM_INT( nq, efNoRunChange );
   /** latch value of counter */
   PRM_INT( lnq, efNoRunChange );
   /** current value of counter(0..nq-1) */
   PRM_INT( cnq, efInner );
   /** min value */
   PRM_DOUBLE( dmin, efInner );
   /** max value */
   PRM_DOUBLE( dmax, efInner );
   /** array size */
   PRM_INT( arrsize, efInner );
   /** current number of datas */
   PRM_INT( n, efInner );
   /** data storage TODO: vector */
   double *arr;
   /** data descriptors */ 
   static TDataInfo toutarr_d_i[19]; 
   /** class decription */
   static TClassInfo class_info;       
   /** help str */
   static const char* helpstr;
   /** autoregister */
   static int registered;
   static int reg();
};
typedef TOutArr *PTOutArr;

#endif

