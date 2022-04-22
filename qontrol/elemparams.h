#pragma once
/***************************************************************************
  elemparams.h - element parametets description
                             -------------------
    begin                : 2017.09.03 (copy from tmiso.h)
    copyright            : (C) 2017-2022 by atu
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

#include "dataset.h"

class ElemParams : public TDataSet  {
  Q_OBJECT
 public:
   DCL_CTOR(ElemParams);
   // virtual ~ElemParams() override;
   DCL_CREATE;
   DCL_STD_INF;

 public: // SIC, this is a structure!
   PRM_INT(          vis_x, efNRC,         "Visual x", "X-coordinate of element in scheme", "min=0\nmax=1024" );
   PRM_INT(          vis_y, efNRC,         "Visual y", "Y-coordinate of element in scheme", "min=0\nmax=1024" );
   PRM_SWITCH( noCalcStart, efNRC, "No calc at start", "Disable out0 at startLoop ", ""  );

   PRM_SWITCH(      locked, efNRC,           "Locked", "Bypass out0_init to output"              , "sep=col"  );
   PRM_SWITCH(     ignored, efNRC,          "Ignored", "Ignore element while run"                , ""  );
   PRM_SWITCH(   onlyFirst, efNRC,       "only First", "Process element only at first iteration" , "" );
   PRM_SWITCH(    onlyLast, efNRC,        "only Last", "Process element only at last iteration"  , "" );
   PRM_SWITCH(        flip, efNRC,       "flip image", "flip left-right element icon"            , "sep=col" );
   PRM_SWITCH(      noIcon, efNRC,          "no Icon", "don't show element icon"                 , "" );
   PRM_SWITCH(  showBaloon, efNRC,      "show Baloon", "Show baloon on scheme with out0"         , "" );

   PRM_STRING(   baloonStr, efNRC,    "Baloon string", "Baloon format string", "sep=block\nmax=256\nncol=-1");

   PRM_SWITCH(     showBar, efNRC,         "show Bar", "Show vertial bar near object"            , "sep=block" );
   PRM_STRING(    barValue, efNRC,        "bar Value", "Source for bar value (def=out0)"         , "" );
   PRM_DOUBLE(      barMin, efNRC,          "bar Min", "Bar minimal value"                       , "def=0" );
   PRM_DOUBLE(      barMax, efNRC,          "bar Max", "Bar maximum value"                       , "def=1" );
   PRM_DOUBLE(       barX0, efNRC,           "bar X0", "Bar X start position (in obj size)"      , "def=0.3\nsep=col" );
   PRM_DOUBLE(       barY0, efNRC,           "bar Y0", "Bar Y start position (in obj size)"      , "def=-0.5" );
   PRM_DOUBLE(        barW, efNRC,        "bar Width", "Bar width (in obj size)"                 , "def=0.2" );
   PRM_DOUBLE(        barH, efNRC,       "bar Heigth", "Bar height (in obj size)"                , "def=1.0" );
   PRM_COLOR(     barColor, efNRC,        "bar Color", "Bar color"                               , "sep=col\ndef=red" );
 protected:
   // virtual int do_postFileRead() override;

   DCL_DEFAULT_STATIC;
};

