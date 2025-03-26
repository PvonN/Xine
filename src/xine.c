#include "../include/xine.h"
#include "../include/helper_functions.h"

#include "./dadras_attractor.c"
// #include "array_opcodes.c" 
////////////////////////////////////////////////////////////////////////////////
/// lorenz attractor opcodes ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// lorenz2 opcode
// - outputs a value when cps cycle is done else outputs 0
typedef struct {
  OPDS h;
  MYFLT *aout_x, *aout_y, *aout_z; /* output */
  MYFLT *cpsp, *reset_trig, *sigma, *rho, *beta, *delta_time, *skip, *in_x, *in_y, *in_z; /* input */
  MYFLT x_value, y_value, z_value, x_in, y_in, z_in; /* internal variables */
  int32_t phs; /* oscillator */
} LORENZ2;

int32_t lorenz2_init(CSOUND *csound, LORENZ2 *p){
  p->phs = 0;
  
  p->x_value = *p->in_x;
  p->y_value = *p->in_y;
  p->z_value = *p->in_z;

  p->x_in = p->x_value;
  p->y_in = p->y_value;
  p->z_in = p->z_value;
  return OK;
}

int32_t lorenz2_process(CSOUND *csound, LORENZ2 *p){
  /* sample accurate mechanism */
  uint32_t offset = p->h.insdshead->ksmps_offset;
  uint32_t early  = p->h.insdshead->ksmps_no_end;
  /* variables */
  MYFLT sr = csound->GetSr(csound);
  uint32_t sample_count = CS_KSMPS;
  uint32_t sample_index;
  MYFLT *out_x, *out_y, *out_z;
  MYFLT sigma, rho, beta, x, y, z, xx, yy, x_in, y_in, z_in, time;
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
  sigma = *p->sigma;
  rho = *p->rho;
  beta = *p->beta;
  
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
	/* functions for the lorenz attractor
	   https://en.wikipedia.org/wiki/Lorenz_system
	 */
	xx = x + time * (sigma * (y - x));
	yy = y + time * (x * (rho - z) - y);
	z  = z + time * ((x * y) - (beta * z));
	x  = xx;
	y  = yy;
      } while(--skip > 0);
      
      /* when phase is crossing 0 the output values are	taken from
	 lorenz attractor functions */      
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


// lorenzh opcode
// - holds the ouput value till next value is generated
typedef struct {
  OPDS h;
  MYFLT *aout_x, *aout_y, *aout_z; /* output */
  MYFLT *cpsp, *reset_trig, *sigma, *rho, *beta, *delta_time, *skip, *in_x, *in_y, *in_z; /* input */
  MYFLT x_value, y_value, z_value; /* internal variables */
  int32_t phs; /* oscillator */
} LORENZ_H;

int32_t lorenz_h_init(CSOUND *csound, LORENZ_H *p){
  p->phs = 0;
  
  p->x_value = *p->in_x;
  p->y_value = *p->in_y;
  p->z_value = *p->in_z;
  return OK;
}

