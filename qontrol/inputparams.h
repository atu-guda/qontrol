/***************************************************************************
                          inputparams.h
                             -------------------
    begin                : 2013.09.01
    copyright            : (C) 2000-2015 by atu
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

#ifndef INPUTPARAMS_H
#define INPUTPARAMS_H

#include "datacont.h"


/**Contains parametric inputs of element
  *@author atu
  */

class InputParams : public TDataContainer  {
  Q_OBJECT
 public:
  DCL_CTOR(InputParams);
  virtual ~InputParams() override;
  DCL_CREATE;
  DCL_STD_INF;

  void prepare();
  int apply();
  int apply_pre();

 protected:
  // cache links before run
  std::vector<const double*> srcs; // ordinary sources;
  std::vector<double*> targets; // ordinary targets;
  std::vector<const double*> srcs_p; // pre-stage sources;
  std::vector<double*> targets_p; // pre-stage targets;
  DCL_DEFAULT_STATIC;

};

#endif

