/***************************************************************************
                          tmiso.h  -  description
                             -------------------
    begin                : Mon Jul 24 2000
    copyright            : (C) 2000-2015 by atu
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

/** \file tmiso.h
  *@author atu
*/


#ifndef TMISO_H
#define TMISO_H

#include "dataset.h"
#include "inputparams.h"

/** iteration type */
enum IterType {
  IterMid = 0,     // not-first and not-last iteration
  IterFirst = 1,   // first iteration in i inner loop
  IterLast = 2     // last iteration in inner loop
};

class TModel; class Scheme;

// -------------------------- TMiso -----------------------------

/** \class TMiso tmiso.h
 * \breef Pure parent of all scheme elements.
 *
 */

class TMiso : public TDataSet  {
  Q_OBJECT
 public:
   DCL_CTOR(TMiso);
   virtual ~TMiso();
   DCL_CREATE;
   DCL_STD_INF;

   /** external computation function + in/out */
   double fun( double t, IterType itype );

   /** will be called before any action -- good place for allocs
    *
    * \param run_tp current run type: 0-simple,
    *      1 - 1d param loop, 2 - 2d param loop
    * \param an number of steps in modeling
    * \param anx number of inner param loop
    * \param any number of outer param loop
    * \param adt time step  - will be a \b tdt in elememt
    * non-virtual: adjusted by do_preRun
    * */
   int preRun( int run_tp, int an, int anx, int any, double adt );
   /** will be called after all actions -- call do_postRun */
   int postRun( int good );
   /** called before each inner param loop -- call do_startLoop */
   int startLoop( int acnx, int acny );
   /** will be called after each inner loop -- call do_endLoop */
   int endLoop();
   /** fast access to order */
   int getOrder() const { return ord; }
   /** compare objects by order - to sort before run */
   friend bool operator<( const TMiso &lhs, const TMiso &rhs )
     { return lhs.ord < rhs.ord; }
   /** check, if element have given visual coords */
   bool isAtCoord( int ax, int ay ) const
     { return (vis_x == ax && vis_y == ay ); }
   virtual void fillComplModelForParams( QStandardItemModel *mdl ) const;
 protected:
   /** main computation function
    * \param t current time
    * \return output value of element
    * */
   virtual double f( double t );
   /** place of customization of preRun, return: !=0 = Ok */
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt );
   /** will be called after all actions from posrtRun  -- good place for deallocs */
   virtual int do_postRun( int good );
   /** called before each inner param loop from startLoop */
   virtual int do_startLoop( int acnx, int acny );
   /** will be called after each inner loop: called from endLoop */
   virtual int do_endLoop();
   /** description on element */
   PRM_STRING( descr, efNoRunChange, "description",
       "Object description", "max=128\nncol=-1");
   /** order, in which elements will be used and holder */
   PRM_INT( ord, efRODial | efNoRunChange, "Order",
       "Order, in which element will be processed", "min=0\nsep=block" );
   /** visual coordinates */
   PRM_INT( vis_x, efRODial | efNoRunChange, "Visual x",
       "X-coordinate of element in scheme", "min=0\nmax=64" );
   PRM_INT( vis_y, efRODial | efNoRunChange, "Visual y",
       "Y-coordinate of element in scheme", "min=0\nmax=64" );
   PRM_DOUBLE( out0_init, efNoRunChange, "Init value",
       "Initial value of output", "" );
   PRM_SWITCH( locked, efNoRunChange, "Locked", "Bypass u[0] to output", "sep=col"  );
   PRM_SWITCH( flip, efNoRunChange, "flip image", "flip left-right element icon", "");
   PRM_SWITCH( onlyFirst, efNoRunChange, "only First", "Process element only at first iteration", "cep=col");
   PRM_SWITCH( onlyLast, efNoRunChange , "only Last", "Process element only at last iteration", "");
   PRM_SWITCH( noIcon, efNoRunChange, "no Icon", "don't show element icon", "sep=col");

   PRM_DOUBLE( out0, efInner, "Output", "Main output", "" );

   /** pointer to param inputs container */
   InputParams *pis;
   /** time step -- setted by preRun 0 - special value to detect usage before start */
   double tdt = 0;
   /** number of iteration per loop -- setted by PreRun */
   int model_nn = 0;
   /** parameters modified during run flag */
   int prm_mod = 0;
   //* Current itration type: to propagete to subschemes...
   IterType iter_c = IterMid;

   DCL_DEFAULT_STATIC;

};

typedef TMiso* PTMiso;

#endif
