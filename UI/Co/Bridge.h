//
//  Bridge.h
//  co
//
//  Created by Ramkumar Ramachandra on 4/23/15.
//  Copyright (c) 2015 Ramkumar Ramachandra. All rights reserved.
//

#ifndef co_Bridge_h
#define co_Bridge_h

#include <objc/objc-runtime.h>

#ifdef __OBJC__
#define OBJC_CLASS(name) @class name
#else
#define OBJC_CLASS(name) typedef struct objc_object name
#endif

OBJC_CLASS(UIDriver);

namespace abc
{
    class UIDriverInterface
    {
        UIDriver* wrapped;
    public:
        UIDriverInterface();
        ~UIDriverInterface();
        void drive();
    };
}

#endif
