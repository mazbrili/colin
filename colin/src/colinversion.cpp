#include <colinversion.h>

const char* ColinVersion() {
  return "__VERSION__"; // now set in includes.mk
  // major.minor.bugfixOrMiniFeatureCounter.tempOrTranslationCounter
}
