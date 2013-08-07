/***************************************************************************
                          toutarr.h  -  description
                             -------------------
    begin                : Sat Aug 5 2000
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

#ifndef TOUTARR_H
#define TOUTARR_H

#include "dataset.h"

struct GraphInfo;

// ==================== TOutArr ======================================

static const char* const toutarr_list = 
  "Simple\n" // 0
  "Parm 1\n" // 1
  "Parm 2\n" // 2
  "Special"  // 3
;

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
   /** return ptr to static class_info, nust be implemented in each class */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
   /** access to array value */
   virtual const dvector* getArray(void);
   /** request to allocate array */
   virtual int alloc( int sz, int a_ny = 1 );
   /** request to dealloc array */
   virtual int free(void);
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
   PRM_LIST1( type, efNoRunChange, "Type", 
       "Type of array: 0:simple, 1:parm1, 2:parm2, 3:special", "", toutarr_list );
   /** name of element to use */
   PRM_STRING1( name, efNoRunChange, "Source", "Name of element to use", "max=32" );
   /** label of data */
   PRM_STRING1( label, efNoRunChange, "Label", "Label of data", "max=32" );
   /** size of x=const block in 2-d arrays */
   PRM_INT1( ny, efInner, "ny","size of x=const block in 2-d arrays", ""  );
   /** each n-th data collect. def=1 */
   PRM_INT1( nq, efNoRunChange, "Every n", "each n-th data collect. def=1", "min=0\nmax=1e6" );
   /** latch value of counter */
   PRM_INT1( lnq, efNoRunChange, "Catch at n=", "latch value of counter", "min=0\nmax=1e6" );
   /** current value of counter(0..nq-1) */
   PRM_INT1( cnq, efInner, "Current n", "current value of counter(0..nq-1)", "" );
   /** min value */
   PRM_DOUBLE1( dmin, efInner, "min", "min value", "" );
   /** max value */
   PRM_DOUBLE1( dmax, efInner, "max", "max value", "" );
   /** array size */
   PRM_INT1( arrsize, efInner, "full size", "Full array size", "" );
   /** current number of datas */
   PRM_INT1( n, efInner, "current size", "Current number of datas", "" );
   /** data storage TODO: vector */
   dvector arr;
   /** fake source */
   double fake_so;
   /** ptr to source */
   const double *so;
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

