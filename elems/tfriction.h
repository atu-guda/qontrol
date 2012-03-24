/***************************************************************************
                          tfriction.h  -  description
                             -------------------
    begin                : Jul 18 2003
    copyright            : (C) 2003-2012 by atu
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

#ifndef TFRICTION_H
#define TFRICTION_H

#include <tmiso.h>

#define CLASS_ID_TFriction 1023

/** Simulate of mass under external force and friction
  *@author atu
  */

class TFriction : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TFriction( TDataSet *aparent );
   /** empty destructor */
   virtual ~TFriction();
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
   /** reimplemented from TMiso to init state */
   virtual int startLoop( int acnx, int acny );
   /** main computation function */
   virtual double f( const double *u, double t );
 protected:
   /** Parameter: mody mass */
   double mass;
   /** Parameter: max dry friction force (if constant)  */
   double f_mx;
   /** Parameter: start force addition coeff */
   double kf_mx;
   /** Parameter: max viscous friction coeff */
   double kfv;
   /** Switch: use u[1] as max dry friction force */
   int useMf;
   /** Current speed */
   double v;
   /** old values and misc */
   double v_old, x_old;
   /** state of mass: 0 -- sleep, 1 -- moving */
   int bodyState;
   /** data descriptors -- with base class elements */ 
   static TDataInfo tfriction_d_i[20];
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** icon */
   static const char* icon[];
};

#endif


