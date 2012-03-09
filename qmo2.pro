TEMPLATE = subdirs 
SUBDIRS += qmo2 elems
CONFIG += release \
          warn_on \
          qt \
          thread 
DISTFILES += NEWS ChangeLog config.h

