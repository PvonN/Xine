<CsoundSynthesizer>
<CsOptions>
-odac
</CsOptions>
<CsInstruments>

sr = 44100
ksmps = 16
nchnls = 2
0dbfs = 1.0
turnoff 
instr 1
  aFreq = 10000
  kResetTrig = 0
  kSigma =  randomi(10,12,1) // 10 good default
  kRho = randomi(28,20,1)    // 28 good default
  kBeta = 8/3                // 8/3 good default
  kDelta = randomi(0.0001,0.01,1)
  kSkip = randomi(1,5,1)
  iX = 2.3
  iY = .20
  iZ = 1.0
  aX, aY, aZ lorenzh aFreq, kResetTrig, kSigma, kRho, kBeta, kDelta, kSkip, iX, iY, iZ

  // pregain
  iPreGain = 0.015
  aX *= iPreGain
  aY *= iPreGain
  aZ *= iPreGain


  // output
  aX1, aX2 pan2 aX, 0
  aY1, aY2 pan2 aY, 0.5
  aZ1, aZ2 pan2 aZ, 1
  aOut1 = sum(aX1, aY1, aZ1)
  aOut2 = sum(aX2, aY2, aZ2)
  outs((limit(dcblock2(aOut1),-1,1)), limit(dcblock2(aOut2),-1,1)) 
endin

</CsInstruments>
<CsScore>
i1 0 20
</CsScore>
</CsoundSynthesizer>

