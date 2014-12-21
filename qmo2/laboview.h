/***************************************************************************
                          laboview.h  -  description
                             -------------------
    begin                : Mon Jul 31 16:51:57 EEST 2000
    copyright            : (C) 2000-2014 by atu
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

#ifndef LABOVIEW_H
#define LABOVIEW_H

// include files for Qt
#include <QWidget>

class QResizeEvent;
class QCloseEvent;
class QAbstractItemView;

class LaboDoc;
class StructView;
class OutView;
class OutDataView;
class GraphView;
class GraphDataView;
class TRootData;
class TModel;
class StatusModel;
class QScrollArea;
class QTreeView;
class QListView;
class SimulView;
class ContScheme;
class Scheme;
class ContOut;
class ContGraph;
class ContSimul;

class HolderData;
class TMiso;
class SimulModel;

/**
 * This class provides an base for LaboDoc view.
 */
class LaboView : public QWidget
{
   Q_OBJECT
   friend class LaboDoc;
 public:
   /** enumeration for chate checking */
   enum CheckType { validCheck = 0, selCheck, linkToCheck,
                     noselCheck, moveCheck, doneCheck
   };
   LaboView( LaboDoc* pDoc, QWidget* parent );
   ~LaboView();
   /** returns a pointer to the document connected to the view*/
   LaboDoc *getDocument() const { return  doc; };
   /** gets called to redraw the document contents if it has been modified */
   void update();
   // /** implement preferred size */
   // QSize sizeHint();
   /** inner size for structure view */
   QSize svSize() const;

   const QString& currentFile() const;

   //* requiest confirmation to delete, true = ok;
   bool confirmDelete( const QString &obj, const QString &nm );
   //* edit some model object
   bool editObj( HolderData *obj, bool resetModel = true );
   //* returns name of current object in given view
   QString getSelName( QAbstractItemView *view );
   //* show std error box
   void showError( const QString &s );
   //* show std warning box
   void showWarn( const QString &s );

 signals:
   void viewChanged();

 public slots:
   /** called whan we need to update views */
   void updateViews();
   /** change selection rel: 0-abs 1-rel 2-next elm */
   void changeSel( int x, int y, int rel );
   /** change level for selecting aux objects */
   void changeLevel( int lev );
   /** contains the implementation for printing functionality  */
   void print();
   // element related
   void newElm();
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

   // outs related
   void newOut();
   void delOut();
   void editOut();
   void renameOut();
   void selectOut();
   void showOutData();
   void exportOut();

   // graphs related
   void newGraph();
   void delGraph();
   void editGraph();
   void renameGraph();
   void selectGraph();
   void showGraph();
   void graphAddOut();
   void showGraphData();
   void exportGraphData();
   void cloneGraph();

   // simulation releated
   void newSimul();
   void delSimul();
   void editSimul();
   void renameSimul();
   void selectSimul();
   void setActiveSimul();
   void cloneSimul();

   // model related
   void editModel();
   void showTreeModel();

   // runs
   void runRun();
   void resetModel();
   void runScript();
   QString runModelScript();
   void initEngine();

 public:
   /** returns document rootdata */ // TODO: remove?
   // TRootData* getRoot() { return root; }
   /** returns document model */
   TModel* getModel() { return model; }
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
 protected:
   virtual void closeEvent( QCloseEvent* );
   virtual void resizeEvent( QResizeEvent* );
   int checkState( CheckType ctp );
   //* call engine and returns result, casted to QString
   QString runScript( const QString& script );
 protected:
   QScrollArea *scrollArea;
   StructView *sview = nullptr;
   OutDataView *outs_view = nullptr;
   GraphDataView *plots_view = nullptr;
   SimulView *sims_view = nullptr;
   StatusModel *stam = nullptr;

   // document and its main parts
   LaboDoc *doc;
   TRootData *root;
   TModel *model;
   ContScheme *schems;
   Scheme *main_s;
   ContOut *outs;
   ContGraph *plots;
   ContSimul *sims;

   int sel = -1, sel_x = 0, sel_y = 0, level = 0;
   // TODO: from file (config)
   QString scr = R"(main_s.add_obj_datas("TLinear","ob","vis_x=4\nvis_y=1\na0=3.14");)";
   /** prt to selected object or nullptr */
   TMiso *selObj = nullptr;
   /** prt to marked object or nullptr */
   TMiso *markObj = nullptr;
   /** name of marked element */
   QString markName;
};

#endif

// end of laboview.h

