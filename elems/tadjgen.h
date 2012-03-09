/***************************************************************************
                          tadjgen.h  -  description
                             -------------------
    begin                : Wed Aug 30 2000
    copyright            : (C) 2000 by atu
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

#ifndef TADJGEN_H
#define TADJGEN_H

#include <tmiso.h>


#define CLASS_ID_TAdjGen 1009

/**Controlled on frequency generator sqare pulses
  *@author atu
  */

class TAdjGen : public TMiso  {
 public:
   /** constructor */
   explicit TAdjGen( TDataSet *aparent );
   /** empty destructor */
   virtual ~TAdjGen();
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
   /** reimplemented from TMiso to reset */
   virtual int startLoop( int acnx, int acny );
   /** main computation function */
   virtual double f( const double *u, double t );
 protected:
   /** type of averaging, misc flags */
   int type, useReset, useLock, outStrobe, useZero, 
       useSignStrobe, usePlusStrobe, currOut, tick;
   /**  accumulators, ... */
   double ctt, ig, ig2;
   /** data descriptors -- with base class elements */ 
   static TDataInfo tadjgen_d_i[22];
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** icon */
   static const char* icon[];
};


#endif

