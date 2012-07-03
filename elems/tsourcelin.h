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
   PRM_DOUBLEx( t_int+0,  t_int_0, efNoRunChange, "t_int[0]",  "Time 0 ", "min=0" );
   PRM_DOUBLEx( t_int+1,  t_int_1, efNoRunChange, "t_int[1]",  "Time 1 ", "min=0" );
   PRM_DOUBLEx( t_int+2,  t_int_2, efNoRunChange, "t_int[2]",  "Time 2 ", "min=0" );
   PRM_DOUBLEx( t_int+3,  t_int_3, efNoRunChange, "t_int[3]",  "Time 3 ", "min=0" );
   PRM_DOUBLEx( t_int+4,  t_int_4, efNoRunChange, "t_int[4]",  "Time 4 ", "min=0" );
   PRM_DOUBLEx( t_int+5,  t_int_5, efNoRunChange, "t_int[5]",  "Time 5 ", "min=0" );
   PRM_DOUBLEx( t_int+6,  t_int_6, efNoRunChange, "t_int[6]",  "Time 6 ", "min=0" );
   PRM_DOUBLEx( t_int+7,  t_int_7, efNoRunChange, "t_int[7]",  "Time 7 ", "min=0" );
   PRM_DOUBLEx( t_int+8,  t_int_8, efNoRunChange, "t_int[8]",  "Time 8 ", "min=0" );
   PRM_DOUBLEx( t_int+9,  t_int_9, efNoRunChange, "t_int[9]",  "Time 9 ", "min=0" );
   PRM_DOUBLEx( t_int+10, t_int_10,efNoRunChange, "t_int[10]", "Time 10", "min=0" );
   PRM_DOUBLEx( t_int+11, t_int_11,efNoRunChange, "t_int[11]", "Time 11", "min=0" );
   PRM_DOUBLEx( t_int+12, t_int_12,efNoRunChange, "t_int[12]", "Time 12", "min=0" );
   PRM_DOUBLEx( t_int+13, t_int_13,efNoRunChange, "t_int[13]", "Time 13", "min=0" );
   PRM_DOUBLEx( t_int+14, t_int_14,efNoRunChange, "t_int[14]", "Time 14", "min=0" );
   PRM_DOUBLEx( t_int+15, t_int_15,efNoRunChange, "t_int[15]", "Time 15", "min=0" );
   /** start values of each line */
   double vs[16];
   PRM_DOUBLEx( vs+0,   vs_0, efNoRunChange,  "vs[0]",  "Start 0 ", "sep=col" );
   PRM_DOUBLEx( vs+1,   vs_1, efNoRunChange,  "vs[1]",  "Start 1 ", "" );
   PRM_DOUBLEx( vs+2,   vs_2, efNoRunChange,  "vs[2]",  "Start 2 ", "" );
   PRM_DOUBLEx( vs+3,   vs_3, efNoRunChange,  "vs[3]",  "Start 3 ", "" );
   PRM_DOUBLEx( vs+4,   vs_4, efNoRunChange,  "vs[4]",  "Start 4 ", "" );
   PRM_DOUBLEx( vs+5,   vs_5, efNoRunChange,  "vs[5]",  "Start 5 ", "" );
   PRM_DOUBLEx( vs+6,   vs_6, efNoRunChange,  "vs[6]",  "Start 6 ", "" );
   PRM_DOUBLEx( vs+7,   vs_7, efNoRunChange,  "vs[7]",  "Start 7 ", "" );
   PRM_DOUBLEx( vs+8,   vs_8, efNoRunChange,  "vs[8]",  "Start 8 ", "" );
   PRM_DOUBLEx( vs+9,   vs_9, efNoRunChange,  "vs[9]",  "Start 9 ", "" );
   PRM_DOUBLEx( vs+10,  vs_10,efNoRunChange,  "vs[10]", "Start 10", "" );
   PRM_DOUBLEx( vs+11,  vs_11,efNoRunChange,  "vs[11]", "Start 11", "" );
   PRM_DOUBLEx( vs+12,  vs_12,efNoRunChange,  "vs[12]", "Start 12", "" );
   PRM_DOUBLEx( vs+13,  vs_13,efNoRunChange,  "vs[13]", "Start 13", "" );
   PRM_DOUBLEx( vs+14,  vs_14,efNoRunChange,  "vs[14]", "Start 14", "" );
   PRM_DOUBLEx( vs+15,  vs_15,efNoRunChange,  "vs[15]", "Start 15", "" );
   /** end values of each line */
   double ve[16];
   PRM_DOUBLEx( ve+0,   ve_0, efNoRunChange,  "ve[0]",  "Stop  0 ", "sep=col" );
   PRM_DOUBLEx( ve+1,   ve_1, efNoRunChange,  "ve[1]",  "Stop  1 ", "" );
   PRM_DOUBLEx( ve+2,   ve_2, efNoRunChange,  "ve[2]",  "Stop  2 ", "" );
   PRM_DOUBLEx( ve+3,   ve_3, efNoRunChange,  "ve[3]",  "Stop  3 ", "" );
   PRM_DOUBLEx( ve+4,   ve_4, efNoRunChange,  "ve[4]",  "Stop  4 ", "" );
   PRM_DOUBLEx( ve+5,   ve_5, efNoRunChange,  "ve[5]",  "Stop  5 ", "" );
   PRM_DOUBLEx( ve+6,   ve_6, efNoRunChange,  "ve[6]",  "Stop  6 ", "" );
   PRM_DOUBLEx( ve+7,   ve_7, efNoRunChange,  "ve[7]",  "Stop  7 ", "" );
   PRM_DOUBLEx( ve+8,   ve_8, efNoRunChange,  "ve[8]",  "Stop  8 ", "" );
   PRM_DOUBLEx( ve+9,   ve_9, efNoRunChange,  "ve[9]",  "Stop  9 ", "" );
   PRM_DOUBLEx( ve+10,  ve_10,efNoRunChange,  "ve[10]", "Stop  10", "" );
   PRM_DOUBLEx( ve+11,  ve_11,efNoRunChange,  "ve[11]", "Stop  11", "" );
   PRM_DOUBLEx( ve+12,  ve_12,efNoRunChange,  "ve[12]", "Stop  12", "" );
   PRM_DOUBLEx( ve+13,  ve_13,efNoRunChange,  "ve[13]", "Stop  13", "" );
   PRM_DOUBLEx( ve+14,  ve_14,efNoRunChange,  "ve[14]", "Stop  14", "" );
   PRM_DOUBLEx( ve+15,  ve_15,efNoRunChange,  "ve[15]", "Stop  15", "" );
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
   /** autoregister */
   static int registered;
   static int reg();
};

#endif

