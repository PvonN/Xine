#include "csdl.h"
#include "csoundCore.h"
#include <math.h>

static inline MYFLT _GetLocalSr(CSOUND *csound, OPDS *ctx) {
#if CS_API_VERSION_MAJOR == 7
    IGN(csound);
    return GetLocalSr(ctx);
#else
    IGN(ctx);
    return csound->GetSr(csound);
#endif
}

MYFLT signed_deviation(MYFLT max_deviation, MYFLT max_signals, int32_t deviation_index)
{
  MYFLT deviation;
  deviation = ((deviation_index / max_signals) * max_deviation) / max_deviation;
  
  if (deviation_index == 0)
    return 0.0;
  if ((deviation_index % 2) == 0)
    return -deviation;
  if ((deviation_index % 2) != 0)
    return deviation;

  return 0;
}

MYFLT linear_interpolation(MYFLT x0, MYFLT y0, MYFLT x1, MYFLT y1, MYFLT x_step){
  MYFLT result = y0 + ((y1 - y0) / (x1 - x0)) * (x_step - x0);
  return result; 
}
