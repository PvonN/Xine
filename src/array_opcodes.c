#include <math.h>
#include <stdint.h>
#include "csdl.h"
#include "csoundCore.h"
#include "arrays.h"

/* #include "sysdep.h" */
#include "xine.h"

//// thomas particle attractor
typedef struct {
  OPDS h;
  ARRAYDAT *aout_x, *aout_y, *aout_z; /* output */
  MYFLT *cpsp, *reset_trig, *b, *delta_time, *skip, *in_x, *in_y, *in_z, *n_particles, *max_deviation; /* input */
  MYFLT n_voices, max_dev; /* internal variables */
  AUXCH x_vals, y_vals, z_vals; /* these holds the last calculated
								   value from the attractor functions */
  AUXCH x_in, y_in, z_in; /* these holds the input to the vector,
							 either a calculated value (x_vals, y_vals
							 or z_vals) or 0*/
  int32_t phs;
} THOMAS_PARTICLE;


int32_t thomas_particle_init(CSOUND *csound, THOMAS_PARTICLE *p){
  printf("init function started\n");
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
  memset(p->x_in.auxp, 0, sizeof(MYFLT) * p->n_voices);
  csound->AuxAlloc(csound, sizeof(MYFLT) * p->n_voices, &(p->y_in));
  memset(p->y_in.auxp, 0, sizeof(MYFLT) * p->n_voices);
  csound->AuxAlloc(csound, sizeof(MYFLT) * p->n_voices, &(p->z_in));
  memset(p->z_in.auxp, 0, sizeof(MYFLT) * p->n_voices);

  csound->AuxAlloc(csound, sizeof(MYFLT) * p->n_voices, &(p->x_vals));  
  memset(p->x_vals.auxp, 0, sizeof(MYFLT) * p->n_voices);
  csound->AuxAlloc(csound, sizeof(MYFLT) * p->n_voices, &(p->y_vals));
  memset(p->y_vals.auxp, 0, sizeof(MYFLT) * p->n_voices);
  csound->AuxAlloc(csound, sizeof(MYFLT) * p->n_voices, &(p->z_vals));
  memset(p->z_vals.auxp, 0, sizeof(MYFLT) * p->n_voices);

  /* init start values */
  MYFLT *x_vals = (MYFLT *) p->x_vals.auxp;
  MYFLT *y_vals = (MYFLT *) p->y_vals.auxp;
  MYFLT *z_vals = (MYFLT *) p->z_vals.auxp;

  p->max_dev = *p->max_deviation;
  for (int32_t i = 0; i < p->n_voices; i++){
    x_vals[i] = *p->in_x + signed_deviation(p->max_dev, p->n_voices, i);
    y_vals[i] = *p->in_y + signed_deviation(p->max_dev, p->n_voices, i);
    z_vals[i] = *p->in_z + signed_deviation(p->max_dev, p->n_voices, i);
  }
  printf("init function closed\n");
  return OK;
}

int32_t thomas_particle_process(CSOUND *csound, THOMAS_PARTICLE *p){
  printf("processing loop started\n");
  /* sample accurate mechanism */
  uint32_t offset = p->h.insdshead->ksmps_offset;
  uint32_t early  = p->h.insdshead->ksmps_no_end;
  /* variables */
  MYFLT sr = csound->GetSr(csound);
  uint32_t sample_count = CS_KSMPS;
  uint32_t n_voices = p->n_voices;
  MYFLT max_deviation = p->max_dev;
  /* oscillator stuff */
  int32_t phs = p->phs;
  MYFLT *cpsp = p->cpsp;
  MYFLT freq = sr / *cpsp++;

  /* set output arrays */
  MYFLT *out_x = (MYFLT *) p->aout_x;
  MYFLT *out_y = (MYFLT *) p->aout_y;
  MYFLT *out_z = (MYFLT *) p->aout_z;
  
  /* set time variant values */
  MYFLT time = *p->delta_time;
  int32_t skip = (int32_t)*p->skip;
  MYFLT b = *p->b;

  /* setting start values for x, y, z*/
  MYFLT *x_vals = (MYFLT *) p->x_vals.auxp;
  MYFLT *y_vals = (MYFLT *) p->y_vals.auxp;
  MYFLT *z_vals = (MYFLT *) p->z_vals.auxp;

  /* set input for writing to vectors */
  MYFLT *x_in = (MYFLT *) p->x_in.auxp;
  MYFLT *y_in = (MYFLT *) p->y_in.auxp;
  MYFLT *z_in = (MYFLT *) p->z_in.auxp;

  /* trigger mechanism */
  if (*p->reset_trig){
    p->phs = 0;
    phs = p->phs;

    for (int voice_index = 0; voice_index < n_voices; voice_index++){
      x_vals[voice_index] = *p->in_x + signed_deviation(*p->max_deviation, p->n_voices, voice_index);
      y_vals[voice_index] = *p->in_y + signed_deviation(*p->max_deviation, p->n_voices, voice_index);
      z_vals[voice_index] = *p->in_z + signed_deviation(*p->max_deviation, p->n_voices, voice_index);
    }

    x_in = p->x_in.auxp;
    y_in = p->y_in.auxp;
    z_in = p->z_in.auxp;
  }
  

  /* sample accurate machanism */
  if (UNLIKELY(offset)){
    for (int voice_index = 0; voice_index < n_voices; voice_index++ ) {
      for (int sample_index = offset; sample_index < sample_count;
		   sample_index++) {
		out_x[(voice_index*sample_count)+sample_index] = 0;
		out_y[(voice_index*sample_count)+sample_index] = 0;
		out_z[(voice_index*sample_count)+sample_index] = 0;
      }
    }
  }
  if (UNLIKELY(early)){
    sample_count -= early;
  }

  /* process loop */
  MYFLT x, y, z, xx, yy;
  /* choose array item first then go to sample loop */
  for (int voice_index = 0; voice_index < n_voices; voice_index++) { 
	for (int sample_index = offset; sample_index < sample_count;
		 sample_index++) {
	  /* write values to vectors first to get the start value into the
		 vector*/
	  out_x[(voice_index*sample_count)+sample_index] =
		x_in[sample_index];
	  out_y[(voice_index*sample_count)+sample_index] =
		y_in[sample_index];
	  out_z[(voice_index*sample_count)+sample_index] =
		z_in[sample_index];

	  /* oscillator loop */
	  if (phs >= freq) {
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
		/* set input for the output vectors when phasor is crossing
		   zero to the calculated value */
		x_in[voice_index] = x_vals[voice_index];
		y_in[voice_index] = y_vals[voice_index];
		z_in[voice_index] = z_vals[voice_index];

		/* reset phasor */
		phs = 0;
	  } else {
		/* set input for the output vectors when phasor is NOT crossing
		   zero to 0 */
		x_in[voice_index] = 0;
		y_in[voice_index] = 0;
		z_in[voice_index] = 0;
	  }
	  /* increment phasor */
	  phs++;
	}

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
