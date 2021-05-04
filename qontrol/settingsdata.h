#pragma once
/***************************************************************************
     settingsdata.h - application settings structure declration
                             -------------------
    begin                : 2015.12.09
    copyright            : (C) 2015-2016 by atu
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

class SettingsData : public TDataSet {
  Q_OBJECT
 public:
   DCL_CTOR(SettingsData);
   DCL_CREATE;
   DCL_STD_INF;
   void load();
   void save() const;
   const QFont& getAsFont( const QString &nm ) const;

 protected:
   PRM_SWITCH( showGrid,  0, "show Grid",   "Show scheme grid",  "def=1" );
   PRM_SWITCH( showNames, 0, "show Names",  "Show elements names",  "def=1" );
   PRM_SWITCH( showIcons, 0, "show Icons",  "Show scheme icon", "def=1" );
   PRM_SWITCH( showLinks, 0, "show Links",  "Show element links", "def=1" );
   PRM_SWITCH( showMax,   0, "Maximize",    "Maximize main window at start", "def=0" );

   PRM_FONT( mainFont,    0, "main Font", "Main application font", "sep=col\ndef=DEFAULT" );
   PRM_FONT( smallFont,   0, "Small font", "Small application font", "def=TINY" );
   PRM_FONT( plotFont,    0, "Plot font", "Application font for plotting", "def=SMALL" );
   PRM_FONT( structFont,  0, "Struct font", "Font for scheme representing", "def=SMALL" );

   PRM_STRING( editCmd,   0, "Edit command", "Command to edit file", "sep=block\nncol=-1\ndef=gvim" );
   PRM_STRING( editArgs,  0, "Edit args",    "Additional arguments", "ncol=-1\ndef=-f" );

   Q_CLASSINFO( "nameHintBase",  "settings" );
   DCL_DEFAULT_STATIC;
   static QFont def_font;

};


