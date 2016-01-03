/***************************************************************************
                          contout.h  -  contrainer for TOutArr
                             -------------------
    begin                : 2014.11.14
    copyright            : (C) 2014-2016 by atu
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

#ifndef _CONTOUT_H
#define _CONTOUT_H

#include "dataset.h"
#include "scriptfuncs.h"

class TOutArr;

class ContOut : public TDataSet {
   Q_OBJECT
  public:
   DCL_CTOR(ContOut);
   virtual ~ContOut() override;
   DCL_CREATE;
   DCL_STD_INF;

   //* called before all runs
   int preRun( int run_tp, int an, int anx, int any, double adt );
   //* will be called after all actions
   int postRun( int good );
   //* called before each inner param loop
   int startLoop( int acnx, int acny );
   /** will be called after each inner loop */
   int endLoop( int acnx, int acny );

   //* get all needed ordinary (outSimple) values
   void takeAllVals();

   // just to test return pair to JS
   Q_INVOKABLE TripleF calcTest() const { return TripleF{ 0.2, 0.5, 3.14 }; }
   //* returns: a: slope, b: shift, c: correlation if need
   Q_INVOKABLE TripleF regre( const QString &nm_x, const QString &nm_y, bool need_corr ) const;
   Q_INVOKABLE double  corr( const QString &nm_x, const QString &nm_y ) const;
   Q_INVOKABLE double  covar( const QString &nm_x, const QString &nm_y ) const;

   //* linerar transformation from one array to other a*x+b
   Q_INVOKABLE int  transLin( const QString &nm_in, const QString &nm_out,
                              double a, double b );

   // fft tranform
   Q_INVOKABLE int fft( const QString &nm_in, const QString &nm_omega,
                        const QString &nm_a, const QString &nm_phi, double ome_max );
   Q_INVOKABLE int fftc( const QString &nm_in, const QString &nm_omega,
                        const QString &nm_re, const QString &nm_im, double ome_max );
  protected:
   int prep_2in( const QString &nm_x, const QString &nm_y,
                 const dvector **p_d_x, const dvector **p_d_y ) const;
   int fftx( const QString &nm_in, const QString &nm_omega,
             const QString &nm_a, const QString &nm_phi, double ome_max, bool cmpl );
   std::vector<TOutArr*> vo;
   DCL_DEFAULT_STATIC;
};

#endif

