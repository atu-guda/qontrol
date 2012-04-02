/***************************************************************************
                          tlogic.h  -  description
                             -------------------
    begin                : Mon Sep 4 2000
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

#ifndef TLOGIC_H
#define TLOGIC_H

#include <tmiso.h>

#define CLASS_ID_TLogic 1013

/**simple logical operation: AND, OR, XOR
  *@author atu
  */

class TLogic : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TLogic( TDataSet* aparent );
   /** empty destructor */
   virtual ~TLogic();
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
   /** main computation function */
   virtual double f( const double *u, double t );
 protected:
   /** type of logic element */
   PRM_LIST( type, efNoRunChange );
   /** level of 1  */
   PRM_DOUBLE( level, 0 );
   /** misc flags */
   PRM_SWITCH( useNInp0, efNoRunChange );
   PRM_SWITCH( useNInp1, efNoRunChange );
   PRM_SWITCH( useNInp2, efNoRunChange );
   PRM_SWITCH( useNInp3, efNoRunChange );
   PRM_SWITCH( useNOut, efNoRunChange );
   PRM_SWITCH( useMinus, efNoRunChange );
   /** data descriptors -- with base class elements */ 
   static TDataInfo tlogic_d_i[20];
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** icon */
   static const char* icon[];
};

#endif


