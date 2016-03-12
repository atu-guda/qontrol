#pragma once
/***************************************************************************
                          toutarr.h  -  description
                             -------------------
    begin                : Sat Aug 5 2000
    copyright            : (C) 2000-2016 by atu
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


#include "linkedobj.h"

struct DatasInfo;

// ==================== TOutArr ======================================


/** Collector of data during simulation.
  * Depend on type, collect data from single loop, parm loop 1 & 2.
  * @author atu
  */

class TOutArr : public LinkedObj  {
  Q_OBJECT
 public:
   DCL_CTOR(TOutArr);
   DCL_CREATE;
   DCL_STD_INF;
   // virtual ~TOutArr() override;
   Q_INVOKABLE virtual QString textVisual() const override;

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
   const dvector* getArray() const { return &arr; }
   /** request to allocate array: only for special arrays now */
   Q_INVOKABLE int alloc( int anx, int any = 1 );
   Q_INVOKABLE virtual arrsize_type arrSize() const override { return arrsize; }
   Q_INVOKABLE int getN() const { return n; }

   /** get and push next value. pushed only if level >= type */
   virtual int take_val();

   /** dumps data to file */
   Q_INVOKABLE int dump( const QString &fn, const QString &delim );
   /** fills fields in DatasInfo structure, return number of elements (nn) */
   int fillDatasInfo( DatasInfo *di ) const;
   //* access to one element by 1 ind 2 index
   Q_INVOKABLE double at( int i ) const;
   Q_INVOKABLE double at( int x, int y ) const;
   Q_INVOKABLE double getLast() const;
   Q_INVOKABLE double atT( double T ) const;
   Q_INVOKABLE void put( int i, double v );
   Q_INVOKABLE void put( int x, int y, double v );
   Q_INVOKABLE void add( double v );
   // fact access to statistics. calc_stat is cheap w/o force
   Q_INVOKABLE void calc_stat( bool force = false, bool forceAll = false );
   Q_INVOKABLE double getMin() const   {  return dmin; }
   Q_INVOKABLE double getMax() const   {  return dmax; }
   Q_INVOKABLE int    getIMin() const  {  return imin; }
   Q_INVOKABLE int    getIMax() const  {  return imax; }
   Q_INVOKABLE double getSum() const   {  return s_x; }
   Q_INVOKABLE double getSum2() const  {  return s_x2; }
   Q_INVOKABLE double getAver() const  {  return a_x; }
   Q_INVOKABLE double getAver2() const {  return a_x2; }
   Q_INVOKABLE double getVariance() const {  return var_x; }
   Q_INVOKABLE double getStdDev() const {  return sd_x; }
   Q_INVOKABLE double getAbsDev() const {  return absdev_x; }
   Q_INVOKABLE double getAutoCorr() const { return acorr; }
   Q_INVOKABLE QString getAllStats( QString sep = "; ") const;
   Q_INVOKABLE bool isAllFinite() const { return isfin; };
 protected:
   virtual void do_reset() override;
   virtual void do_structChanged() override;
   void set_link();
   void reset_stat();
   virtual int do_preRun() override;
   virtual int do_postRun( int good ) override;
   virtual int do_startLoop( int acnx, int acny ) override;
   virtual int do_endLoop() override;

   PRM_LIST( type, efNRC, "Type", "Type of array", "enum=OutArrType" );
   PRM_STRING( descr, efNRC, "description", "Output array description", "max=256\nncol=-1");
   PRM_STRING( name, efNRC, "Source", "Name of element to use", "sep=block\nmax=128\nprops=STRING,SIMPLE,LINK\ncmpl=in" );
   PRM_STRING( label, efNRC, "Label", "Label of data", "max=32" );
   PRM_SWITCH( allStat, efNRC, "All stat", "calculate all statistics", "def=0" );
   PRM_INT( nx, efInner, "nx","size of x dimensions arrays", "def=1"  );
   PRM_INT( ny, efInner, "ny","size of x=const block in 2-d arrays", "def=1"  );
   PRM_INT( nq, efNRC, "Every n", "each n-th data collect. ", "min=1\nmax=1000000\ndef=1" );
   PRM_INT( lnq, efNRC, "Catch at n=", "latch value of counter", "min=0\nmax=1000000" );
   PRM_INT( cnq, efInner, "Current n", "current value of counter(0..nq-1)", "" );
   PRM_DOUBLE( scale, efNRC, "Scale", "input factor", "def=1" );
   PRM_DOUBLE( shift, efNRC, "Shift", "input shift", "" );
   // statistics
   PRM_DOUBLE( dmin, efInner, "min", "min value", "" );
   PRM_DOUBLE( dmax, efInner, "max", "max value", "" );
   PRM_DOUBLE( s_x,  efInner, "s_x", "summ of elements", "" );
   PRM_DOUBLE( s_x2,  efInner, "s_x2", "summ of x^2", "" );
   PRM_DOUBLE( a_x,  efInner, "a_x", "average of elements", "" );
   PRM_DOUBLE( a_x2,  efInner, "a_x2", "average of squares", "" );
   PRM_DOUBLE( var_x,  efInner, "var_x", "Variance", "" );
   PRM_DOUBLE( sd_x,  efInner, "sd_x", "Standard deviation", "" );
   PRM_DOUBLE( absdev_x,  efInner, "absvar_x", "Absolute deviation", "" );
   PRM_DOUBLE( acorr,  efInner, "acorr", "autocorrelation", "" );
   /** array size */
   PRM_INT( arrsize, efInner, "full size", "Full array size", "" );
   /** current number of datas */
   PRM_INT( n, efInner, "current size", "Current number of datas", "" );
   PRM_INT( imin, efInner, "imin", "index of minimal element", "def=-1" );
   PRM_INT( imax, efInner, "imax", "index of maximal element", "def=-1" );
   PRM_INT( isfin, efInner, "isfin", "Flag: all values was finite", "def=1" );
   /** data storage */
   dvector arr;
   /** fake source */
   double fake_so = 0;
   /** ptr to source */
   const double *so = &fake_so;
   //* flag: need to calc statistics
   bool need_calc_stat = false;

   void put_next_val();

   Q_CLASSINFO( "nameHintBase",  "out_" );
   DCL_DEFAULT_STATIC;
};
typedef TOutArr *PTOutArr;


