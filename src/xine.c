// include header files 
#include "../include/xine.h"
#include "../include/helper_functions.h"
// include program files
#include "./lorenz_attractor.c"
#include "./lorenz_trig_attractor.c" 
#include "./dadras_attractor.c"
#include "./thomas_attractor.c"
// #include "array_opcodes.c" 

////////////////////////////////////////////////////////////////////////////////
/// opcode registration ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define S(x)    sizeof(x)
static OENTRY localops[] = {
  /* lorenz opcodes */
  {"lorenz2", S(LORENZ2), 0, 7, "aaa", "akkkkkkiii", (SUBR)lorenz2_init,
   (SUBR)lorenz2_process},
  {"lorenzh", S(LORENZ_H), 0, 7, "aaa", "akkkkkkiii", (SUBR)lorenz_h_init,
   (SUBR)lorenz_h_process},
  {"lorenzi", S(LORENZ_I), 0, 7, "aaa", "akkkkkkiii", (SUBR)lorenz_i_init,
   (SUBR)lorenz_i_process},
  /* thomas opcodes */
  {"thomas", S(THOMAS), 0, 7, "aaa", "akkkkiii",
   (SUBR)thomas_init, (SUBR)thomas_process},
  {"thomash", S(THOMAS_H), 0, 7, "aaa", "akkkkiii",
   (SUBR)thomas_h_init, (SUBR)thomas_h_process},
  {"thomasi", S(THOMAS_I), 0, 7, "aaa", "akkkkiii",
   (SUBR)thomas_i_init, (SUBR)thomas_i_process},
  /* dadras opcodes */
  {"dadras", S(DADRAS), 0, 7, "aaa", "akkkkkkkkiii",
   (SUBR)dadras_init, (SUBR)dadras_process},
  {"dadrash", S(DADRAS_H), 0, 7, "aaa", "akkkkkkkkiii",
   (SUBR)dadras_h_init, (SUBR)dadras_h_process},
  {"dadrasi", S(DADRAS_I), 0, 7, "aaa", "akkkkkkkkiii",
   (SUBR)dadras_i_init, (SUBR)dadras_i_process},
};

LINKAGE
