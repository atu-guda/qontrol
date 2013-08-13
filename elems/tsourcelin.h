/***************************************************************************
                          tsourcelin.h  -  description
                             -------------------
    begin                : Thu Aug 24 2003
    copyright            : (C) 2003-2013 by atu
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

#ifndef TSOURCELIN_H
#define TSOURCELIN_H

#include "tmiso.h"

/** This element provide source, which consist of
  * linear elements.
  * @author atu
  */

class TSourceLin : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TSourceLin);
   DCL_CREATE;
   DCL_STD_INF;
   /** called before each inner param loop. Unused param - -1 */
   void recalc(void);
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   /** times of lines, <=0 - end */
   virtual int do_startLoop( int acnx, int acny ) override;
   
   /** recalculate slopes and numbers */
   double t_int[16]; // FIXME: ???? link or array data
   PRM_DOUBLE( t_int_0,  efNoRunChange, "t_int[0]",  "Time 0 ", "min=0" );
   PRM_DOUBLE( t_int_1,  efNoRunChange, "t_int[1]",  "Time 1 ", "min=0" );
   PRM_DOUBLE( t_int_2,  efNoRunChange, "t_int[2]",  "Time 2 ", "min=0" );
   PRM_DOUBLE( t_int_3,  efNoRunChange, "t_int[3]",  "Time 3 ", "min=0" );
   PRM_DOUBLE( t_int_4,  efNoRunChange, "t_int[4]",  "Time 4 ", "min=0" );
   PRM_DOUBLE( t_int_5,  efNoRunChange, "t_int[5]",  "Time 5 ", "min=0" );
   PRM_DOUBLE( t_int_6,  efNoRunChange, "t_int[6]",  "Time 6 ", "min=0" );
   PRM_DOUBLE( t_int_7,  efNoRunChange, "t_int[7]",  "Time 7 ", "min=0" );
   PRM_DOUBLE( t_int_8,  efNoRunChange, "t_int[8]",  "Time 8 ", "min=0" );
   PRM_DOUBLE( t_int_9,  efNoRunChange, "t_int[9]",  "Time 9 ", "min=0" );
   PRM_DOUBLE( t_int_10, efNoRunChange, "t_int[10]", "Time 10", "min=0" );
   PRM_DOUBLE( t_int_11, efNoRunChange, "t_int[11]", "Time 11", "min=0" );
   PRM_DOUBLE( t_int_12, efNoRunChange, "t_int[12]", "Time 12", "min=0" );
   PRM_DOUBLE( t_int_13, efNoRunChange, "t_int[13]", "Time 13", "min=0" );
   PRM_DOUBLE( t_int_14, efNoRunChange, "t_int[14]", "Time 14", "min=0" );
   PRM_DOUBLE( t_int_15, efNoRunChange, "t_int[15]", "Time 15", "min=0" );
   /** start values of each line */
   double vs[16];
   PRM_DOUBLE(  vs_0, efNoRunChange,  "vs[0]",  "Start 0 ", "sep=col" );
   PRM_DOUBLE(  vs_1, efNoRunChange,  "vs[1]",  "Start 1 ", "" );
   PRM_DOUBLE(  vs_2, efNoRunChange,  "vs[2]",  "Start 2 ", "" );
   PRM_DOUBLE(  vs_3, efNoRunChange,  "vs[3]",  "Start 3 ", "" );
   PRM_DOUBLE(  vs_4, efNoRunChange,  "vs[4]",  "Start 4 ", "" );
   PRM_DOUBLE(  vs_5, efNoRunChange,  "vs[5]",  "Start 5 ", "" );
   PRM_DOUBLE(  vs_6, efNoRunChange,  "vs[6]",  "Start 6 ", "" );
   PRM_DOUBLE(  vs_7, efNoRunChange,  "vs[7]",  "Start 7 ", "" );
   PRM_DOUBLE(  vs_8, efNoRunChange,  "vs[8]",  "Start 8 ", "" );
   PRM_DOUBLE(  vs_9, efNoRunChange,  "vs[9]",  "Start 9 ", "" );
   PRM_DOUBLE(  vs_10,efNoRunChange,  "vs[10]", "Start 10", "" );
   PRM_DOUBLE(  vs_11,efNoRunChange,  "vs[11]", "Start 11", "" );
   PRM_DOUBLE(  vs_12,efNoRunChange,  "vs[12]", "Start 12", "" );
   PRM_DOUBLE(  vs_13,efNoRunChange,  "vs[13]", "Start 13", "" );
   PRM_DOUBLE(  vs_14,efNoRunChange,  "vs[14]", "Start 14", "" );
   PRM_DOUBLE(  vs_15,efNoRunChange,  "vs[15]", "Start 15", "" );
   /** end values of each line */
   double ve[16];
   PRM_DOUBLE(   ve_0, efNoRunChange,  "ve[0]",  "Stop  0 ", "sep=col" );
   PRM_DOUBLE(   ve_1, efNoRunChange,  "ve[1]",  "Stop  1 ", "" );
   PRM_DOUBLE(   ve_2, efNoRunChange,  "ve[2]",  "Stop  2 ", "" );
   PRM_DOUBLE(   ve_3, efNoRunChange,  "ve[3]",  "Stop  3 ", "" );
   PRM_DOUBLE(   ve_4, efNoRunChange,  "ve[4]",  "Stop  4 ", "" );
   PRM_DOUBLE(   ve_5, efNoRunChange,  "ve[5]",  "Stop  5 ", "" );
   PRM_DOUBLE(   ve_6, efNoRunChange,  "ve[6]",  "Stop  6 ", "" );
   PRM_DOUBLE(   ve_7, efNoRunChange,  "ve[7]",  "Stop  7 ", "" );
   PRM_DOUBLE(   ve_8, efNoRunChange,  "ve[8]",  "Stop  8 ", "" );
   PRM_DOUBLE(   ve_9, efNoRunChange,  "ve[9]",  "Stop  9 ", "" );
   PRM_DOUBLE(   ve_10,efNoRunChange,  "ve[10]", "Stop  10", "" );
   PRM_DOUBLE(   ve_11,efNoRunChange,  "ve[11]", "Stop  11", "" );
   PRM_DOUBLE(   ve_12,efNoRunChange,  "ve[12]", "Stop  12", "" );
   PRM_DOUBLE(   ve_13,efNoRunChange,  "ve[13]", "Stop  13", "" );
   PRM_DOUBLE(   ve_14,efNoRunChange,  "ve[14]", "Stop  14", "" );
   PRM_DOUBLE(   ve_15,efNoRunChange,  "ve[15]", "Stop  15", "" );
   /* ------------ end of iface vars */
   /** slope values of each line */
   double slopes[16];
   /** number of lines, current line */
   int n_lin, curr_lin;
   /** start time of current line */
   double t_start;
   
   DCL_DEFAULT_STATIC;
};

#endif

