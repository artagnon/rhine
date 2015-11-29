//-*- C++ -*-
#ifndef RHINE_GC_H
#define RHINE_GC_H

namespace rhine {
void linkRhineGC();

struct ForceGCLinking {
  ForceGCLinking() {
    linkRhineGC();
  }
} ForceGCLinking;
}

#endif
