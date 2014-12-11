/***************************************************************************
                          scaledata.h  -  description
                             -------------------
    begin                : 19 Jul 29 2012
    copyright            : GPL (C) 2012-2014 by atu
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

   PRM_INT(    w0,    efNRC, "width", "Initial plot width", "def=800\nmin=100\nmax=10000" );
   PRM_INT(    h0,    efNRC, "height", "Initial plot height", "def=600\nmin=100\nmax=10000\nsep=col" );
   PRM_DOUBLE( plotFactor,  efNRC, "Plot factor", "Plot scale relative to canvas", "def=1.3\nmin=0.5\nmax=10\nsep=block" );
   PRM_DOUBLE( fontSise,  efNRC, "Font size", "Base font size", "def=2.0\nmin=0.1\nmax=10" );
   PRM_DOUBLE( phi,  efNRC, "Phi", "Phi angle", "def=0.0" );
   PRM_DOUBLE( theta,  efNRC, "Theta", "Theta angle", "def=0.0" );
   PRM_DOUBLE( maxErr,  efNRC, "Max Err", "Maximum plot error", "def=0.6" );
   PRM_SWITCH( useLight,  efNRC, "Light", "Use lighting", "def=1\nsep=col" );
   PRM_SWITCH( useAlpha,  efNRC, "Use alpha", "Use transparency", "def=0" );
   PRM_DOUBLE( alpha,     efNRC, "Alpha", "transparency value", "def=0.7\nmin=0.0\nmax=1.0" );
   PRM_INT( legend_pos,   efNRC, "Legend pos", "Legend position index", "def=3\nmin=0\nmax=4" );
   PRM_COLOR( bgcolor, efNRC, "BG color", "Background color", "sep=col\ndef=#FFFFFF" );
   PRM_COLOR( axis_color, efNRC, "Axis color", "Axis color", "def=#000000" );
   PRM_COLOR( grid_color, efNRC, "Grid color", "Grid color", "def=#CCCCCC" );
   PRM_STRING( extra, efNRC, "Extra", "Extra options to plot", "max=128" );
   PRM_STRING( colorScheme, efNRC, "Color scheme", "Color Scheme for 2D plot", "max=128\ndef=BbcyrR" );
   // X-scale
   PRM_SWITCH( autoScX,  efNRC,    "Autoscale X", "Automaticaly make X scale", "def=1\nsep=block" );
   PRM_DOUBLE( realMinX, efNoDial | efNoSave , "Real min(x)", "Real minimum X value", "" );
   PRM_DOUBLE( plotMinX, efNRC,    "Plot min(x)", "Used for plot minimum X value", "" );
   PRM_DOUBLE( realMaxX, efNoDial | efNoSave , "Real max(x)", "Real maximum X value", "def=1" );
   PRM_DOUBLE( plotMaxX, efNRC,    "Plot max(x)", "Used for plot maximum X value", "def=1" );
   PRM_DOUBLE(    gridX, efNRC,    "Grid X", "Number of Grid lines for X (- =step)", "def=0" );
   PRM_INT(       tickX, efNRC,    "Tick X", "Number of Ticks for X", "def=5" );
   PRM_SWITCH( goodScX,  efOld,    "Good Scale X", "Make good values for X scale", "def=1" );
   PRM_SWITCH(    logX,  efOld,    "Logarifmic X", "Use log(X) ", "" );
   PRM_SWITCH(logScaleX, efOld,    "Log. scale X", "Use logarifmic scale for X", "" );
   PRM_SWITCH(    zeroX, efOld,    "Zero X", "Use zero as one of X bounds", "" );
   PRM_SWITCH(  centerX, efOld,    "Center X", "Use symmetrical bounds for X", "" );
   // Y-scale
   PRM_SWITCH( autoScY,  efNRC,    "Autoscale Y", "Automaticaly make Y scale", "sep=col\ndef=1" );
   PRM_DOUBLE( realMinY, efNoDial | efNoSave , "Real min(y)", "Real minimum Y value", "" );
   PRM_DOUBLE( plotMinY, efNRC,    "Plot min(y)", "Used for plot minimum Y value", "" );
   PRM_DOUBLE( realMaxY, efNoDial | efNoSave , "Real max(y)", "Real maximum Y value", "def=1" );
   PRM_DOUBLE( plotMaxY, efNRC,    "Plot max(y)", "Used for plot maximum Y value", "def=1" );
   PRM_DOUBLE(    gridY, efNRC,    "Grid Y", "Number of Grid lines for Y (- =step)", "def=0" );
   PRM_INT(       tickY, efNRC,    "Tick Y", "Number of Ticks for Y", "def=5" );
   PRM_SWITCH( goodScY,  efOld,    "Good Scale Y", "Make good values for Y scale", "def=1" );
   PRM_SWITCH(    logY,  efOld,    "Logarifmic Y", "Use log(Y) ", "" );
   PRM_SWITCH(logScaleY, efOld,    "Log. scale Y", "Use logarifmic scale for Y", "" );
   PRM_SWITCH(    zeroY, efOld,    "Zero Y", "Use zero as one of Y bounds", "" );
   PRM_SWITCH(  centerY, efOld,    "Center Y", "Use symmetrical bounds for Y", "" );
   // margins
   PRM_DOUBLE( leftMar,  efOld, "Left Margin %", "Value of plot left margin in %", "sep=block\ndef=0.1" );
   PRM_DOUBLE(  topMar,  efOld, "Top Margin %", "Value of plot top margin in %", "def=0.02" );
   PRM_DOUBLE(rightMar,  efOld, "Right Margin %", "Value of plot right margin in %", "sep=col\ndef=0.1" );
   PRM_DOUBLE(bottomMar, efOld, "Bottom Margin %", "Value of plot bottom margin in %", "def=0.1" );
  protected:
   DCL_DEFAULT_STATIC;
};

#endif  // _SCALEDATA_H
