#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(HSRCORE_LIB)
#  define HSRCORE_EXPORT Q_DECL_EXPORT
# else
#  define HSRCORE_EXPORT Q_DECL_IMPORT
# endif
#else
# define HSRCORE_EXPORT
#endif
