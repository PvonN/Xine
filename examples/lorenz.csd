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
  kSigma =  10
  kRho = 28
  kBeta = 8/3
  kDelta = 0.001
  kSkip = 1.0
  iX = 0.3
  iY = .20
  iZ = 1.0
  iNum_Particles = 2.
  iMax_Devi = 1.
  aX[], aY[], aZ[] lorenzparticle kSigma, kRho, kBeta, kDelta, kSkip, iX, iY, iZ, iNum_Particles, iMax_Devi
  
  kGain = 1
  aX *= kGain
  aY *= kGain
  aZ *= kGain

  outs aX[0], aX[0]
endin

</CsInstruments>
<CsScore>
i1 0 20
</CsScore>
</CsoundSynthesizer>

