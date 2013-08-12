/***************************************************************************
                          datacont.h  -  description
                             -------------------
    begin                : Thu Apr 20 2000
    copyright            : GPL (C) 2000-2013 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/



#ifndef _DATACONT_H
#define _DATACONT_H

#include "dataset.h"

/** this class allows insertion any type of objects
  *@author atu
  */

class TDataContainer : public TDataSet  {
  Q_OBJECT
 public:
  DCL_CTOR(TDataContainer);
  virtual ~TDataContainer() override;
  DCL_CREATE;
  DCL_STD_INF;
 protected: 
  DCL_DEFAULT_STATIC;
};

#endif // _DATACONT_H
