/***************************************************************************
                          tmiso.h  -  description
                             -------------------
    begin                : Mon Jul 24 2000
    copyright            : (C) 2000-2016 by atu
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

#include "linkedobj.h"


// -------------------------- TMiso -----------------------------

/** \class TMiso tmiso.h
 * \breef Parent of all scheme elements.
 *
 */

class TMiso : public LinkedObj  {
  Q_OBJECT
 public:
   DCL_CTOR(TMiso);
   // virtual ~TMiso() override;
   DCL_CREATE;
   DCL_STD_INF;

   /** external computation function + in/out */
   double fun( IterType itype ) noexcept;

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
   virtual double f() noexcept;
   /** place of customization of preRun, return: !=0 = Ok */
   // virtual int do_preRun( int run_tp, int an, int anx, int any, double adt ) override;
   /** will be called after all actions from posrtRun  -- good place for deallocs */
   // virtual int do_postRun( int good ) override;
   /** called before each inner param loop from startLoop */
   virtual int do_startLoop( int acnx, int acny ) override;
   /** will be called after each inner loop: called from endLoop */
   // virtual int do_endLoop() override;
   /** do real actions after structure changed - refills inputs */
   virtual void do_structChanged() override;

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
   PRM_PARAMD( out0_init, efNRC, "Init value", "Initial value of output", "def=0.0" );
   PRM_SWITCH( locked, efNoRunChange, "Locked", "Bypass out0_init to output", "sep=col"  );
   PRM_SWITCH( ignored, efNoRunChange, "Ignored", "Ignore element while run", ""  );
   PRM_SWITCH( onlyFirst, efNoRunChange, "only First", "Process element only at first iteration", "" );
   PRM_SWITCH( onlyLast, efNoRunChange , "only Last", "Process element only at last iteration", "" );
   PRM_SWITCH( flip, efNoRunChange, "flip image", "flip left-right element icon", "sep=col" );
   PRM_SWITCH( noIcon, efNoRunChange, "no Icon", "don't show element icon", "sep=blockend" );

   PRM_DOUBLE( out0, efInner, "Output", "Main output", "" );

   DCL_DEFAULT_STATIC;

};

typedef TMiso* PTMiso;

#endif
