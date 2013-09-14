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

constexpr int def_n_slopes=16; // keep with init strings

class TSourceLin : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TSourceLin);
   DCL_CREATE;
   DCL_STD_INF;
   virtual void post_set() override; // to conversrt from old structure
   /** called before each inner param loop. Unused param - -1 */
   void recalc(void);
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   /** times of lines, <=0 - end */
   virtual int do_startLoop( int acnx, int acny ) override;

   // time 0 means end of sequence - to start
   PRM_DOUBLE_ARR( t_int, efNoRunChange, "t_int", "Time intervals", "N=16\ndef=0\nmin=0\ndefs=1 1 1  1  1 0\nsep=block" );
   PRM_DOUBLE_ARR( vs, efNoRunChange, "vs", "start value",                 "N=16\ndef=0\ndefs=0 0 1 -1 -1 0\nsep=col" );
   PRM_DOUBLE_ARR( ve, efNoRunChange, "ve", "end value",                   "N=16\ndef=0\ndefs=0 1 1 -1  0 0\nsep=col" );

   // [0]==3141592 is a label of old data
   PRM_DOUBLE( t_int_0,  efOld, "t_int[0]",  "Time 0 ", "min=0\ndef=31415926" );
   PRM_DOUBLE( t_int_1,  efOld, "t_int[1]",  "Time 1 ", "min=0" );
   PRM_DOUBLE( t_int_2,  efOld, "t_int[2]",  "Time 2 ", "min=0" );
   PRM_DOUBLE( t_int_3,  efOld, "t_int[3]",  "Time 3 ", "min=0" );
   PRM_DOUBLE( t_int_4,  efOld, "t_int[4]",  "Time 4 ", "min=0" );
   PRM_DOUBLE( t_int_5,  efOld, "t_int[5]",  "Time 5 ", "min=0" );
   PRM_DOUBLE( t_int_6,  efOld, "t_int[6]",  "Time 6 ", "min=0" );
   PRM_DOUBLE( t_int_7,  efOld, "t_int[7]",  "Time 7 ", "min=0" );
   PRM_DOUBLE( t_int_8,  efOld, "t_int[8]",  "Time 8 ", "min=0" );
   PRM_DOUBLE( t_int_9,  efOld, "t_int[9]",  "Time 9 ", "min=0" );
   PRM_DOUBLE( t_int_10, efOld, "t_int[10]", "Time 10", "min=0" );
   PRM_DOUBLE( t_int_11, efOld, "t_int[11]", "Time 11", "min=0" );
   PRM_DOUBLE( t_int_12, efOld, "t_int[12]", "Time 12", "min=0" );
   PRM_DOUBLE( t_int_13, efOld, "t_int[13]", "Time 13", "min=0" );
   PRM_DOUBLE( t_int_14, efOld, "t_int[14]", "Time 14", "min=0" );
   PRM_DOUBLE( t_int_15, efOld, "t_int[15]", "Time 15", "min=0" );
   //
   PRM_DOUBLE(  vs_0, efOld,  "vs[0]",  "Start 0 ", "" );
   PRM_DOUBLE(  vs_1, efOld,  "vs[1]",  "Start 1 ", "" );
   PRM_DOUBLE(  vs_2, efOld,  "vs[2]",  "Start 2 ", "" );
   PRM_DOUBLE(  vs_3, efOld,  "vs[3]",  "Start 3 ", "" );
   PRM_DOUBLE(  vs_4, efOld,  "vs[4]",  "Start 4 ", "" );
   PRM_DOUBLE(  vs_5, efOld,  "vs[5]",  "Start 5 ", "" );
   PRM_DOUBLE(  vs_6, efOld,  "vs[6]",  "Start 6 ", "" );
   PRM_DOUBLE(  vs_7, efOld,  "vs[7]",  "Start 7 ", "" );
   PRM_DOUBLE(  vs_8, efOld,  "vs[8]",  "Start 8 ", "" );
   PRM_DOUBLE(  vs_9, efOld,  "vs[9]",  "Start 9 ", "" );
   PRM_DOUBLE(  vs_10,efOld,  "vs[10]", "Start 10", "" );
   PRM_DOUBLE(  vs_11,efOld,  "vs[11]", "Start 11", "" );
   PRM_DOUBLE(  vs_12,efOld,  "vs[12]", "Start 12", "" );
   PRM_DOUBLE(  vs_13,efOld,  "vs[13]", "Start 13", "" );
   PRM_DOUBLE(  vs_14,efOld,  "vs[14]", "Start 14", "" );
   PRM_DOUBLE(  vs_15,efOld,  "vs[15]", "Start 15", "" );
   //
   PRM_DOUBLE(   ve_0, efOld,  "ve[0]",  "Stop  0 ", "" );
   PRM_DOUBLE(   ve_1, efOld,  "ve[1]",  "Stop  1 ", "" );
   PRM_DOUBLE(   ve_2, efOld,  "ve[2]",  "Stop  2 ", "" );
   PRM_DOUBLE(   ve_3, efOld,  "ve[3]",  "Stop  3 ", "" );
   PRM_DOUBLE(   ve_4, efOld,  "ve[4]",  "Stop  4 ", "" );
   PRM_DOUBLE(   ve_5, efOld,  "ve[5]",  "Stop  5 ", "" );
   PRM_DOUBLE(   ve_6, efOld,  "ve[6]",  "Stop  6 ", "" );
   PRM_DOUBLE(   ve_7, efOld,  "ve[7]",  "Stop  7 ", "" );
   PRM_DOUBLE(   ve_8, efOld,  "ve[8]",  "Stop  8 ", "" );
   PRM_DOUBLE(   ve_9, efOld,  "ve[9]",  "Stop  9 ", "" );
   PRM_DOUBLE(   ve_10,efOld,  "ve[10]", "Stop  10", "" );
   PRM_DOUBLE(   ve_11,efOld,  "ve[11]", "Stop  11", "" );
   PRM_DOUBLE(   ve_12,efOld,  "ve[12]", "Stop  12", "" );
   PRM_DOUBLE(   ve_13,efOld,  "ve[13]", "Stop  13", "" );
   PRM_DOUBLE(   ve_14,efOld,  "ve[14]", "Stop  14", "" );
   PRM_DOUBLE(   ve_15,efOld,  "ve[15]", "Stop  15", "" );

   // NO inputs: todo: fake time

   /** slope values of each line */
   dvector slopes;
   /** number of lines, current line */
   int n_lin, curr_lin;
   /** start time of current line */
   double t_start;

   DCL_DEFAULT_STATIC;
};

#endif

