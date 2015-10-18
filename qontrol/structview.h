/***************************************************************************
                          structview.h  -  description
                             -------------------
    begin                : Sat Aug 12 2000
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

#ifndef STRUCTVIEW_H
#define STRUCTVIEW_H

#include <QWidget>

class QPaintEvent;
class QMouseEvent;
class QKeyEvent;
class QMenu;

class OutDataView;

#include "scheme.h"
#include "laboview.h"
#include "commonsubwin.h"

class QPainter;


/**display model (scheme) structure ...
  *@author atu
  */

class StructView : public QWidget  {
  Q_OBJECT
  public:
    //* information about element to draw
    struct ElemInfo {
      // from element
      QString name, type;
      int vis_x, vis_y, ord, locked, onlyFirst, onlyLast, flip, noIcon;
      // number of signal and parametric inputs
      int n_inp, n_pinp;
      // calculated
      int xs0, ys0, xs, ys, xc, yc, flip_factor;
      int li_dst_x, pre_dst_x, li_src_x, li_pdst_y;
      const InputParams *pis;
    };
    /** enumeration for chate checking */
    enum CheckType { validCheck = 0, selCheck, linkToCheck,
                     noselCheck, moveCheck, doneCheck
    };

    StructView( QWidget *a_par, Scheme *a_sch,  LaboView *mview, OutDataView *a_oview );
    virtual ~StructView();
    /** print contents */
    void printAll();
    /** grid size */
    int getGridSize() const { return grid_sz; }
    /** bound of all elements in model */
    QSize getElemsBound() const;
    virtual QSize sizeHint () const;
    /** gets coordinates on selected element */
    QPoint getSelCoords() const;
    /** returns x-coordinate for selected cell */
    int getSelX() const { return sel_x; }
    /** returns y-coordinate for selected cell */
    int getSelY() const { return sel_y; }
    /** returns number of selected element or -1 if none */
    int getSel() const { return sel; }
    /** returns ptr to selected element or nullptr if none */
    TMiso* getSelObj() const { return selObj; }
    /** returns ptr to marked element or nullptr if none */
    TMiso* getMarkObj() const { return markObj; }
    /** returns level for aux objects selection */
    int getLevel() const { return level; }
    int checkState( CheckType ctp );
    QString getSchemeName() const;
  public slots:
    void update();
    void print();
    /** change selection rel: 0-abs 1-rel 2-next elm */
    void changeSel( int x, int y, int rel );
    /** change level for selecting aux objects */
    void changeLevel( int lev );
    // element related
    void addElm();
    void delElm();
    void editElm();
    void renameElm();
    void qlinkElm();
    void qplinkElm();
    void unlinkElm();
    void lockElm();
    void ordElm();
    void markElm();
    void moveElm();
    void infoElm();
    void showTreeElm();
    void testElm1();
    void testElm2();
    void cutElm();
    void copyElm();
    void pasteElm();

  signals:
    void sig_changeSel(int,int,int);
    void sig_changeLevel(int);
    void viewChanged();
  protected:
    /** fill and calc info about element */
    bool fill_elmInfo( const TMiso *ob, ElemInfo &el ) const;
    /** draw itself */
    virtual void paintEvent( QPaintEvent *pe );
    /** reaction to mouse */
    virtual void mousePressEvent( QMouseEvent *me );
    /** reaction on mouse doubleclick */
    virtual void mouseDoubleClickEvent( QMouseEvent *me );
    /** reaction to keyboard */
    virtual void keyPressEvent( QKeyEvent *ke );
    /** paint all in given painter */
    void drawAll( QPainter &p );
    /** create popup menu */
    QMenu* createPopupMenu( const QString &title, bool has_elem );

  protected:
    /** pointer to TModel to be drawed */
    Scheme *sch;
    /** main view (parent) TODO: replace with less */
    LaboView *mainview;
    OutDataView *oview; // pointer to releted view or nullptr
    /** type of output device */
    int devTp = 0;
    /** grid size, left, top margin size, object size,  elements margin */
    int grid_sz = 46, lm = 4, tm = 4, obj_sz = 32, el_marg = 1, em_small = 10, ex_small = 10;
    // selections
    int sel = -1, sel_x = 0, sel_y = 0, level = 0;
    TMiso *selObj = nullptr;  //* ptr to selected object or nullptr
    TMiso *markObj = nullptr; //* prt to marked object or nullptr
    int em = 10;
};


// ============================== StructSubwin =======================================

class StructSubwin : public CommonSubwin {
  public:
   StructSubwin( QWidget *a_par, LaboDoc *a_doc, Scheme *a_sch,  LaboView *mview, OutDataView *a_oview  );
   ~StructSubwin();
   virtual bool callSlot( const char *nm );
   virtual bool checkSlot( const char *nm );
  protected:
   QScrollArea *scrollArea = nullptr; // null only for forrect layout create
   StructView *sview = nullptr;
};

#endif

// end of structview.h

