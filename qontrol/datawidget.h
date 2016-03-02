#pragma once
/***************************************************************************
                          datawidget.h  - widgets for DataDialog
                             -------------------
    begin                : 2012.03.27
    copyright            : GPL (C) 2012-2016 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/


#include <QtWidgets>
#include <QVariant>
#include <QString>

#include "dataset.h"
#include "colorbtn.h"
#include "fontbtn.h"

class DataWidget;

#define DW_DCL_STD_FUN \
 public: \
  virtual bool set() override; \
  virtual bool get() const override; \
  static DataWidget* create( HolderData &h, QWidget *parent ); \
 protected: \
  static int reg(); \
  static int registered;

#define DW_REG_FUN_STD( clname, can_edit ) \
  int clname::registered = clname::reg(); \
  int clname::reg() \
  { \
    static DataWidgetProp p { create, can_edit }; \
    return FactoryDataWidget::theFactory().registerWidgetType( #clname, p ); \
  } \
  DataWidget* clname::create( HolderData &h, QWidget *parent  ) \
  { \
    return new clname( h, parent ); \
  }

/** Abstract class for any widgets for data editing in DataDialog...
 * */
class DataWidget : public QFrame {
 public:
  DataWidget( HolderData &d, QWidget *parent = nullptr, bool hideLabel = false );
  virtual QSize        minimumSizeHint() const;
  virtual QSize        sizeHint() const;
  virtual bool set() = 0;
  virtual bool get() const = 0;
  QVariant::Type getTp() const { return ho.getTp(); }
  static DataWidget* create( HolderData & /*ho*/, QWidget * /*parent*/ )
    { return nullptr;}
 protected:
  HolderData &ho;
  QWidget *main_w;
  QLabel *lbl;
};

/** properties of DataWidget */
struct DataWidgetProp {
  DataWidget* (*creator)( HolderData &h, QWidget *parent );
  const char* eprop; // edit props
};

typedef QMap<QString,DataWidgetProp> DwPropMap;


class DummyDataWidget:  public DataWidget {
 public:
  DummyDataWidget( HolderData &h, QWidget *parent = nullptr, bool hideLabel = false );
  DW_DCL_STD_FUN;
 protected:
  QLabel *lbl_d;
};


class StringDataWidget:  public DataWidget {
 public:
  StringDataWidget( HolderData &h, QWidget *parent = nullptr, bool hideLabel = false );
  DW_DCL_STD_FUN;
 protected:
  QLineEdit *le;
};

class StringMLDataWidget: public DataWidget {
 public:
  StringMLDataWidget( HolderData &h, QWidget *parent = nullptr, bool hideLabel = false );
  DW_DCL_STD_FUN;
  virtual QSize  minimumSizeHint() const override;
  virtual QSize  sizeHint() const override;
 protected:
  QTextEdit *te;
};

class StringExtDataWidget: public DataWidget {
  Q_OBJECT
 public:
  StringExtDataWidget( HolderData &h, QWidget *parent = nullptr, bool hideLabel = false );
  DW_DCL_STD_FUN;
 protected Q_SLOTS:
   void edit();
 protected:
  QPushButton *pb;
  QString ts; // temporary string;
};



class IntDataWidget: public DataWidget {
 public:
  IntDataWidget( HolderData &h, QWidget *parent = nullptr, bool hideLabel = false );
  DW_DCL_STD_FUN;
 protected:
  QLineEdit *le;
};

class IntSpinDataWidget: public DataWidget {
 public:
  IntSpinDataWidget( HolderData &h, QWidget *parent = nullptr, bool hideLabel = false );
  DW_DCL_STD_FUN;
 protected:
  QSpinBox *sb;
};

class SwitchDataWidget: public DataWidget {
 public:
  SwitchDataWidget( HolderData &h, QWidget *parent = nullptr, bool hideLabel = false );
  DW_DCL_STD_FUN;
 protected:
  QCheckBox *cb;
};


class ListDataWidget: public DataWidget {
 public:
  ListDataWidget( HolderData &h, QWidget *parent = nullptr, bool hideLabel = false );
  DW_DCL_STD_FUN;
 protected:
  QComboBox *cb;
};

class DoubleDataWidget: public DataWidget {
 public:
  DoubleDataWidget( HolderData &h, QWidget *parent = nullptr, bool hideLabel = false );
  DW_DCL_STD_FUN;
 protected:
  QLineEdit *le;
};


