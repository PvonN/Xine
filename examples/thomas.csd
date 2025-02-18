<CsoundSynthesizer>
<CsOptions>
-d -odac -M2
</CsOptions>
<CsInstruments>

sr = 44100
ksmps = 16
nchnls = 2
0dbfs = 1.0

/****** readTableAndPrint
- takes a k signal and indexes a interpolated table
- the output has a portamento applied
- prints any string by the printks2 opcode
- with optional normalizing the table to indexes between 0 an 1
******/ 
opcode readTableAndPrint, k, kiiSo
  kControl, iTable, iPort, String, iNorm xin

  kOut tablei kControl, iTable, iNorm
  
  printks2 String, kOut
  kOut port kOut, iPort
  
  xout kOut
endop

/* 
basic setup for faderfox EC4 MIDI controller
- only for CC commands
- layout for standard layout on channel 1
- output 1 is an array with index 0 - 15
- output 3 is an array with index 0 - 15 and portamento
- allows scaling of the output
*/

opcode faderfoxEC4CC, k[]k[], iiii
  iChn, iMin, iMax, iPort xin

  kCC[] init 16
  kCCPort[] init 16
  
  #define CC_PORT(Chn'CC'Port'Min'Max)
  #
  kCC[$CC] ctrl7 $Chn, $CC, $Min, $Max
  kCCPort[$CC] port kCC[$CC], $Port
  #

  $CC_PORT(iChn'0'iPort'iMin'iMax)
  $CC_PORT(iChn'1'iPort'iMin'iMax)
  $CC_PORT(iChn'2'iPort'iMin'iMax)
  $CC_PORT(iChn'3'iPort'iMin'iMax)
  $CC_PORT(iChn'4'iPort'iMin'iMax)
  $CC_PORT(iChn'5'iPort'iMin'iMax)
  $CC_PORT(iChn'6'iPort'iMin'iMax)
  $CC_PORT(iChn'7'iPort'iMin'iMax)
  $CC_PORT(iChn'8'iPort'iMin'iMax)
  $CC_PORT(iChn'9'iPort'iMin'iMax)
  $CC_PORT(iChn'10'iPort'iMin'iMax)
  $CC_PORT(iChn'11'iPort'iMin'iMax)
  $CC_PORT(iChn'12'iPort'iMin'iMax)
  $CC_PORT(iChn'13'iPort'iMin'iMax)
  $CC_PORT(iChn'14'iPort'iMin'iMax)
  $CC_PORT(iChn'15'iPort'iMin'iMax)

  xout kCC, kCCPort
endop

opcode faderfoxEC4_trigger, k, ii
  iChan, iNote xin

  kPrevStatus init 0
  kStatus, kChan, kData1, kData2 midiin 

  kState = (kStatus == 0) && (kPrevStatus == 144) && \
    (kChan == iChan) && (kData1 == iNote) ? 1 : kState
  kState = (kStatus == 0) && (kPrevStatus == 128) && \
    (kChan == iChan) && (kData1 == iNote) ? 0 : kState

  kPrevStatus = kStatus

  kOut = kState == 1 ? 1 : 0
  
  xout kOut
endop

massign 0, 0
alwayson "faderfoxEC4_CC"
instr faderfoxEC4_CC
  //---------- midi controler ----------//
  kCC[], kCCPort[] faderfoxEC4CC 1, 0, 1, 0.02

  // table for midi controler
  iB ftgen 0, 0, 4096, -7, 0.0001, 4096, 0.9
  iDelta ftgen 0, 0, 4096, -7, 0.001, 4096, 1
  iSkip ftgen 0, 0, 4096, -7, 1, 4096, 500
  iFreq ftgen 0, 0, 4096, -5, 1, 4096, 16000
  
  gkB readTableAndPrint kCC[0], iB, 0.02, "B: %f\n", 1
  gkDelta readTableAndPrint kCC[1], iDelta, 0.02, "Delta: %f\n", 1
  gkSkip readTableAndPrint kCC[2], iSkip, 0.02, "Skip: %d\n", 1
  gkFreq readTableAndPrint kCC[3], iFreq, 0.02, "Freq: %d\n", 1

  gkTrig faderfoxEC4_trigger 1, 0
  printks2 "Trigger: %d\n", gkTrig
endin

;; instr 1
;;   kB = gkB
;;   kDelta = gkDelta
;;   iSkip = 4.0
;;   iX = 0.3
;;   iY = .20
;;   iZ = 1.0
;;   iNum_Particles = 10.
;;   iMax_Devi = 0
;;   aX[], aY[], aZ[] thomas  kB, kDelta, iSkip, iX, iY, iZ, iNum_Particles, iMax_Devi
  
;;   kGain = 0.5
;;   aX *= kGain
;;   aY *= kGain
;;   aZ *= kGain
;;   kIndex = line(0, p3, iNum_Particles)
;;   kIndex = round(kIndex)
;;   printk2 kIndex
;;   printk2 k(aX[kIndex])
;;   outs aX[kIndex], aX[kIndex]
;; endin

instr 2

  kTrigger = gkTrig
  kB = gkB
  kDelta = gkDelta
  kSkip = gkSkip

  aFreq = a(gkFreq)

  iX = 2.17
  iY = 0.35
  iZ = 1.16

  aX, aY, aZ thomasi aFreq, kTrigger, kB, kDelta, kSkip, iX,\
    iY, iZ
  
  kGain = 0.125
  aX *= kGain
  aY *= kGain
  aZ *= kGain
  aOut1 = limit(aZ, -0.9, 0.9)
  aOut2 = limit(aY, -0.9, 0.9)
  aOut1 = dcblock(aOut1)
  aOut2 = dcblock(aOut2)
  outs aOut1, aOut2
endin

</CsInstruments>
<CsScore>
i2 0 z
</CsScore>
</CsoundSynthesizer>

