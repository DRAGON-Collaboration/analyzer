#ifndef DRAGON_GLOBALS_H
#define DRAGON_GLOBALS_H


#ifndef G__DICTIONARY
#define EXTERN extern
#else
#define EXTERN
#endif

/// Gamma events (global)
EXTERN dragon::Head gHead;
/// Heavy-ion events (global)
EXTERN dragon::Tail gTail;
/// Coinc Events (global)
EXTERN dragon::Coinc gCoinc;


#undef EXTERN

#endif
