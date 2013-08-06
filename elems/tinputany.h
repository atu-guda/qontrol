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
   /** return ptr to static class_info, nust be implemented in each class */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
 protected:
   /** main computation function */
   virtual double f( double t );
   /** reimplemented from TMiso to set source */
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt );
   /** name of source */
   PRM_STRING1( name, efNoRunChange, "Name", 
       "Name of element (and may be parameter) to get values", "max=80");
   /** fake source */
   double fake_so;
   /** pointer to source */
   const double *so;
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** autoregister */
   static int registered;
   static int reg();
};

#endif


