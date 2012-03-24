/***************************************************************************
                          toutputany.h  -  description
                             -------------------
    begin                : Sat Sep 2 2000
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

#ifndef TOUTPUTANY_H
#define TOUTPUTANY_H

#include <tmiso.h>

#define CLASS_ID_TOutputAny 1101

/**gets data from different source by name
  *@author atu
  */

class TOutputAny : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TOutputAny( TDataSet* aparent );
   /** empty destructor */
   virtual ~TOutputAny();
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
   /** return icon */
   virtual const char** getIcon(void) const;      
   /** reimplemented from TMiso to set source */
   virtual int preRun( int run_tp, int an, int anx, int any, double adt );
   /** main computation function */
   virtual double f( const double *u, double t );
 protected:
   /** name of source */
   char name[80];
   /** enabler, type of output: -1:bad, 0:object, 1-vars.  */
   int useEnable, type, ne;
   /** pointer to element  */
   TDataSet *pel;
   /** data descriptors -- with base class elements */ 
   static TDataInfo toutputany_d_i[13];
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** icon */
   static const char* icon[];
};

#endif


