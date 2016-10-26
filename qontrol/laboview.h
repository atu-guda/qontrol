#pragma once
/***************************************************************************
                          laboview.h  -  description
                             -------------------
    begin                : Mon Jul 31 16:51:57 EEST 2000
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


#include "commonsubwin.h"

class QResizeEvent;
class QCloseEvent;
class QAbstractItemView;
class QItemSelectionModel;

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
class SchemeView;
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
class LaboView : public CommonSubwin
{
   Q_OBJECT
   friend class LaboDoc; // TODO: remove
 public:
   /** enumeration for state checking */
   enum CheckType { validCheck = 0, selCheck, linkToCheck,
                     noselCheck, moveCheck, doneCheck
   };
   LaboView( LaboDoc* pDoc, QWidget* parent );
   ~LaboView();
   /** gets called to redraw the document contents if it has been modified */
   void update();
   // /** implement preferred size */
   // QSize sizeHint();
   /** inner size for structure view */
   QSize svSize() const;

   virtual bool callSlot( const char *nm );
   virtual bool checkSlot( const char *nm );

 Q_SIGNALS:
   void viewChanged();

 public Q_SLOTS:
   /** called whan we need to update views */
   void updateViews();
   /** contains the implementation for printing functionality  */
   void print();
   // element related
   void qlinkElm();
   void qplinkElm();
   void unlinkElm();
   void lockElm();
   void markElm();
   void moveElm();
   void testElm2();
   // common object
   bool actionObj( const char *action ); // common actions: not slot?
   bool addObj();
   bool delObj();
   bool editObj();
   bool cutObj();
   bool copyObj();
   bool pasteObj();
   bool renameObj();
   bool cloneObj();
   bool infoObj();
   bool showTreeObj();
   bool testObj();

   // outs related
   void addOut();
   void showOutData();
   void exportOut();

   // graphs related
   void addGraph();
   void showGraph();
   void graphAddOut();
   void showGraphData();
   void exportGraphData();

   // simulation releated
   void addSimul();
   void setActiveSimul();

   // model/scheme related
   void editModel();
   void showTreeModel();
   void addScheme();
   void importAllSchemes();

   // runs
   void runRun();
   void resetModel();
   void runScript();
   QString runModelScript();
   void initEngine();

   /** returns level for aux objects selection */
   virtual int getLevel() const override;

 public:
   /** returns document model */
   TModel* getModel() { return model; } // TODO: remove?
   /** returns x-coordinate for selected cell */
   int getSelX() const;
   /** returns y-coordinate for selected cell */
   int getSelY() const;
   /** returns number of selected element or -1 if none */
   int getSel() const;
   /** returns ptr to selected element or nullptr if none */
   TMiso* getSelElm() const;
   /** returns ptr to marked element or nullptr if none */
   TMiso* getMarkElm() const;
 protected:
   virtual void closeEvent( QCloseEvent* );
   virtual void resizeEvent( QResizeEvent* );
   // int checkState( CheckType ctp );

 protected:
   QScrollArea *scrollArea;
   StructView *sview = nullptr;
   OutDataView *outs_view = nullptr;
   GraphDataView *plots_view = nullptr;
   SimulView *sims_view = nullptr;
   SchemeView *schems_view = nullptr;
   StatusModel *stam = nullptr;

   TRootData *root;
   TModel *model;
   ContScheme *schems;
   Scheme *main_s;
   ContOut *outs;
   ContGraph *plots;
   ContSimul *sims;

   //* copy of LaboWin em;
   int em = 10;
};


// end of laboview.h

