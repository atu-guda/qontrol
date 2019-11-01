/***************************************************************************
     settingsdata.cpp - application settings structure definitions
                             -------------------
    begin                : 2015.12.09
    copyright            : (C) 2015-2019 by atu
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

#include "settingsdata.h"

const char* SettingsData::helpstr = "<H1>SettingsData</H1>\n"
 "Contains application settings\n";

QFont SettingsData::def_font;

STD_CLASSINFO(SettingsData,clpSpecial);

CTOR(SettingsData,TDataSet)
{
}

const QFont& SettingsData::getAsFont( const QString &nm ) const
{
  HolderFont *hf = getObjT<HolderFont*>( nm );
  if( !hf ) {
    return def_font;
  }
  return hf->cval();
}

void SettingsData::load()
{
  QSettings sets( ORG, PACKAGE );
  sets.beginGroup("iface");
  /* -------- flags ------------ */
  showGrid  = sets.value("view/showgrid", true ).toBool();
  showNames = sets.value("view/shownames", true ).toBool();
  showIcons = sets.value("view/showicons", true ).toBool();
  showLinks = sets.value("view/showLinks", true ).toBool();
  showMax   = sets.value("view/showmax", true ).toBool();
  /* -------- fonts ----------- */
  QFont df;
  QString dfs = df.key();

  QString v = sets.value("fonts/main", dfs ).toString();
  // qWarning() << "mainFont: " << v << " dfs: " << dfs << WHE;
  mainFont.fromString( v );

  v = sets.value("fonts/small", "Sans,7" ).toString();
  smallFont.fromString( v );
  v = sets.value("fonts/plot", "Sans,8" ).toString();
  plotFont.fromString( v );
  v = sets.value("fonts/struct", "Sans,8" ).toString();
  structFont.fromString( v );
  // misc
  editCmd  = sets.value("editCmd", "gvim -f " ).toString();
  sets.endGroup();
}

void SettingsData::save() const
{
  QSettings sets( ORG, PACKAGE );
  sets.beginGroup("iface");
  /* ---------- flags ------------- */
  sets.setValue( "view/showgrid", showGrid.cval() );
  sets.setValue( "view/shownames", showNames.cval() );
  sets.setValue( "view/showicons", showIcons.cval() );
  sets.setValue( "view/showLinks", showLinks.cval() );
  sets.setValue( "view/showmax", showMax.cval() );
  /* ---------- fonts ------------- */
  // QString v = mainFont.cval().key();
  // qWarning() << "mainFont: " << v << WHE;
  sets.setValue( "fonts/main", mainFont.get() );
  sets.setValue( "fonts/small", smallFont.get() );
  sets.setValue( "fonts/plot", plotFont.get() );
  sets.setValue( "fonts/struct", structFont.get() );
  // ------------- more -------------
  sets.setValue( "editCmd", editCmd.cval() );

  sets.endGroup();
  sets.sync();

}


DEFAULT_FUNCS_REG(SettingsData)


