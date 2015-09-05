//-*- C++ -*-

#ifndef UIBRIDGE_H
#define UIBRIDGE_H

#include <objc/objc-runtime.h>

#ifdef __OBJC__
#define OBJC_CLASS(name) @class name
#else
#define OBJC_CLASS(name) typedef struct objc_object name
#endif

extern int ns_gui_init();

// OBJC_CLASS(UIDriver);

// namespace rhine
// {
// class UIDriverInterface
// {
//   UIDriver* wrapped;
// public:
//   UIDriverInterface();
//   ~UIDriverInterface();
//   void drive();
// };
// }

#endif
