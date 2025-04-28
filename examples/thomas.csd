<CsoundSynthesizer>
<CsOptions>
-d -odac 
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 16
nchnls = 2
0dbfs = 1.0

instr 1
  aFreq = 10000
  kResetTrigger = 0
  kB = randomi(0.05,0.329,1)
  kDelta = randomh(0.1, 0.9, 0.25)
  kSkip = randomh(1, 20, 1)
  iX = 2.17
  iY = 0.35
  iZ = 1.16

  aX, aY, aZ thomas aFreq, kResetTrigger, kB, kDelta, kSkip, iX, iY, iZ

  // pregain
  iPreGain = 0.05
  aX *= iPreGain
  aY *= iPreGain
  aZ *= iPreGain

  // output
  aX1, aX2 pan2 aX, 0
  aY1, aY2 pan2 aY, 0.5
  aZ1, aZ2 pan2 aZ, 1
  aOut1 = sum(aX1, aY1, aZ1)
  aOut2 = sum(aX2, aY2, aZ2)
  outs(limit(dcblock2(aOut1),-1,1), limit(dcblock2(aOut2),-1,1))
endin

</CsInstruments>
<CsScore>
i1 0 20
</CsScore>
</CsoundSynthesizer>

