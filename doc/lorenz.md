# Lorenz

## Abstract

GEN routine based on the Lorenz attractor


## Description

``lorenz`` creates a ft based on data created by a Lorenz attractor. 

The Lorenz attractor is a chaotic, deterministic system described by
three equations for the variables x, y, and z. It exhibits nonlinear
behavior where small changes in initial conditions can lead to vastly
different outcomes. The system forms a distinctive "butterfly shape,"
which is known for its chaos and sensitivity to initial conditions.


## Syntax


```csound

f1 0 4096 "lorenz" 0 30 1600 -1 5 0.001 2 1 0 0 0 0

```
    
## Arguments
* p5 = choose axis to select values from; 0 = x, 1 = y, 2 = z
* p6 = min output; if p6 and p7 == 0 don't scale
* p7 = max output; if p6 and p7 == 0 don't scale
* p8 = normalize; 0 == normalize, -1 == don't normalize
* p9 = stepsize
* p10 = x start value
* p11 = y start value
* p12 = z start value
* [p13 = sigma; 0 == default -> 10] 
* [p14 = rho; 0 == default -> 28 ] 
* [p15 = beta; 0 == default -> 8./3.]  
* [p16 = time delta; 0 == default -> 0.001 ] 


## Output


## Execution Time

* Init 

## Examples


```csound

<CsoundSynthesizer>
<CsOptions>
-odac
</CsOptions>
<CsInstruments>

sr = 44100
ksmps = 16
nchnls = 2
0dbfs = 1.0

instr 1
  iX random -1, 1
  iY random -1, 1
  iZ random -1, 1
  iLorenzFreqs ftgen 0, 0, 16384, "lorenz", 0, 80, 1600, -1, 5, iX,\
    iY, iZ, 0, 0, 0, 0
  
  aIndex line 0,p3,1  
  aFreq table3 aIndex, iLorenzFreqs, 1
  aSig poscil3 0.8, aFreq  
  aEnv linseg 0,0.02,1,p3-0.02,1,0.02,0

  outs aSig, aSig
endin

</CsInstruments>
<CsScore>
i1 0 20
</CsScore>
</CsoundSynthesizer>

```


## See also

* [lorenz attractor](https://en.wikipedia.org/wiki/Lorenz_system)

## Credits

Philipp von Neumann, 2024