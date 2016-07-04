#pragma once
/***************************************************************************
     askedparam.h - structure AskedParam to describe params to set before run
                             -------------------
    begin                : 2016.07.04
    copyright            : GPL (C) 2012-2016 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include <QString>

struct AskedParam {
  QString name;
  double saved;
  double cur;
  double *targ = nullptr;
};