int32_t lorenz_h_process(CSOUND *csound, LORENZ_H *p){
  /* sample accurate mechanism */
  uint32_t offset = p->h.insdshead->ksmps_offset;
  uint32_t early  = p->h.insdshead->ksmps_no_end;
  /* variables */
  MYFLT sr = csound->GetSr(csound);
  uint32_t sample_count = CS_KSMPS;
  uint32_t sample_index;
  MYFLT *out_x, *out_y, *out_z;
  MYFLT sigma, rho, beta, x, y, z, xx, yy, x_in, y_in, z_in, time;
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
  sigma = *p->sigma;
  rho = *p->rho;
  beta = *p->beta;

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
	/* functions for the lorenz attractor
	   https://en.wikipedia.org/wiki/Lorenz_system
	*/
	xx = x + time * (sigma * (y - x));
	yy = y + time * (x * (rho - z) - y);
	z  = z + time * ((x * y) - (beta * z));
	x  = xx;
	y  = yy;
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


// lorenzi opcode
// - linear interpolation between values
typedef struct {
  OPDS h;
  MYFLT *aout_x, *aout_y, *aout_z; /* output */
  MYFLT *cpsp, *reset_trig, *sigma, *rho, *beta, *delta_time, *skip, *in_x, *in_y, *in_z; /* input */
  MYFLT x_value, y_value, z_value, x0, y0, z0; /* internal variables */
  int32_t phs; /* oscillator */
} LORENZ_I;

int32_t lorenz_i_init(CSOUND *csound, LORENZ_I *p){
  p->phs = 0;
  
  p->x_value = *p->in_x;
  p->y_value = *p->in_y;
  p->z_value = *p->in_z;
  
  p->x0 = *p->in_x;
  p->y0 = *p->in_y;
  p->z0 = *p->in_z;
  return OK;
}

int32_t lorenz_i_process(CSOUND *csound, LORENZ_I *p){
  /* sample accurate mechanism */
  uint32_t offset = p->h.insdshead->ksmps_offset;
  uint32_t early  = p->h.insdshead->ksmps_no_end;
  /* variables */
  MYFLT sr = csound->GetSr(csound);
  uint32_t sample_count = CS_KSMPS;
  uint32_t sample_index;
  MYFLT *out_x, *out_y, *out_z;
  MYFLT sigma, rho, beta, x, y, z, xx, yy, time;
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
  sigma = *p->sigma;
  rho = *p->rho;
  beta = *p->beta;

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
	/* functions for the lorenz attractor
	   https://en.wikipedia.org/wiki/Lorenz_system
	*/
	xx = x + time * (sigma * (y - x));
	yy = y + time * (x * (rho - z) - y);
	z  = z + time * ((x * y) - (beta * z));
	x  = xx;
	y  = yy;
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

/// lorenz attractor
typedef struct {
  OPDS h;
  ARRAYDAT *aout_x, *aout_y, *aout_z; /* output */
  MYFLT *sigma, *rho, *beta, *delta_time, *skip, *in_x, *in_y, *in_z, *n_particles, *max_deviation; /* input */
  MYFLT n_voices; /* internal variables */
  MYFLT skip_count;
  MYFLT x_start;
  MYFLT y_start;
  MYFLT z_start;
  MYFLT max_dev;
} LORENZ_PARTICLE;


int32_t lorenz_particle_init(CSOUND *csound, LORENZ_PARTICLE *p){
  if (UNLIKELY(*p->n_particles == 0))
    return
      csound->InitError(csound, "%s",
                        Str("Error: Number of particles must be > 0"));

  p->n_voices = *p->n_particles;
  tabinit(csound, p->aout_x, p->n_voices);
  tabinit(csound, p->aout_y, p->n_voices);
  tabinit(csound, p->aout_z, p->n_voices);
  p->skip_count = *p->skip;
  p->x_start = *p->in_x;
  p->y_start = *p->in_y;
  p->z_start = *p->in_z;
  p->max_dev = *p->max_deviation;
  return OK;
}

int32_t lorenz_particle_process(CSOUND *csound, LORENZ_PARTICLE *p){
  /* sample accurate mechanism */
  uint32_t offset = p->h.insdshead->ksmps_offset;
  uint32_t early  = p->h.insdshead->ksmps_no_end;

  /* variables */
  uint32_t sample_count = CS_KSMPS;
  uint32_t sample_index, voice_index;
  uint32_t n_voices = p->n_voices;
  uint32_t voice;
  MYFLT max_deviation = p->max_dev;
  MYFLT deviation;
  MYFLT x, y, z, xx, yy, x_with_dev, y_with_dev, z_with_dev;
  MYFLT sigma = *p->sigma;
  MYFLT rho = *p->rho;
  MYFLT beta = *p->beta;
  MYFLT time = *p->delta_time;
  uint32_t skip = (uint32_t)p->skip_count;
  ARRAYDAT *out_x = p->aout_x;
  ARRAYDAT *out_y = p->aout_y;
  ARRAYDAT *out_z = p->aout_z;
  MYFLT *x_out, *y_out, *z_out;
  
  /* Process channels: */
  if (UNLIKELY(early))
    sample_count -= early;
  for (voice_index = 0; voice_index < n_voices; voice_index++){
    x_out = (MYFLT*)&out_x->data[voice_index];
    y_out = (MYFLT*)&out_y->data[voice_index];
    z_out = (MYFLT*)&out_z->data[voice_index];
    deviation = signed_deviation(max_deviation, n_voices, voice_index);
    if (UNLIKELY(offset)){
      memset(x_out, '\0', offset * sizeof(MYFLT));
      memset(y_out, '\0', offset * sizeof(MYFLT));
      memset(z_out, '\0', offset*sizeof(MYFLT));
      }
    if (UNLIKELY(early)) {
      memset(&x_out[sample_count], '\0', early*sizeof(MYFLT));
      memset(&y_out[sample_count], '\0', early*sizeof(MYFLT));
      memset(&z_out[sample_count], '\0', early*sizeof(MYFLT));
    }
    for (sample_index = offset; sample_index < sample_count;
	 sample_index++) {
      for (int j = 0; j < skip; j++){
        /* fuctions for the lorenz attractor
           https://en.wikipedia.org/wiki/Lorenz_system
         */
        x_with_dev = x + deviation;
        y_with_dev = y + deviation;
        z_with_dev = z + deviation;
	xx = (sigma * (y_with_dev - x_with_dev))       * time;
	yy = (x_with_dev     * (rho - z_with_dev) - y_with_dev) * time;
	z  += ((x_with_dev * y_with_dev) - (beta * z_with_dev))  * time;
	x  += xx;
	y  += yy;
      }
      x_out[sample_index] = x;
      y_out[sample_index] = y;
      z_out[sample_index] = z;
    }
  }

  return OK;
}
////////////////////////////////////////////////////////////////////////////////
/// thomas attractor opcodes ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// thomas opcode
// - outputs a value when cps cycle is done else outputs 0
typedef struct {
  OPDS h;
  MYFLT *aout_x, *aout_y, *aout_z; /* output */
  MYFLT *cpsp, *reset_trig, *b, *delta_time, *skip, *in_x, *in_y, *in_z; /* input */
  MYFLT x_value, y_value, z_value, x_in, y_in, z_in; /* internal variables */
  int32_t phs; /* oscillator */
} THOMAS;

int32_t thomas_init(CSOUND *csound, THOMAS *p){
  p->phs = 0;
  
  p->x_value = *p->in_x;
  p->y_value = *p->in_y;
  p->z_value = *p->in_z;

  p->x_in = p->x_value;
  p->y_in = p->y_value;
  p->z_in = p->z_value;
  return OK;
}

int32_t thomas_process(CSOUND *csound, THOMAS *p){
  /* sample accurate mechanism */
  uint32_t offset = p->h.insdshead->ksmps_offset;
  uint32_t early  = p->h.insdshead->ksmps_no_end;
  /* variables */
  MYFLT sr = csound->GetSr(csound);
  uint32_t sample_count = CS_KSMPS;
  uint32_t sample_index;
  MYFLT *out_x, *out_y, *out_z;
  MYFLT b, x, y, z, xx, yy, x_in, y_in, z_in, time;
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
  b = *p->b;
  
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
	/* fuctions for the thomas attractor
	   https://en.wikipedia.org/wiki/Thomas'_cyclically_symmetric_attractor
	*/
	xx = x + time * (-b * x + sin(y));
	yy = y + time * (-b * y + sin(z));
	z =  z + time * (-b * z + sin(x));
	x = xx;
	y = yy;
      } while(--skip > 0);
      
      /* when phase is crossing 0 the output values are	taken from
	 thomas attractor functions */      
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

// thomash opcode
// - holds the ouput value till next value is generated
typedef struct {
  OPDS h;
  MYFLT *aout_x, *aout_y, *aout_z; /* output */
  MYFLT *cpsp, *reset_trig, *b, *delta_time, *skip, *in_x, *in_y, *in_z; /* input */
  MYFLT x_value, y_value, z_value; /* internal variables */
  int32_t phs; /* oscillator */
} THOMAS_H;

int32_t thomas_h_init(CSOUND *csound, THOMAS_H *p){
  p->phs = 0;
  
  p->x_value = *p->in_x;
  p->y_value = *p->in_y;
  p->z_value = *p->in_z;
  return OK;
}

int32_t thomas_h_process(CSOUND *csound, THOMAS_H *p){
  /* sample accurate mechanism */
  uint32_t offset = p->h.insdshead->ksmps_offset;
  uint32_t early  = p->h.insdshead->ksmps_no_end;
  /* variables */
  MYFLT sr = csound->GetSr(csound);
  uint32_t sample_count = CS_KSMPS;
  uint32_t sample_index;
  MYFLT *out_x, *out_y, *out_z;
  MYFLT b, x, y, z, xx, yy, x_in, y_in, z_in, time;
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
  b = *p->b;

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
	/* fuctions for the thomas attractor
	   https://en.wikipedia.org/wiki/Thomas'_cyclically_symmetric_attractor
	*/
	xx = x + time * (-b * x + sin(y));
	yy = y + time * (-b * y + sin(z));
	z =  z + time * (-b * z + sin(x));
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

// thomasi opcode
// - linear interpolation between values
typedef struct {
  OPDS h;
  MYFLT *aout_x, *aout_y, *aout_z; /* output */
  MYFLT *cpsp, *reset_trig, *b, *delta_time, *skip, *in_x, *in_y, *in_z; /* input */
  MYFLT x_value, y_value, z_value, x0, y0, z0; /* internal variables */
  int32_t phs; /* oscillator */
} THOMAS_I;

int32_t thomas_i_init(CSOUND *csound, THOMAS_I *p){
  p->phs = 0;
  
  p->x_value = *p->in_x;
  p->y_value = *p->in_y;
  p->z_value = *p->in_z;
  
  p->x0 = *p->in_x;
  p->y0 = *p->in_y;
  p->z0 = *p->in_z;
  return OK;
}

int32_t thomas_i_process(CSOUND *csound, THOMAS_I *p){
  /* sample accurate mechanism */
  uint32_t offset = p->h.insdshead->ksmps_offset;
  uint32_t early  = p->h.insdshead->ksmps_no_end;
  /* variables */
  MYFLT sr = csound->GetSr(csound);
  uint32_t sample_count = CS_KSMPS;
  uint32_t sample_index;
  MYFLT *out_x, *out_y, *out_z;
  MYFLT b, x, y, z, xx, yy, time;
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
  b = *p->b;

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
	/* fuctions for the thomas attractor
	   https://en.wikipedia.org/wiki/Thomas'_cyclically_symmetric_attractor
	*/
	xx = x + time * (-b * x + sin(y));
	yy = y + time * (-b * y + sin(z));
	z =  z + time * (-b * z + sin(x));
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





////////////////////////////////////////////////////////////////////////////////
/// opcode registration ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define S(x)    sizeof(x)
static OENTRY localops[] = {
  /* lorenz opcodes */
  {"lorenz2", S(LORENZ2), 0, 7, "aaa", "akkkkkkiii",
   (SUBR)lorenz2_init, (SUBR)lorenz2_process},
  {"lorenzh", S(LORENZ_H), 0, 7, "aaa", "akkkkkkiii",
   (SUBR)lorenz_h_init, (SUBR)lorenz_h_process},
  {"lorenzi", S(LORENZ_I), 0, 7, "aaa", "akkkkkkiii",
   (SUBR)lorenz_i_init, (SUBR)lorenz_i_process},
  /* thomas opcodes */
  {"thomas", S(THOMAS), 0, 7, "aaa", "akkkkiii",
   (SUBR)thomas_init, (SUBR)thomas_process},
  /* {"thomas", S(THOMAS), 0, 7, "a[]a[]a[]", "akkkkiiiii", */
  /*  (SUBR)thomas_particle_init, (SUBR)thomas_particle_process}, */
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
