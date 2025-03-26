#include "../include/xine.h"
#include "../include/helper_functions.h"
////////////////////////////////////////////////////////////////////////////////
/// dadras attractor opcodes ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// dadras opcode
// - outputs a value when cps cycle is done else outputs 0
typedef struct {
  OPDS h;
  MYFLT *aout_x, *aout_y, *aout_z; /* output */
  MYFLT *cpsp, *reset_trig, *a, *b, *c, *d, *e, *delta_time, *skip, *in_x, *in_y, *in_z; /* input */
  MYFLT x_value, y_value, z_value, x_in, y_in, z_in; /* internal variables */
  int32_t phs; /* oscillator */
} DADRAS;

int32_t dadras_init(CSOUND *csound, DADRAS *p){
  p->phs = 0;
  
  p->x_value = *p->in_x;
  p->y_value = *p->in_y;
  p->z_value = *p->in_z;

  p->x_in = p->x_value;
  p->y_in = p->y_value;
  p->z_in = p->z_value;
  return OK;
}

int32_t dadras_process(CSOUND *csound, DADRAS *p){
  /* sample accurate mechanism */
  uint32_t offset = p->h.insdshead->ksmps_offset;
  uint32_t early  = p->h.insdshead->ksmps_no_end;
  /* variables */
  MYFLT sr = csound->GetSr(csound);
  uint32_t sample_count = CS_KSMPS;
  uint32_t sample_index;
  MYFLT *out_x, *out_y, *out_z;
  MYFLT a, b, c, d, e, x, y, z, xx, yy, x_in, y_in, z_in, time;
  int32_t skip;
  /* oscillator stuff */
  int32_t phs = p->phs;
  MYFLT *cpsp;
  MYFLT freq;

  /* set output vectors*/
  out_x = p->aout_x;
  out_y = p->aout_y;
  out_z = p->aout_z;

  /* set time variant values */
  time = *p->delta_time;
  skip = (int32_t)*p->skip;
  a = *p->a;
  b = *p->b;
  c = *p->c;
  d = *p->d;
  e = *p->e;
  
  /* setting start values for x, y, z*/
  x = p->x_value;
  y = p->y_value;
  z = p->z_value;

  /* set input for writing to vectors */
  x_in = p->x_in;
  y_in = p->y_in;
  z_in = p->z_in;

  /* trigger mechanism */
  if (*p->reset_trig){
    p->phs = 0;
    phs = p->phs;
    
    p->x_value = *p->in_x;
    p->y_value = *p->in_y;
    p->z_value = *p->in_z;
    x = p->x_value;
    y = p->y_value;
    z = p->z_value;

    x_in = p->x_value;
    y_in = p->y_value;
    z_in = p->z_value;
  }
  
  /* oscillator init */
  phs = p->phs;
  cpsp = p->cpsp;
  freq = sr / *cpsp++;
  
  /* sample accurate mechanism*/
  if (UNLIKELY(offset)){
	memset(out_x, '\0', offset*sizeof(MYFLT));
	memset(out_y, '\0', offset*sizeof(MYFLT));
	memset(out_z, '\0', offset*sizeof(MYFLT));    
  }
  if (UNLIKELY(early)){
    sample_count -= early;
    memset(&out_x[sample_count], '\0', early*sizeof(MYFLT));
    memset(&out_y[sample_count], '\0', early*sizeof(MYFLT));
    memset(&out_z[sample_count], '\0', early*sizeof(MYFLT));
  }
  
  /* process loop */
  for (sample_index = offset; sample_index <
		 sample_count;sample_index++){

    /* write values to vectors */
    out_x[sample_index] = x_in;
    out_y[sample_index] = y_in;
    out_z[sample_index] = z_in;
    // printf("x: %f y: %f x: %f\n", x_in, y_in, z_in);
    
    /* get new values when phase reaches freq =
       sr/cps */
    if (phs >= freq){
      /* get values loop */
      do {
		/* fuctions for the dadras attractor

		 */
		xx = x + time * (y - (a * x) + (b * y * z));
		yy = y + time * ((c * y) - (x * y) + z);
		z =  z + time * ((d * x * y) - (e * z));
		x = xx;
		y = yy;
      } while(--skip > 0);
      
      /* when phase is crossing 0 the output values are	taken from
		 dadras attractor functions */      
      x_in = x;
      y_in = y;
      z_in = z;

      /* start new phase cycle */ 
      phs = 0;
    } else {

      /* while phase is in cycle boundaries the output values are set
		 to 0 */      
      x_in = 0;
      y_in = 0;
      z_in = 0;
    }

    /* increment phase */
    phs++; 
  }
  /* save values for next process loop */
  p->phs = phs;
  
  p->x_value = x;
  p->y_value = y;
  p->z_value = z;

  p->x_in = x_in;
  p->y_in = y_in;
  p->z_in = z_in;
  
  return OK;
}

