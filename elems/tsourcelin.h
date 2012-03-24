/***************************************************************************
                          tsourcelin.h  -  description
                             -------------------
    begin                : Thu Aug 24 2003
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

#ifndef TSOURCELIN_H
#define TSOURCELIN_H

#include "tmiso.h"

#define CLASS_ID_TSourceLin 1024

/** This element provide source, which consist of
  * linear elements.
  * @author atu
  */

class TSourceLin : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TSourceLin( TDataSet* aparent );
   /** empty destructor */
   virtual ~TSourceLin();
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
   /** called before each inner param loop. Unused param - -1 */
   virtual int startLoop( int acnx, int acny );
   /** recalculate slopes and numbers */
   void recalc(void);
 protected:
   /** times of lines, <=0 - end */
   double t_int[16];
   /** start values of each line */
   double vs[16];
   /** end values of each line */
   double ve[16];
   /* ------------ end of iface vars */
   /** slope values of each line */
   double slopes[16];
   /** number of lines, current line */
   int n_lin, curr_lin;
   /** start time of current line */
   double t_start;
   /** data descriptors -- with base classe elements */
   static TDataInfo tsourcelin_d_i[61];
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** icon */
   static const char* icon[];
};

#endif

