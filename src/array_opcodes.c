#include <math.h>
#include <stdint.h>
#include "csdl.h"
#include "csoundCore.h"

#include "arrays.h"

#include "sysdep.h"
#include "xine.h"



//// thomas particle attractor
typedef struct {
  OPDS h;
  ARRAYDAT *aout_x, *aout_y, *aout_z; /* output */
  MYFLT *cpsp, *reset_trig, *b, *delta_time, *skip, *in_x, *in_y, *in_z, *n_particles, *max_deviation; /* input */
  MYFLT n_voices, max_dev; /* internal variables */
  AUXCH x_vals, y_vals, z_vals, x_in, y_in, z_in;
  int32_t phs;
} THOMAS_PARTICLE;


int32_t thomas_particle_init(CSOUND *csound, THOMAS_PARTICLE *p){
  /* safety check on particle number */
  if (UNLIKELY(*p->n_particles == 0))
    return
      csound->InitError(csound, "%s",
                        Str("Error: Number of particles must be > 0"));
  /* init output arrays */
  p->n_voices = *p->n_particles;
  tabinit(csound, p->aout_x, p->n_voices);
  tabinit(csound, p->aout_y, p->n_voices);
  tabinit(csound, p->aout_z, p->n_voices);
  /* init value arrays */
  csound->AuxAlloc(csound, sizeof(MYFLT) * p->n_voices, &(p->x_in));
  memset(p->x_in.auxp, 0, sizeof(MYFLT)*p->n_voices);
  csound->AuxAlloc(csound, sizeof(MYFLT) * p->n_voices, &(p->y_in));
  memset(p->y_in.auxp, 0, sizeof(MYFLT)*p->n_voices);
  csound->AuxAlloc(csound, sizeof(MYFLT) * p->n_voices, &(p->z_in));
  memset(p->z_in.auxp, 0, sizeof(MYFLT)*p->n_voices);
  csound->AuxAlloc(csound, sizeof(MYFLT) * p->n_voices, &(p->x_vals));
  memset(p->x_vals.auxp, 0, sizeof(MYFLT)*p->n_voices);
  csound->AuxAlloc(csound, sizeof(MYFLT) * p->n_voices, &(p->y_vals));
  memset(p->y_vals.auxp, 0, sizeof(MYFLT)*p->n_voices);
  csound->AuxAlloc(csound, sizeof(MYFLT) * p->n_voices, &(p->z_vals));
  memset(p->z_vals.auxp, 0, sizeof(MYFLT)*p->n_voices);
  /* init start values */
  MYFLT *x_vals, *y_vals, *z_vals;
  x_vals = p->x_vals.auxp;
  y_vals = p->y_vals.auxp;
  z_vals = p->z_vals.auxp;
  
  for (int32_t i = 0; i < p->n_voices; i++){
    x_vals[i] = *p->in_x + signed_deviation(*p->max_deviation, p->n_voices, i);
    y_vals[i] = *p->in_y + signed_deviation(*p->max_deviation, p->n_voices, i);
    z_vals[i] = *p->in_z + signed_deviation(*p->max_deviation, p->n_voices, i);
  }

  return OK;
}