// dadrash opcode
// - holds the ouput value till next value is generated
typedef struct {
  OPDS h;
  MYFLT *aout_x, *aout_y, *aout_z; /* output */
  MYFLT *cpsp, *reset_trig, *a, *b, *c, *d, *e, *delta_time, *skip, *in_x, *in_y, *in_z; /* input */
  MYFLT x_value, y_value, z_value, x_in, y_in, z_in; /* internal variables */
  int32_t phs; /* oscillator */
} DADRAS_H;

int32_t dadras_h_init(CSOUND *csound, DADRAS_H *p){
  p->phs = 0;
  
  p->x_value = *p->in_x;
  p->y_value = *p->in_y;
  p->z_value = *p->in_z;
  return OK;
}

int32_t dadras_h_process(CSOUND *csound, DADRAS_H *p){
  /* sample accurate mechanism */
  uint32_t offset = p->h.insdshead->ksmps_offset;
  uint32_t early  = p->h.insdshead->ksmps_no_end;
  /* variables */
  MYFLT sr = csound->GetSr(csound);
  uint32_t sample_count = CS_KSMPS;
  uint32_t sample_index;
  MYFLT *out_x, *out_y, *out_z;
  MYFLT a, b, c, d, e, x, y, z, xx, yy, x_in, y_in, z_in, time;
  int32_t skip;
  /* oscillator stuff */
  int32_t phs;
  MYFLT *cpsp;
  MYFLT freq;

  /* set output vectors*/
  out_x = p->aout_x;
  out_y = p->aout_y;
  out_z = p->aout_z;

  /* set time variant values */
  time = *p->delta_time;
  skip = (int32_t)*p->skip;
  a = *p->a;
  b = *p->b;
  c = *p->c;
  d = *p->d;
  e = *p->e;

  /* setting start values for x, y, z*/
  x = p->x_value;
  y = p->y_value;
  z = p->z_value;

  /* trigger mechanism */
  if (*p->reset_trig){
    p->phs = 0;
    phs = p->phs;
    
    p->x_value = *p->in_x;
    p->y_value = *p->in_y;
    p->z_value = *p->in_z;

    x = p->x_value;
    y = p->y_value;
    z = p->z_value;
  }
    
  /* oscillator init */
  phs = p->phs;
  cpsp = p->cpsp;
  freq = sr / *cpsp++;
    
  /* sample accurate mechanism*/
  if (UNLIKELY(offset)){
	memset(out_x, '\0', offset * sizeof(MYFLT));
	memset(out_y, '\0', offset * sizeof(MYFLT));
	memset(out_z, '\0', offset * sizeof(MYFLT));    
  }
  if (UNLIKELY(early)){
    sample_count -= early;
    memset(&out_x[sample_count], '\0', early * sizeof(MYFLT));
    memset(&out_y[sample_count], '\0', early * sizeof(MYFLT));
    memset(&out_z[sample_count], '\0', early * sizeof(MYFLT));
  }
  
  /* process loop */
  for (sample_index = offset; sample_index < sample_count;
       sample_index++){

    /* get new values when phase reaches freq = sr/cps */
    if (phs >= freq){

      /* get values loop */
      do {
		/* fuctions for the dadras attractor

		 */
		xx = x + time * (y - (a * x) + (b * y * z));
		yy = y + time * ((c * y) - (x * y) + z);
		z =  z + time * ((d * x * y) - (e * z));
		x = xx;
		y = yy;
      } while(--skip > 0);

      /* start new phase cycle */
      phs = 0;      
    }
    
    /* write values to vectors */
    out_x[sample_index] = x;
    out_y[sample_index] = y;
    out_z[sample_index] = z;
    //printf("x: %f y: %f z: %f\n", x, y, z);
    
    /* increment phase */
    phs++; 
  }

  /* save values for next processing loop */
  p->phs = phs;
  
  p->x_value = x;
  p->y_value = y;
  p->z_value = z;

  return OK;
}

// dadrasi opcode
// - linear interpolation between values
typedef struct {
  OPDS h;
  MYFLT *aout_x, *aout_y, *aout_z; /* output */
  MYFLT *cpsp, *reset_trig, *a, *b, *c, *d, *e, *delta_time, *skip, *in_x, *in_y, *in_z; /* input */
  MYFLT x_value, y_value, z_value, x_in, y_in, z_in, x0, y0, z0; /* internal variables */
  int32_t phs; /* oscillator */
} DADRAS_I;

