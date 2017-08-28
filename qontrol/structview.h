#pragma once
/***************************************************************************
                          structview.h  -  description
                             -------------------
    begin                : Sat Aug 12 2000
    copyright            : (C) 2000-2017 by atu
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


#include <QWidget>

class QPaintEvent;
class QMouseEvent;
class QKeyEvent;
class QMenu;
class QScrollArea;

class OutDataView;

#include "scheme.h"
#include "commonsubwin.h"
#include "cmdview.h"

class QPainter;


/**display model (scheme) structure ...
  *@author atu
  */

class StructView : public CmdView  {
  Q_OBJECT
  public:
    //* information about element to draw
    struct ElemInfo {
      // from element
      QString name, type, vis_name;
      int vis_x, vis_y, locked, ignored, onlyFirst, onlyLast, flip, noIcon, showBaloon;
      // number of signal and parametric inputs
      int n_inp, n_pinp;
      // calculated
      int xs0, ys0, xs, ys, xc, yc, flip_factor;
      int li_dst_x, pre_dst_x, li_src_x, li_pdst_y;
    };
    /** enumeration for state checking */
    enum CheckType { validCheck = 0, selCheck, linkToCheck,
                     noselCheck, moveCheck, doneCheck
    };

    StructView( CommonSubwin *a_par, Scheme *a_sch );
    // virtual ~StructView();
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
    TMiso* getSelElm() const { return selObj; }
    virtual void handleSelChange() override;
    virtual HolderData* getSelObj() const override { return selObj; }
    /** returns ptr to marked element or nullptr if none */
    TMiso* getMarkElm() const { return markObj; }
    /** returns level for aux objects selection */
    int getLevel() const { return level; }
    int checkState( CheckType ctp );
    QString getSchemeName() const;
  public Q_SLOTS:
    void update();
    void print();
    /** change selection rel: 0-abs 1-rel 2-next elm */
    void changeSel( int x, int y, int rel );
    /** change level for selecting aux objects */
    void changeLevel( int lev );
    // element related
    void qlinkElm();
    void qplinkElm();
    void unlinkElm();
    void lockElm();
    void markElm();
    void moveElm();
    void testElm2();
    // common object slots
    virtual bool addObj() override;
    virtual bool delObj() override; // handle mark == select
    virtual bool editObj() override; // ask relink after
    // virtual bool renameObj() override;
    virtual bool cloneObj() override;
    // virtual bool cutObj() override;
    // virtual bool copyObj() override;
    virtual bool pasteObj() override;
    // virtual bool infoObj() override;
    // virtual bool showTreeObj() override;
    // virtual bool testObj() override;
    // virtual bool showObj() override;
    // virtual bool showDataObj() override;
    // virtual bool exportObj() override;


  Q_SIGNALS:
    void sig_changeSel(int,int,int);
    void sig_changeLevel(int);
  protected:
    /** fill and calc info about element */
    bool fill_elmInfo( const TMiso *ob, ElemInfo &el ) const;
    virtual void paintEvent( QPaintEvent *pe );
    virtual void mousePressEvent( QMouseEvent *me );
    virtual void mouseDoubleClickEvent( QMouseEvent *me );
    virtual void keyPressEvent( QKeyEvent *ke );
    void drawAll( QPainter &p ); //* paint all in given painter
    QMenu* createPopupMenu( const QString &title, bool has_elem );

  protected:
    Scheme *sch; //* pointer to Scheme to be drawed
    bool isPrinter = false;
    int margin_l = 4, margin_t = 4; // scheme offsets from left top
    /** object size, elements margin, grid size  */
    int obj_sz = 32, el_marg = 7, grid_sz = obj_sz + el_marg * 2;
    int em = 10, em_small = 10, ex_small = 10; //* base and small font metrics
    // selections
    int sel = -1, sel_x = 0, sel_y = 0, level = 0;
    TMiso *selObj = nullptr;  //* ptr to selected object or nullptr
    TMiso *markObj = nullptr; //* prt to marked object or nullptr
    bool scheme_ro;           //* some schemes useless to edit: autocreated by template
};


// ============================== StructSubwin =======================================

class StructSubwin : public CommonSubwin {
  Q_OBJECT
  public:
   StructSubwin( QWidget *a_par, LaboDoc *a_doc, Scheme *a_sch );
   ~StructSubwin();
   virtual bool callSlot( const char *nm );
   virtual bool checkSlot( const char *nm );
  public Q_SLOTS:
   virtual int getLevel() const override;
  protected:
   QScrollArea *scrollArea = nullptr; // null only for correct layout create
   StructView *sview = nullptr;
};


// end of structview.h

