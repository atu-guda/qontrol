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
class StatData : public QObject {
  Q_OBJECT
  public:
    StatData() { reset(); }
    Q_INVOKABLE void reset();

    Q_PROPERTY( int    n         READ get_n    ); //* number(x)

    Q_PROPERTY( double s_x       READ get_s_x  ); //* sum(x)
    Q_PROPERTY( double s_x2      READ get_s_x2 ); //* sum(x^2)
    Q_PROPERTY( double s_y       READ get_s_y  ); //* sum(y)
    Q_PROPERTY( double s_y2      READ get_s_y2 ); //* sum(y^2)
    Q_PROPERTY( double s_xy      READ get_s_xy ); //* sum(x*y)
    //
    Q_PROPERTY( double a_x       READ get_a_x  ); //* average(x)
    Q_PROPERTY( double a_x2      READ get_a_x2 ); //* average(x^2)
    Q_PROPERTY( double a_y       READ get_a_y  ); //* average(y)
    Q_PROPERTY( double a_y2      READ get_a_y2 ); //* average(y^2)
    //
    Q_PROPERTY( double dis_x     READ get_dis_x  ); //* variance(x)
    Q_PROPERTY( double dis_y     READ get_dis_y  ); //* variance(y)
    Q_PROPERTY( double sigma_x   READ get_sigma_x  ); //* std der(x)
    Q_PROPERTY( double sigma_y   READ get_sigma_y  ); //* std der(x)
  public slots:
    int get_n() const { return n; }
    double get_s_x() const  { return s_x; }
    double get_s_x2() const { return s_x2; }
    double get_s_y() const  { return s_y; }
    double get_s_y2() const { return s_y2; }
    double get_s_xy() const { return s_xy; }
    double get_a_x() const  { return a_x; }
    double get_a_x2() const { return a_x2; }
    double get_a_y() const  { return a_y; }
    double get_a_y2() const { return a_y2; }
    double get_dis_x() const { return dis_x; }
    double get_dis_y() const { return dis_y; }
    double get_sigma_x() const { return sigma_x; }
    double get_sigma_y() const { return sigma_y; }
  public: // Sic! simple a structure
    int n;
    double s_x, s_x2, s_y, s_y2, s_xy, a_x, a_x2, a_y, a_y2, dis_x, dis_y, sigma_x, sigma_y;
};

#endif
