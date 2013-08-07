/***************************************************************************
                          tmultipl.h  -  description
                             -------------------
    begin                : Fri Aug 25 2000
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

#ifndef TMULTIPL_H
#define TMULTIPL_H

#include <tmiso.h>

/**simple multiplicator
  *@author atu
  */

class TMultipl : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TMultipl( TDataSet* aparent );
   /** empty destructor */
   virtual ~TMultipl();
   /** creator */
   static TDataSet* create( TDataSet* apar );
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
   /** switches -- is this input on */
   PRM_SWITCH1( on0, 0, "On 0", "Use input 0", "" );
   PRM_SWITCH1( on1, 0, "On 1", "Use input 1", "" );
   PRM_SWITCH1( on2, 0, "On 2", "Use input 2", "" );
   PRM_SWITCH1( on3, 0, "On 3", "Use input 3", "" );
   /** common coefficient */
   PRM_DOUBLE1( a,    0, "a", "Common factor", "sep=col" );
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** autoregister */
   static int registered;
   static int reg();
};

#endif


