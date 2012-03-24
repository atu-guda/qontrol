TEMPLATE = app
TARGET = qmo2x
CONFIG += release warn_on qt thread 
#CONFIG += debug warn_on qt thread 

RESOURCES = qmo2x.qrc

INCLUDEPATH += .. ../elems 

LIBS += -L ../elems -lelems -lgsl -lgslcblas -lm

SOURCES += \
  datacont.cpp \
  dataset.cpp \
  main.cpp \
  miscfun.cpp \
  mo2settdlg.cpp \
  qanydial.cpp \
  qcolorbtn.cpp \
  qdoubletable.cpp \
  qgraphview.cpp \
  qmo2doc.cpp \
  qmo2view.cpp \
  qmo2win.cpp \
  qoutview.cpp \
  qplotview.cpp \
  qrunview.cpp \
  qstatusmodel.cpp \
  qstructview.cpp \
  rootdata.cpp \
  tmiso.cpp \
  tmodel.cpp
HEADERS += \
  datacont.h \
  dataset.h \
  defs.h \
  miscfun.h \
  mo2settdlg.h \
  myelems1.h \
  qanydial.h \
  qcolorbtn.h \
  qdoubletable.h \
  qgraphview.h \
  qmo2doc.h \
  qmo2view.h \
  qmo2win.h \
  qoutview.h \
  qplotview.h \
  qrunview.h \
  qstatusmodel.h \
  qstructview.h \
  rootdata.h \
  tmiso.h \
  tmodel.h 


