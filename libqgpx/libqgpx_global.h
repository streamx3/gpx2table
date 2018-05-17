#ifndef LIBQGPX_GLOBAL_H
#define LIBQGPX_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIBQGPX_LIBRARY)
#  define LIBQGPXSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LIBQGPXSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBQGPX_GLOBAL_H
