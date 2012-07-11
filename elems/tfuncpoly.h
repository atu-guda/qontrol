/***************************************************************************
                          tfuncpoly.h  -  description
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

#ifndef TFUNCPOLY_H
#define TFUNCPOLY_H

#include <tmiso.h>

#define CLASS_ID_TFuncPoly 1005

/**polinomial functions
  *@author atu
  */

class TFuncPoly : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TFuncPoly( TDataSet *aparent );
   /** empty destructor */
   virtual ~TFuncPoly();
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
   // /** reimplemented from TMiso to ... */
   // virtual int startLoop( int acnx, int acny );
   /** main computation function */
   virtual double f( double t );
 protected:
   /** type of function */
   PRM_LIST( type, 0 );
   /** coefficients and shift */
   PRM_DOUBLE( a, 0 );
   PRM_DOUBLE( b, 0 );
   PRM_DOUBLE( c, 0 );
   PRM_DOUBLE( d, 0 );
   PRM_DOUBLE( e, 0 );
   PRM_DOUBLE( g, 0 );
   PRM_DOUBLE( x0, 0 );
   /** data descriptors -- with base class elements */ 
   static TDataInfo tfuncpoly_d_i[26];
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


