/***************************************************************************
                          rootdata.h  -  description lcass TRootData
                             -------------------
    begin                : Thu Apr 20 16:55:07 2000
    copyright            : GPL (C) 2000-2013 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#ifndef _ROOTDATA_H
#define _ROOTDATA_H

#include "datacont.h"

/** class holding other classes descriptions
    so can create all registered class objects
*/
class TRootData : public TDataContainer {
  Q_OBJECT
 public:
   DCL_CTOR(TRootData); 
   virtual ~TRootData() override;
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   DCL_DEFAULT_STATIC;
};

#endif  // _ROOTDATA_H


