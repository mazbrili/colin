#include <colinversion.h>

const char* ColinVersion() {
  return "__VERSION__"; // now set in includes.mk
  // major.minor.bugfixOrMiniFeatureCounter.tempOrTranslationCounter
}


const char* BuildDate() {
    return "__BUILD_DATE__";
}