class DoubleSpinDataWidget: public DataWidget {
 public:
  DoubleSpinDataWidget( HolderData &h, QWidget *parent = nullptr, bool hideLabel = false );
  DW_DCL_STD_FUN;
 protected:
  QDoubleSpinBox *sb;
};


class ColorDataWidget: public DataWidget {
 public:
  ColorDataWidget( HolderData &h, QWidget *parent = nullptr, bool hideLabel = false );
  DW_DCL_STD_FUN;
 protected:
  ColorBtn *cb;
};


class FontDataWidget: public DataWidget {
 public:
  FontDataWidget( HolderData &h, QWidget *parent = nullptr, bool hideLabel = false );
  DW_DCL_STD_FUN;
 protected:
  FontBtn *cb;
};


class DateDataWidget: public DataWidget {
 public:
  DateDataWidget( HolderData &h, QWidget *parent = nullptr, bool hideLabel = false );
  DW_DCL_STD_FUN;
 protected:
  QDateEdit *de;
};


class TimeDataWidget: public DataWidget {
 public:
  TimeDataWidget( HolderData &h, QWidget *parent = nullptr, bool hideLabel = false );
  DW_DCL_STD_FUN;
 protected:
  QTimeEdit *te;
};



class IntArrayDataWidget: public DataWidget {
 public:
  IntArrayDataWidget( HolderData &h, QWidget *parent = nullptr, bool hideLabel = false );
  DW_DCL_STD_FUN;
 protected:
  std::vector<QLineEdit*> les;
  QWidget *pwi;
};


class DoubleArrayDataWidget: public DataWidget {
 public:
  DoubleArrayDataWidget( HolderData &h, QWidget *parent = nullptr, bool hideLabel = false );
  DW_DCL_STD_FUN;
 protected:
  std::vector<QLineEdit*> les;
  QWidget *pwi;
};

class StringArrayDataWidget: public DataWidget {
 public:
  StringArrayDataWidget( HolderData &h, QWidget *parent = nullptr, bool hideLabel = false );
  DW_DCL_STD_FUN;
 protected:
  std::vector<QLineEdit*> les;
  QWidget *pwi;
};


class ObjDataWidget: public DataWidget {
  Q_OBJECT
 public:
  ObjDataWidget( HolderData &h, QWidget *parent = nullptr, bool hideLabel = false );
  void updateLabel();
  DW_DCL_STD_FUN;
 Q_SIGNALS:
  void editingFinished();
 protected Q_SLOTS:
  void edit();
 protected:
  QPushButton *pb;
};

class InputDataWidget: public DataWidget {
  Q_OBJECT
 public:
  InputDataWidget( HolderData &h, QWidget *parent = nullptr, bool hideLabel = false );
  DW_DCL_STD_FUN;
 protected Q_SLOTS:
  void lineToObj();
  void objToLine();
 protected:
  QLineEdit *le;
  ObjDataWidget *ow;
};


// --------------------------- Factory ----------------------


/** creator of DataWidget and database of its properties
 * */
class FactoryDataWidget {
  public:
   static FactoryDataWidget& theFactory();
   DataWidget* createDataWidget( HolderData &ho, QWidget *parent  ) const;
   QString findForHolder( const HolderData &ho, int *lev = 0 ) const;
   bool registerWidgetType( const QString &wname, const DataWidgetProp &prop );
   bool unregisterWidgetType( const QString &wname );

  private:
   FactoryDataWidget();
   FactoryDataWidget( const FactoryDataWidget& r ) = delete;
   FactoryDataWidget& operator=( const FactoryDataWidget& r ) = delete;

   DwPropMap propMap;
};

// ======================= DataDialog ===========================
//* simple class for dialog with DataWidgets

typedef QMap<QString,DataWidget*> DaWiMap;

class DataDialog : public QDialog {
  Q_OBJECT
  public:
   DataDialog( HolderData &a_ds, QWidget *parent = nullptr );
   int getAll(); // from object to wigets
   int setAll(); // from widgets to object
  public Q_SLOTS:
   virtual void accept();
   void showHelp();
   void checkData();
   void refreshData();
   void revertData();
   bool addObj();
   bool delObj();
   bool copyOne();
   bool copyAll();
   bool pasteOne();
   bool pasteAll();
  protected:
   virtual int createWidgets();
   virtual void createButtons();
   void showSimpleHelp();
   void addFinalSpace( QGridLayout *lay );
   HolderData &ds;
   DaWiMap dwm;
   QString saved_data;
   bool ro;
   bool buttons_created = false;
   QVBoxLayout *lay_main = nullptr;
   QTabWidget *tw = nullptr; // pane for all data widgets
};



