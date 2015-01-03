/***************************************************************************
  statdata.h - descrive StatData - objects for holding
  statistical data
  -------------------
begin                : 2015.01.02
copyright            : GPL (C) 2015-2015 by atu
email                : atu@nmetau.edu.ua
 ***************************************************************************/

#ifndef _STATDATA_H
#define _STATDATA_H

#include <QObject>

//* really struct, but with handles to use in JS
class StatData {
  public:
    StatData() { reset(); }
    void reset();

  public: // Sic! simple a structure
    int n;
    double s_x, s_x2, s_y, s_y2, s_xy; // sums
    double a_x, a_x2, a_y, a_y2;       // averages
    double min_x, max_x, min_y, max_y; // min and max
    int    imin_x, imax_x, imin_y, imax_y; // indexes of min and max
    double var_x, var_y, sd_x, sd_y;   // variance (sigma^2) and std deviation (sigma)
    double adev_x, adev_y;             // absolute deviation
    double corr, acorr, covar;         // coefficients of correlation, autocorrelation and covar
};

Q_DECLARE_METATYPE(StatData);

#endif
