/***************************************************************************
                          structview.h  -  description
                             -------------------
    begin                : Sat Aug 12 2000
    copyright            : (C) 2000-2016 by atu
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
      QString name, type;
      int vis_x, vis_y, ord, locked, ignored, onlyFirst, onlyLast, flip, noIcon;
      // number of signal and parametric inputs
      int n_inp, n_pinp;
      // calculated
      int xs0, ys0, xs, ys, xc, yc, flip_factor;
      int li_dst_x, pre_dst_x, li_src_x, li_pdst_y;
    };
    /** enumeration for chate checking */
    enum CheckType { validCheck = 0, selCheck, linkToCheck,
                     noselCheck, moveCheck, doneCheck
    };

    StructView( CommonSubwin *a_par, Scheme *a_sch );
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
    TMiso* getSelElm() const { return selObj; }
    virtual void handleSelChange() override;
    virtual HolderData* getSelObj() const override { return selObj; };
    /** returns ptr to marked element or nullptr if none */
    TMiso* getMarkElm() const { return markObj; }
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
    void qlinkElm();
    void qplinkElm();
    void unlinkElm();
    void lockElm();
    void ordElm();
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


  signals:
    void sig_changeSel(int,int,int);
    void sig_changeLevel(int);
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
    /** type of output device */
    int devTp = 0;
    /** grid size, left, top margin size, object size,  elements margin */
    int grid_sz = 46, lm = 4, tm = 4, obj_sz = 32, el_marg = 1, em_small = 10, ex_small = 10;
    // selections
    int sel = -1, sel_x = 0, sel_y = 0, level = 0;
    TMiso *selObj = nullptr;  //* ptr to selected object or nullptr
    TMiso *markObj = nullptr; //* prt to marked object or nullptr
    int em = 10;
    bool ro;
};


// ============================== StructSubwin =======================================

class StructSubwin : public CommonSubwin {
  Q_OBJECT
  public:
   StructSubwin( QWidget *a_par, LaboDoc *a_doc, Scheme *a_sch );
   ~StructSubwin();
   virtual bool callSlot( const char *nm );
   virtual bool checkSlot( const char *nm );
  public slots:
   virtual int getLevel() const override;
  protected:
   QScrollArea *scrollArea = nullptr; // null only for correct layout create
   StructView *sview = nullptr;
};

#endif

// end of structview.h

