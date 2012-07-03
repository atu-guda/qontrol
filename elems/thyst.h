/***************************************************************************
                          thyst.h  -  description
                             -------------------
    begin                : Sat Aug 26 2000
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

#ifndef THYST_H
#define THYST_H

#include <tmiso.h>

#define CLASS_ID_THyst 1011

/**basic hysteresis simulation
  *@author atu
  */
class THyst : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit THyst( TDataSet *aparent );
   /** empty destructor */
   virtual ~THyst();
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
   /** main computation function */
   virtual double f( const double *u, double t );
   /** reimplemented from TDataSet to handle dynamical coeff */
   virtual int setDataID( int ni, double da, int allowConv );
   /** reimplemented from TMiso to set init state */
   virtual int startLoop( int acnx, int acny );
 protected:
   void fixState(void);
 protected:
   /** type of hysteresis */
   PRM_LIST( type, efNoRunChange );
   /** width of hysteresis  */
   PRM_DOUBLE( x0, 0 );
   /** slope of hysteresis  */
   PRM_DOUBLE( alpha, 0 );
   /** output scale */
   PRM_DOUBLE( a, 0 );
   /** output shift */
   PRM_DOUBLE( b, 0 );
   /** state d */
   PRM_DOUBLE( d, efInner );
   /** state d */
   PRM_DOUBLE( s, efInner );
   /** data descriptors -- with base class elements */ 
   static TDataInfo thyst_d_i[20];
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