int32_t dadras_i_init(CSOUND *csound, DADRAS_I *p){
  p->phs = 0;
  
  p->x_value = *p->in_x;
  p->y_value = *p->in_y;
  p->z_value = *p->in_z;
  
  p->x0 = *p->in_x;
  p->y0 = *p->in_y;
  p->z0 = *p->in_z;
  return OK;
}

int32_t dadras_i_process(CSOUND *csound, DADRAS_I *p){
  /* sample accurate mechanism */
  uint32_t offset = p->h.insdshead->ksmps_offset;
  uint32_t early  = p->h.insdshead->ksmps_no_end;
  /* variables */
  MYFLT sr = csound->GetSr(csound);
  uint32_t sample_count = CS_KSMPS;
  uint32_t sample_index;
  MYFLT *out_x, *out_y, *out_z;
  MYFLT a, b, c, d, e, x, y, z, xx, yy, time;
  int32_t skip;
  /* oscillator stuff */
  int32_t phs;
  MYFLT *cpsp;
  MYFLT freq;
  MYFLT last_x, last_y, last_z; /* for interpolation between values */

  /* set output vectors */
  out_x = p->aout_x;
  out_y = p->aout_y;
  out_z = p->aout_z;

  /* set time variant values */
  time = *p->delta_time;
  skip = (int32_t)*p->skip;
  a = *p->a;
  b = *p->b;
  c = *p->c;
  d = *p->d;
  e = *p->e;


  /* setting start values for x, y, z*/
  x = p->x_value;
  y = p->y_value;
  z = p->z_value;
  last_x = p->x0;
  last_y = p->y0;
  last_z = p->z0;
  
  /* reset trigger mechanism */
  if (*p->reset_trig){
    p->phs = 0;
    phs = p->phs;
    
    p->x_value = *p->in_x;
    p->y_value = *p->in_y;
    p->z_value = *p->in_z;
    x = p->x_value;
    y = p->y_value;
    z = p->z_value;

    p->x0 = *p->in_x;
    p->y0 = *p->in_y;
    p->z0 = *p->in_z;
    last_x = p->x0;
    last_y = p->y0;
    last_z = p->z0;  
  }
    
  /* oscillator init */
  phs = p->phs;
  cpsp = p->cpsp;
  freq = sr / *cpsp++;
    
  /* csound sample accurate mechanism*/
  if (UNLIKELY(offset)){
	memset(out_x, '\0', offset * sizeof(MYFLT));
	memset(out_y, '\0', offset * sizeof(MYFLT));
	memset(out_z, '\0', offset * sizeof(MYFLT));    
  }
  if (UNLIKELY(early)){
    sample_count -= early;
    memset(&out_x[sample_count], '\0', early * sizeof(MYFLT));
    memset(&out_y[sample_count], '\0', early * sizeof(MYFLT));
    memset(&out_z[sample_count], '\0', early * sizeof(MYFLT));
  }

  /* process loop */
  for (sample_index = offset; sample_index < sample_count;
       sample_index++){    
    /* get new values when phase reaches freq =
       sr/cps or in first processing cycle when phs is 0 */
    if ((0 == phs) || (phs >= freq)){
      /* only update values when it's not the first processing loop or
		 not retriggerd */ 
      if (phs >= freq){
		/* keep values updated for interpolation */
		last_x = x;
		last_y = y;
		last_z = z;      
      }

      /* get values loop */
      do {
		/* fuctions for the dadras attractor

		 */
		xx = x + time * (y - (a * x) + (b * y * z));
		yy = y + time * ((c * y) - (x * y) + z);
		z =  z + time * ((d * x * y) - (e * z));
		x = xx;
		y = yy;
      } while(--skip > 0);

      /* start new phase cycle */
      phs = 0;      
    }
    
    /* write values to vectors */
    out_x[sample_index] = linear_interpolation(0, last_x, freq, x, (MYFLT)phs);
    out_y[sample_index] = linear_interpolation(0, last_y, freq, y, (MYFLT)phs);
    out_z[sample_index] = linear_interpolation(0, last_z, freq, z, (MYFLT)phs);
    //printf("x: %f y: %f z: %f\n", x, y, z);
    
    /* increment phase */
    phs++; 
  }

  /* save values for next processing loop */
  p->phs = phs;
  
  p->x_value = x;
  p->y_value = y;
  p->z_value = z;

  p->x0 = last_x;
  p->y0 = last_y;
  p->z0 = last_z;
  
  return OK;
}
