/***************************************************************************
                          tdelay.h  -  description
                             -------------------
    begin                : Sun Aug 27 2000
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

#ifndef TDELAY_H
#define TDELAY_H

#include <tmiso.h>
class TCircBuf;


#define CLASS_ID_TDelay 1008

/**delays signal for given time
  *@author atu
  */

class TDelay : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TDelay( TDataSet *aparent );
   /** empty destructor */
   virtual ~TDelay();
   /** creator */
   static TDataSet* create( TDataSet *apar );
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
   /** return icon */
   virtual const char** getIcon(void) const;      
   /** reimplemented from TMiso to reset array */
   virtual int startLoop( int acnx, int acny );
   /** main computation function */
   virtual double f( const double *u, double t );
   /** reimplemented from TDataSet handle current delay limit */
   virtual int setDataID( int ni, double da, int allowConv );
   /** calc values & alloc buf */
   virtual int preRun( int run_tp, int an, int anx, int any, double adt );
   /** deallocates buf */
   virtual int postRun( int good );
 protected:
   /** max delay, current delay + */
   double mdelay, cdelay, v1, v2, u00;
   /** max and current delays indexes ... */
   int imd, icd;
   /** data container */
   TCircBuf *buf;
   /** data descriptors -- with base class elements */ 
   static TDataInfo tdelay_d_i[14];
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** icon */
   static const char* icon[];
};

#endif


