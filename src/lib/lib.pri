TEMPLATE = subdirs

SUBDIRS += \
    $$PWD/od \
    $$PWD/canopen

canopen.depends = od
