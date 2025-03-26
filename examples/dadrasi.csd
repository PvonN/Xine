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
  aFreq = randomi(20, 100, 1)
  kResetTrigger = 0
  kA = 3
  kB = 2.7
  kC = 1.7
  kD = 2
  kE = 9
  kDelta = 0.15
  kSkip = 10
  iX = 1.1
  iY = 2.1
  iZ = -2

  aX, aY, aZ dadrasi aFreq, kResetTrigger, kA, kB, kC, kD, kE, kDelta, kSkip, iX, iY, iZ

  // pregain
  iPreGain = 0.05
  aX *= iPreGain
  aY *= iPreGain
  aZ *= iPreGain

  // output
  iChn = 0
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

