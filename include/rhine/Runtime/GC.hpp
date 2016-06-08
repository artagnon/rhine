#pragma once

namespace rhine {
void linkRhineGC();

struct ForceGCLinking {
  ForceGCLinking() { linkRhineGC(); }
} ForceGCLinking;
}
