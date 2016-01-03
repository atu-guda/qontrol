/***************************************************************************
                          tsubscheme.h  - subscheme element defs
    begin                : 2015.10.03
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

#ifndef _TSUBSCHEME_H
#define _TSUBSCHEME_H


#include "tmiso.h"
#include "scheme.h"


// helper element - get value from inner subscheme // TODO: may by a Link type
class SubOutput : public LinkedObj {
  Q_OBJECT
 public:
  DCL_CTOR(SubOutput);
  virtual ~SubOutput();
  // virtual QVariant dataObj( int col, int role = Qt::DisplayRole ) const override;
  DCL_CREATE;
  DCL_STD_INF;
  void getInput() { out0 = *p; };
  double value() const { return *p; }
  bool set_link();
 protected:

  PRM_STRING( source, efNoRunChange, "Source", "Address of signal source from subscheme", "max=128\nprops=STRING,SIMPLE,LINK\ncmpl=in"  );

  PRM_DOUBLE( out0, efInner, "input", "Output value from subscheme", "" );
  PRM_INT( linkType,  efInner | efRO, "Link type", "Describes type of link", "def=3" ); // 3=LinkBad

  static const double fake_in;
  const double *p = &fake_in;
  const LinkedObj *src_obj = nullptr;

  Q_CLASSINFO( "nameHintBase",  "o_" );
  DCL_DEFAULT_STATIC;
};


/** Provides interface to Subscheme
  *@author atu
  */

class TSubScheme : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TSubScheme);
   DCL_CREATE;
   DCL_STD_INF;
   virtual ~TSubScheme() override;
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt ) override;
   virtual int do_postRun( int good ) override;
   virtual int do_startLoop( int acnx, int acny ) override;
   virtual int do_endLoop() override;
   // TODO: all do_ -- pass to cheme
   virtual void do_post_set() override;

   /** Scheme name */
   PRM_STRING( sch_name, efNRC, "Scheme", "Scheme name", "" );

   //* ptr to shadow scheme copy
   Scheme *sch = nullptr;
   //* ptr to scheme prototype
   Scheme *sch_proto = nullptr;
   //* ptrs to inputs
   QList<InputSimple*> inputs;
   //* ptrs to outputs
   QList<SubOutput*> subouts;
   static constexpr const char* const sch_ename = "sch";

   Q_CLASSINFO( "nameHintBase",  "subs_" );
   DCL_DEFAULT_STATIC;
};

#endif


