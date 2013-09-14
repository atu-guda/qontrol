/***************************************************************************
                          scaledata.h  -  description
                             -------------------
    begin                : 19 Jul 29 2012
    copyright            : GPL (C) 2012-2013 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#ifndef _SCALEDATA_H
#define _SCALEDATA_H

#include "dataset.h"

/** Information about scale: really struct */
class ScaleData : public TDataSet
{
  Q_OBJECT
  public:
   DCL_CTOR(ScaleData);
   virtual ~ScaleData() override;
   DCL_CREATE;
   DCL_STD_INF;
   // X-scale
   PRM_DOUBLE( realMinX, efRODial | efNoSave , "Real min(x)", "Real minimum X value", "" );
   PRM_DOUBLE( plotMinX, efNRC,    "Plot min(x)", "Used for plot minimum X value", "" );
   PRM_DOUBLE( realMaxX, efRODial | efNoSave , "Real max(x)", "Real maximum X value", "def=1" );
   PRM_DOUBLE( plotMaxX, efNRC,    "Plot max(x)", "Used for plot maximum X value", "def=1" );
   PRM_INT(       gridX, efNRC,    "Grid X", "Number of Grid lines for X", "def=10" );
   PRM_INT(       tickX, efNRC,    "Tick X", "Number of Ticks for X", "def=5" );
   PRM_SWITCH( autoScX,  efNRC,    "Autoscale X", "Automaticaly make X scale", "sep=col\ndef=1" );
   PRM_SWITCH( goodScX,  efNRC,    "Good Scale X", "Make good values for X scale", "def=1" );
   PRM_SWITCH(    logX,  efNRC,    "Logarifmic X", "Use log(X) ", "" );
   PRM_SWITCH(logScaleX, efNRC,    "Log. scale X", "Use logarifmic scale for X", "" );
   PRM_SWITCH(    zeroX, efNRC,    "Zero X", "Use zero as one of X bounds", "" );
   PRM_SWITCH(  centerX, efNRC,    "Center X", "Use symmetrical bounds for X", "" );
   // Y-scale
   PRM_DOUBLE( realMinY, efRODial | efNoSave , "Real min(y)", "Real minimum Y value", "sep=block" );
   PRM_DOUBLE( plotMinY, efNRC,    "Plot min(y)", "Used for plot minimum Y value", "" );
   PRM_DOUBLE( realMaxY, efRODial | efNoSave , "Real max(y)", "Real maximum Y value", "def=1" );
   PRM_DOUBLE( plotMaxY, efNRC,    "Plot max(y)", "Used for plot maximum Y value", "def=1" );
   PRM_INT(       gridY, efNRC,    "Grid Y", "Number of Grid lines for Y", "def=10" );
   PRM_INT(       tickY, efNRC,    "Tick Y", "Number of Ticks for Y", "def=5" );
   PRM_SWITCH( autoScY,  efNRC,    "Autoscale Y", "Automaticaly make Y scale", "sep=col\ndef=1" );
   PRM_SWITCH( goodScY,  efNRC,    "Good Scale Y", "Make good values for Y scale", "def=1" );
   PRM_SWITCH(    logY,  efNRC,    "Logarifmic Y", "Use log(Y) ", "" );
   PRM_SWITCH(logScaleY, efNRC,    "Log. scale Y", "Use logarifmic scale for Y", "" );
   PRM_SWITCH(    zeroY, efNRC,    "Zero Y", "Use zero as one of Y bounds", "" );
   PRM_SWITCH(  centerY, efNRC,    "Center Y", "Use symmetrical bounds for Y", "" );
   // margins
   PRM_DOUBLE( leftMar,  efNRC, "Left Margin %", "Value of plot left margin in %", "sep=block\ndef=0.1" );
   PRM_DOUBLE(  topMar,  efNRC, "Top Margin %", "Value of plot top margin in %", "def=0.02" );
   PRM_DOUBLE(rightMar,  efNRC, "Right Margin %", "Value of plot right margin in %", "sep=col\ndef=0.1" );
   PRM_DOUBLE(bottomMar, efNRC, "Bottom Margin %", "Value of plot bottom margin in %", "def=0.1" );
   PRM_DOUBLE(  maxErr,  efNRC, "Max Err", "Maximum plot error in pixels", "def=0.5" );
  protected:
   DCL_DEFAULT_STATIC;
};

#endif  // _SCALEDATA_H
