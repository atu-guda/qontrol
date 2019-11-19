#pragma once
/***************************************************************************
                          textrlatch.h  -  description
                             -------------------
    begin                : Mon Aug 28 2000
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


#include <tmiso.h>



/**detector of extremums, average and amplitude
  *@author atu
  */

class TExtrLatch : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TExtrLatch);
   DCL_CREATE;
   DCL_STD_INF;

   enum ExtrType {
     extrMax, extrMin, extrModMax, extrAver, extrAmpl
   };
   Q_ENUM(ExtrType);
   Q_CLASSINFO( "enum_ExtrType_0", "Max" );        // extrMax
   Q_CLASSINFO( "enum_ExtrType_1", "Min" );        // extrMin
   Q_CLASSINFO( "enum_ExtrType_2", "Max||" );      // extrModMax
   Q_CLASSINFO( "enum_ExtrType_3", "Average" );    // extrAver
   Q_CLASSINFO( "enum_ExtrType_4", "Amplitude" );  // extrAmpl
 protected:
   /** main computation function */
   virtual double f() noexcept override;
   /** reimplemented from TMiso to reset */
   virtual int miso_startLoop( long acnx, long acny ) override;
   void reset_vars();

   PRM_LIST(       type, efNRC, "&Type", "Type of catching extremum", "enum=ExtrType" );
   PRM_SWITCH( useLocal, efNRC, "&Local extr", "Catch local extremums", "sep=col" );
   PRM_SWITCH( useFirst, efNRC, "Only &first", "Catch only first local extremum", "");
   PRM_SWITCH(     outT, efNRC, "Output T", "Output time of catched extremum, not value", "" );
   PRM_PARAMD(   tStart, efNRC, "Time &start", "Time start", "sep=col");

   PRM_INT(    wasExtr, efInner, "wasExtr", "flag: extremum detected", "" );
   PRM_INT(    isStart, efInner, "isStart", "Was start", "");
   /** holders for extr levels and old values */
   PRM_DOUBLE( u_max,   efInner, "u_max", "u_max", "" );
   PRM_DOUBLE( t_max,   efInner, "t_max", "t_max", "" );
   PRM_DOUBLE( u_min,   efInner, "u_min", "u_min", "" );
   PRM_DOUBLE( t_min,   efInner, "t_min", "t_min", "" );
   PRM_DOUBLE( u_abs,   efInner, "u_abs", "u_abs", "" );
   PRM_DOUBLE( t_abs,   efInner, "t_abs", "t_abs", "" );
   PRM_DOUBLE(  u_ex,   efInner, "u_ex",  "u_ex" , "" );
   PRM_DOUBLE(  t_ex,   efInner, "t_ex",  "t_ex" , "" );

   PRM_INPUT(   in_u,         0, "&in_u", "Main input",  "sep=block" );
   PRM_LOGICIN(in_rst,        0, "rst", "Signal to reset", "sep=col" );

   double  u_old = 0, u_old2 = 0, t_old = 0;

   Q_CLASSINFO( "nameHintBase",  "extr_" );
   DCL_DEFAULT_STATIC;
};



