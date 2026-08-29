// Registers Qt's platform plugin when Qt is built statically
// (RUNTIME_LINK=static, see conanfile.py). A statically built Qt has no
// platforms/*.so to dlopen() at runtime, so the plugin must instead be
// linked into the binary and registered explicitly via Q_IMPORT_PLUGIN
// (see the matching target_link_libraries in CMakeLists.txt). This file
// only needs to be compiled into the executable -- nothing calls it
// directly, Q_IMPORT_PLUGIN registers itself through a static initializer.

#if defined(QT_STATIC_PLATFORM_PLUGIN)
#include <QtPlugin>
#if defined(Q_OS_WIN)
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#elif defined(Q_OS_LINUX)
Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)
#endif
#endif
