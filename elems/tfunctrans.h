/***************************************************************************
                          tfunctrans.h  -  description
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

#ifndef TFUNCTRANS_H
#define TFUNCTRANS_H

#include <tmiso.h>


#define CLASS_ID_TFuncTrans 1006

/**trigonometric, exponetial and wavelet functions
  *@author atu
  */

class TFuncTrans : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TFuncTrans( TDataSet *aparent );
   /** empty destructor */
   virtual ~TFuncTrans();
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
   // /** reimplemented from TMiso to ... */
   // virtual int startLoop( int acnx, int acny );
   /** main computation function */
   virtual double f( const double *u, double t );
 protected:
   /** type of function */
   int type;
   /** coefficients and shift */
   double a, b, c, d, e, g, x0;
   /** data descriptors -- with base class elements */ 
   static TDataInfo tfunctrans_d_i[26];
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** icon */
   static const char* icon[];
};

#endif



