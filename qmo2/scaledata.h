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
   ScaleData( TDataSet* apar );
   virtual ~ScaleData();
   static TDataSet* create( TDataSet *apar );
   virtual const TClassInfo* getClassInfo(void) const;
   virtual const char* getHelp(void) const;
   // X-scale
   PRM_DOUBLE1( realMinX, efRODial | efNoSave , "Real min(x)", "Real minimum X value", "" );
   PRM_DOUBLE1( plotMinX, efNRC,    "Plot min(x)", "Used for plot minimum X value", "" );
   PRM_DOUBLE1( realMaxX, efRODial | efNoSave , "Real max(x)", "Real maximum X value", "" );
   PRM_DOUBLE1( plotMaxX, efNRC,    "Plot max(x)", "Used for plot maximum X value", "" );
   PRM_INT1(       gridX, efNRC,    "Grid X", "Number of Grid lines for X", "" );
   PRM_INT1(       tickX, efNRC,    "Tick X", "Number of Ticks for X", "" );
   PRM_SWITCH1( autoScX,  efNRC,    "Autoscale X", "Automaticaly make X scale", "sep=col" );
   PRM_SWITCH1( goodScX,  efNRC,    "Good Scale X", "Make good values for X scale", "" );
   PRM_SWITCH1(    logX,  efNRC,    "Logarifmic X", "Use log(X) ", "" );
   PRM_SWITCH1(logScaleX, efNRC,    "Log. scale X", "Use logarifmic scale for X", "" );
   PRM_SWITCH1(    zeroX, efNRC,    "Zero X", "Use zero as one of X bounds", "" );
   PRM_SWITCH1(  centerX, efNRC,    "Center X", "Use symmetrical bounds for X", "" );
   // Y-scale
   PRM_DOUBLE1( realMinY, efRODial | efNoSave , "Real min(y)", "Real minimum Y value", "sep=block" );
   PRM_DOUBLE1( plotMinY, efNRC,    "Plot min(y)", "Used for plot minimum Y value", "" );
   PRM_DOUBLE1( realMaxY, efRODial | efNoSave , "Real max(y)", "Real maximum Y value", "" );
   PRM_DOUBLE1( plotMaxY, efNRC,    "Plot max(y)", "Used for plot maximum Y value", "" );
   PRM_INT1(       gridY, efNRC,    "Grid Y", "Number of Grid lines for Y", "" );
   PRM_INT1(       tickY, efNRC,    "Tick Y", "Number of Ticks for Y", "" );
   PRM_SWITCH1( autoScY,  efNRC,    "Autoscale Y", "Automaticaly make Y scale", "sep=col" );
   PRM_SWITCH1( goodScY,  efNRC,    "Good Scale Y", "Make good values for Y scale", "" );
   PRM_SWITCH1(    logY,  efNRC,    "Logarifmic Y", "Use log(Y) ", "" );
   PRM_SWITCH1(logScaleY, efNRC,    "Log. scale Y", "Use logarifmic scale for Y", "" );
   PRM_SWITCH1(    zeroY, efNRC,    "Zero Y", "Use zero as one of Y bounds", "" );
   PRM_SWITCH1(  centerY, efNRC,    "Center Y", "Use symmetrical bounds for Y", "" );
   // margins
   PRM_DOUBLE1( leftMar,  efNRC, "Left Margin %", "Value of plot left margin in %", "sep=block" );
   PRM_DOUBLE1(  topMar,  efNRC, "Top Margin %", "Value of plot top margin in %", "" );
   PRM_DOUBLE1(rightMar,  efNRC, "Right Margin %", "Value of plot right margin in %", "sep=col" );
   PRM_DOUBLE1(bottomMar, efNRC, "Bottom Margin %", "Value of plot bottom margin in %", "" );
   PRM_DOUBLE1(  maxErr,  efNRC, "Max Err", "Maximum plot error in pixels", "" );
  protected: 
   /** class decription */
   static TClassInfo class_info;
   /** help string */
   static const char* helpstr; 
  /** autoregister */
  static int registered;
  static int reg();
};

#endif  // _SCALEDATA_H