int32_t thomas_particle_process(CSOUND *csound, THOMAS_PARTICLE *p){
  /* sample accurate mechanism */
  uint32_t offset = p->h.insdshead->ksmps_offset;
  uint32_t early  = p->h.insdshead->ksmps_no_end;
  /* variables */
  MYFLT sr = csound->GetSr(csound);
  uint32_t sample_count = CS_KSMPS;
  uint32_t sample_index, voice_index, n_voices;
  MYFLT max_deviation;
  MYFLT b, x, y, z, xx, yy, x_with_dev, y_with_dev, z_with_dev, time;
  MYFLT *x_vals, *y_vals, *z_vals, *x_in, *y_in, *z_in;
  int32_t skip;
  /* oscillator stuff */
  int32_t phs = p->phs;
  MYFLT *cpsp;
  MYFLT freq;

  /* set output arrays */
  MYFLT *out_x, *out_y, *out_z;
  out_x = (MYFLT*)p->aout_x->data;
  out_y = (MYFLT*)p->aout_y->data;
  out_z = (MYFLT*)p->aout_z->data;
  n_voices = p->n_voices;
  
  /* set time variant values */
  time = *p->delta_time;
  skip = (int32_t)*p->skip;
  b = *p->b;

  /* setting start values for x, y, z*/
  max_deviation = p->max_dev;
  x_vals = (MYFLT *)p->x_vals.auxp;
  y_vals = (MYFLT *)p->y_vals.auxp;
  z_vals = (MYFLT *)p->z_vals.auxp;

  /* set input for writing to vectors */
  x_in = p->x_in.auxp;
  y_in = p->y_in.auxp;
  z_in = p->z_in.auxp;

  /* trigger mechanism */
  if (*p->reset_trig){
    p->phs = 0;
    phs = p->phs;

    for (voice_index = 0; voice_index < n_voices; voice_index++){
      x_vals[voice_index] = *p->in_x + signed_deviation(*p->max_deviation, p->n_voices, voice_index);
      y_vals[voice_index] = *p->in_y + signed_deviation(*p->max_deviation, p->n_voices, voice_index);
      z_vals[voice_index] = *p->in_z + signed_deviation(*p->max_deviation, p->n_voices, voice_index);
    }

    x_in = p->x_in.auxp;
    y_in = p->y_in.auxp;
    z_in = p->z_in.auxp;
  }
  
  /* oscillator init */
  phs = p->phs;
  cpsp = p->cpsp;
  freq = sr / *cpsp++;

  /* sample accurate machanism */
  if (UNLIKELY(offset)){
    for (voice_index = 0; voice_index < n_voices; voice_index++ ) {
      for (sample_index = 0; sample_index < sample_count;
	   sample_index++) {
	out_x[voice_index*sample_count+sample_index] = 0.0;
	out_y[voice_index*sample_count+sample_index] = 0.0;
	out_z[voice_index*sample_count+sample_index] = 0.0;
      }
    }
  }
  if (UNLIKELY(early)){
    sample_count -= early;
  }


  /* process loop */
  for (sample_index = offset; sample_index < sample_count;
       sample_index++) {
    /* select voice */
    for (voice_index = 0; voice_index < n_voices; voice_index++){
      /* write values to vectors */
      out_x[voice_index*sample_count+sample_index] = x_in[voice_index];
      out_y[voice_index*sample_count+sample_index] = y_in[voice_index];
      out_z[voice_index*sample_count+sample_index] = z_in[voice_index];
    }
    if (phs >= freq) {
      /* select voice */
      for (voice_index = 0; voice_index < n_voices; voice_index++){
	/* get new values when phase reaches freq =
	   sr/cps */
	do {
	  /* fuctions for the thomas attractor
	     https://en.wikipedia.org/wiki/Thomas'_cyclically_symmetric_attractor
	  */
	  x = x_vals[voice_index];
	  y = y_vals[voice_index];
	  z = z_vals[voice_index];
	  xx = x + time * (-b * x + sin(y));
	  yy = y + time * (-b * y + sin(z));
	  z_vals[voice_index] = z + time * (-b * z + sin(x));
	  x_vals[voice_index] = xx;
	  y_vals[voice_index] = yy;
	} while(--skip > 0);
      }
      /* when phase is crossing 0 the output values are	taken from
	 thomas attractor functions */
      x_in[voice_index] = x_vals[voice_index];
      y_in[voice_index] = y_vals[voice_index];
      z_in[voice_index] = z_vals[voice_index];
      /* start new phase cycle */
      phs = 0;
    } else {
      /* while phase is in cycle boundaries the output values are set
	 to 0 */
      x_in[voice_index] = 0;
      y_in[voice_index] = 0;
      z_in[voice_index] = 0;
    }
    /* increment phase */
    phs++;
  }
  /* save values for next process loop */
  p->phs = phs;
  
  /* p->x_vals.auxp = x_vals; */
  /* p->y_vals.auxp = y_vals; */
  /* p->z_vals.auxp = z_vals; */

  /* p->x_in.auxp = x_in; */
  /* p->y_in.auxp = y_in; */
  /* p->z_in.auxp = z_in; */
    
  return OK;
}
