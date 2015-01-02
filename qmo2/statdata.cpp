/***************************************************************************
                          statdata.h - descrive StatData - objects for holding
                          statistical data
                             -------------------
    begin                : 2015.01.02
    copyright            : GPL (C) 2015-2015 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/
#include "statdata.h"

void StatData::reset()
{
  n=0;
  s_x = s_x2 = s_y = s_y2 = s_xy = a_x = a_x2 = a_y = a_y2
      = dis_x = dis_y = sigma_x = sigma_y = 0.0;
}

