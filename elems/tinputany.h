/***************************************************************************
                          tinputany.h  -  description
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

#ifndef TINPUTANY_H
#define TINPUTANY_H

#include <tmiso.h>


#define CLASS_ID_TInputAny 1100

/**gets data from different source by name
  *@author atu
  */

class TInputAny : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TInputAny( TDataSet *aparent );
   /** empty destructor */
   virtual ~TInputAny();
   /** creator */
   static TDataSet* create( TDataSet *apar );
   /** class id */
   virtual int getClassId(void) const ;
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
   PRM_STRING( name, efNoRunChange );
   /** type of input: -1:bad, 0:object, 1-vars., index of input  */
   int type, ne;
   /** pointer to element  */
   TDataSet *pel;
   /** data descriptors -- with base class elements */ 
   static TDataInfo tinputany_d_i[12];
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** icon */
   static const char* icon[];
   /** autoregister */
   static int registered;
   static int reg();
};

#endif


