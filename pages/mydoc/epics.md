---
title: Epics Channels
keywords: epics
sidebar: mydoc_sidebar
permalink: epics.html
---


## __EPICS ODB Tree__

The table below lists each DRAGON's EPICS device according to its channel number in the Epics ODB tree.

{% include important.html content="The device channels listed below are the current EPICS channel values (circa Jan. 2016 - present). If you are working with data from another time period, consult the `.xml` files in your data set to find the desired EPICS channel by searching for the \"EPICS_NAMES\" key array in Epics ODB tree:" %}


``` xml
<dir name="Epics">
  <dir name="Common">
  Keys in "Common"
  .
  .
  .
  </dir>
  <dir name="Variables">
  Keys in "Variables"
  .
  .
  .
  </dir>
  <dir name="Statistics">
  Keys in "Statistics"
  .
  .
  .
  </dir>
  <dir name="Settings">
    <key name="Command" type="STRING" size="80">status</key>
    <keyarray name="EPICS_Names" type="STRING" size="32" num_values="374">
      <value index="0">DRA:CMGC:RDVAC</value>
      <value index="1">DRA:CMTRIN:RDVAC</value>
      <value index="2">DRA:FCCH:RDCUR</value>
      <value index="3">DRA:XSLITC:RDCUR1</value>
      <value index="4">DRA:XSLITC:RDCUR2</value>
      <value index="5">DRA:YSLITC:RDCUR1</value>
      <value index="6">DRA:YSLITC:RDCUR2</value>
      <value index="7">DRA:FCM:RDCUR</value>
      <value index="8">DRA:XSLITM:SCALECUR1</value>
      <value index="9">DRA:XSLITM:SCALECUR2</value>
      <value index="10">DRA:YSLITM:SCALECUR1</value> 
      .
      .
      .
    </keyarray>  
    More keys in "Settings"
    .
    .
    .
  </dir>
</dir>
```


Channel | Epics Device Name      | Description                                                     |
--------|------------------------|----------------------------------------------------------------
[0]     | `DRA:CMGC:RDVAC`       | Gas target pressure [Torr]
[1]     | `DRA:TCM1:GT:RDTEMP`   | Gas target temperature [&deg;C]
[2]     | `CCS2ISAC:BL2ACURRENT` | BL2A Proton current [&mu;A]
[3]     | `DRA:TCM1:SP6:RDTEMP`  | `<description placholder>`
[4]     | `DRA:TCM1:SP7:RDTEMP`  | `<description placeholder>`
[5]     | `HEBT2:FC4:SCALECUR`   | FC4 current readback [A]
[6]     | `HEBT2:FC4:GAIN`       | FC4 gain setting
[7]     | `HEBT2:FC4:STATON`     | FC4 inserted
[8]     | `HEBT2:FC4CI:SCALECUR` | FC4 current integrator readback [A]
[9]     | `DRA:ATTN0:STATON`     | DRAGON attenuator inserted
[10]    | `DRA:Q1:CUR`           | Q1 current set point [A]
[11]    | `DRA:Q1:RDCUR`         | Q1 current set point [A]
[12]    | `DRA:HALLQ1:STATGAIN0` | Q1 Hall probe gain setting
[13]    | `DRA:HALLQ1:RDFIELD`   | Q1 Hall probe field read back [Gauss]
[14]    | `DRA:Q2:CUR`           | Q2 current set point [A]
[15]    | `DRA:Q2:RDCUR`         | Q2 current set point [A]
[16]    | `DRA:HALLQ2:STATGAIN0` | Q2 Hall probe gain setting
[17]    | `DRA:HALLQ2:RDFIELD`   | Q2 Hall probe field read back [Gauss]
[18]    | `DRA:FC1:SCALECUR`     | FC1 current readback [A]
[19]    | `DRA:FC1:GAIN`         | FC1 gain setting
[20]    | `DRA:FC1:STATON`       | FC1 inserted
[21]    | `DRA:FC1CI:SCALECUR`   | FC1 current integrator readback [A]
[22]    | `DRA:MD1:RDCUR`        | MD1 Current read back [A]
[23]    | `DRA:MD1:RDNMR`        | MD1 NMR probe read back [Gauss]
[24]    | `DRA:MD1:CUR`          | MD1 Current set point [A]
[25]    | `DRA:PROFCH:MOTOR.LLS` | Profile monitor in limit switch engaged 
[26]    | `DRA:PROFCH:MOTOR.HLS` | Profile monitor out limit switch engaged
[27]    | `DRA:MINIIC:MOTOR.RBV` | Mini IC position read back
[28]    | `DRA:MINIIC:MOTOR.VAL` | Mini IC position set point
[29]    | `DRA:MINIIC:MOTOR.LLS` | Mini IC in limit switch engaged 
[30]    | `DRA:MINIIC:MOTOR.HLS` | Mini IC out limit switch engaged
[31]    | `DRA:XSLITC:SCALECUR1` | Left horizontal charge slit current read back [A]
[32]    | `DRA:XSLITC:SCALECUR2` | Right horizontal charge slit current read back [A]
[33]    | `DRA:XSLITC:MOTION.F`  | `<description placeholder>`
[34]    | `DRA:XSLITC:MOTION.G`  | `<description placeholder>`
[35]    | `DRA:SLITC:X1`         | `<description placeholder>`
[36]    | `DRA:SLITC:X2`         | `<description placeholder>`
[37]    | `DRA:XSLITC:WIDSP`     | `<description placeholder>`
[38]    | `DRA:XSLITC:POSSP`     | `<description placeholder>`
[39]    | `DRA:XSLITC:GAIN`      | Vertical charge slits gain setting
[40]    | `DRA:YSLITC:SCALECUR1` | Top vertical charge slit current read back [A]
[41]    | `DRA:YSLITC:SCALECUR2` | Bottom vertical charge slit current read back [A]
[42]    | `DRA:YSLITC:MOTION.F`  | `<description placeholder>`
[43]    | `DRA:YSLITC:MOTION.G`  | `<description placeholder>`
[44]    | `DRA:XSLITC:WIDSP`     | `<description placeholder>`
[45]    | `DRA:XSLITC:POSSP`     | `<description placeholder>`
[46]    | `DRA:XSLITC:GAIN`      | Vertical charge slits gain setting
[47]    | `DRA:FCCH:SCALECUR`    | FCCH current readback [A]
[48]    | `DRA:FCCH:GAIN`        | FCCH gain setting
[49]    | `DRA:FCCH:STATON`      | FCCH inserted
[50]    | `DRA:FCCHCI:SCALECUR`  | FCCH current integrator readback [A]
[51]    | `DRA:SM1X:CUR`         | Horizontal steering magnet 1 current setpoint
[52]    | `DRA:SM1X:RDCUR`     | Horizontal steering magnet 1 current readback
[53]    | `DRA:SM1Y:CUR`     | `<description placeholder>`
[54]    | `DRA:SM1Y:RDCUR`     | `<description placeholder>`
[55]    | `DRA:SX1:CUR`     | `<description placeholder>`
[56]    | `DRA:SX1:RDCUR`     | `<description placeholder>`
[57]    | `DRA:Q3:CUR`     | `<description placeholder>`
[58]    | `DRA:Q3:RDCUR`     | `<description placeholder>`
[59]    | `DRA:HALLQ3:STATGAIN0`     | `<description placeholder>`
[60]    | `DRA:HALLQ3:RDFIELD`     | `<description placeholder>`
[61]    | `DRA:Q4:CUR`     | `<description placeholder>`
[62]    | `DRA:Q4:RDCUR`     | `<description placeholder>`
[63]    | `DRA:HALLQ4:STATGAIN0`     | `<description placeholder>`
[64]    | `DRA:HALLQ4:RDFIELD`     | `<description placeholder>`
[65]    | `DRA:Q5:CUR`     | `<description placeholder>`
[66]    | `DRA:Q5:RDCUR`     | `<description placeholder>`
[67]    | `DRA:HALLQ5:STATGAIN0`     | `<description placeholder>`
[68]    | `DRA:HALLQ5:RDFIELD`     | `<description placeholder>`
[69]    | `DRA:SX2:CUR`     | `<description placeholder>`
[70]    | `DRA:SX2:RDCUR`     | `<description placeholder>`
[71]    | `DRA:BCM2:STATON`     | `<description placeholder>`
[72]    | `DRA:ED1:POS:VOL`     | `<description placeholder>`
[73]    | `DRA:ED1:POS:RDVOL`     | `<description placeholder>`
[74]    | `DRA:ED1:NEG:RDVOL`     | `<description placeholder>`
[75]    | `DRA:ED1:POS:RDCUR`     | `<description placeholder>`
[76]    | `DRA:ED1:NEG:RDCUR`     | `<description placeholder>`
[77]    | `DRA:ED1:IPOS:CUR`     | `<description placeholder>`
[78]    | `DRA:ED1:INEG:CUR`     | `<description placeholder>`
[79]    | `DRA:XRAY1:RDCOUNT`     | `<description placeholder>`
[80]    | `DRA:XSLITM:SCALECUR1`     | `<description placeholder>`
[81]    | `DRA:XSLITM:SCALECUR2`     | `<description placeholder>`
[82]    | `DRA:XSLITM:GAIN`     | `<description placeholder>`
[83]    | `DRA:XSLITM:WIDSP`     | `<description placeholder>`
[84]    | `DRA:XSLITM:MOTION.F`     | `<description placeholder>`
[85]    | `DRA:XSLITM:POSSP`     | `<description placeholder>`
[86]    | `DRA:XSLITM:MOTION.G`     | `<description placeholder>`
[87]    | `DRA:YSLITM:SCALECUR1`     | `<description placeholder>`
[88]    | `DRA:YSLITM:SCALECUR2`     | `<description placeholder>`
[89]    | `DRA:YSLITM:GAIN`     | `<description placeholder>`
[90]    | `DRA:YSLITM:MOTION.F`     | `<description placeholder>`
[91]    | `DRA:YSLITM:WIDSP`     | `<description placeholder>`
[92]    | `DRA:YSLITM:MOTION.G`     | `<description placeholder>`
[93]    | `DRA:YSLITM:POSSP`     | `<description placeholder>`
[94]    | `DRA:FCM:SCALECUR`     | `<description placeholder>`
[95]    | `DRA:FCM:GAIN`     | `<description placeholder>`
[96]    | `DRA:FCM:STATON`     | `<description placeholder>`
[97]    | `DRA:SM2X:CUR`     | `<description placeholder>`
[98]    | `DRA:SM2X:RDCUR`     | `<description placeholder>`
[99]    | `DRA:SM2Y:CUR`     | `<description placeholder>`
[100]    | `DRA:SM2Y:RDCUR`     | `<description placeholder>`
[101]    | `DRA:Q6:CUR`     | `<description placeholder>`
[102]    | `DRA:Q6:RDCUR`     | `<description placeholder>`
[103]    | `DRA:HALLQ6:STATGAIN0`     | `<description placeholder>`
[104]    | `DRA:HALLQ6:RDFIELD`     | `<description placeholder>`
[105]    | `DRA:Q7:CUR`     | `<description placeholder>`
[106]    | `DRA:Q7:RDCUR`     | `<description placeholder>`
[107]    | `DRA:HALLQ7:STATGAIN0`     | `<description placeholder>`
[108]    | `DRA:HALLQ7:RDFIELD`     | `<description placeholder>`
[109]    | `DRA:SX3:CUR`     | `<description placeholder>`
[110]    | `DRA:SX3:RDCUR`     | `<description placeholder>`
[111]    | `DRA:BCM3:STATON`     | `<description placeholder>`
[112]    | `DRA:MD2:CUR`     | `<description placeholder>`
[113]    | `DRA:MD2:RDCUR`     | `<description placeholder>`
[114]    | `DRA:MD2:RDNMR`     | `<description placeholder>`
[115]    | `DRA:BCM4:STATON`     | `<description placeholder>`
[116]    | `DRA:SM3X:CUR`     | `<description placeholder>`
[117]    | `DRA:SM3X:RDCUR`     | `<description placeholder>`
[118]    | `DRA:SM3Y:CUR`     | `<description placeholder>`
[119]    | `DRA:SM3Y:RDCUR`     | `<description placeholder>`
[120]    | `DRA:Q8:CUR`     | `<description placeholder>`
[121]    | `DRA:Q8:RDCUR`     | `<description placeholder>`
[122]    | `DRA:HALLQ8:STATGAIN0`     | `<description placeholder>`
[123]    | `DRA:HALLQ8:RDFIELD`     | `<description placeholder>`
[124]    | `DRA:SX4:CUR`     | `<description placeholder>`
[125]    | `DRA:SX4:RDCUR`     | `<description placeholder>`
[126]    | `DRA:BCM5:STATON`     | `<description placeholder>`
[127]    | `DRA:ED2:POS:VOL`     | `<description placeholder>`
[128]    | `DRA:ED2:POS:RDVOL`     | `<description placeholder>`
[129]    | `DRA:ED2:NEG:RDVOL`     | `<description placeholder>`
[130]    | `DRA:ED2:POS:RDCUR`     | `<description placeholder>`
[131]    | `DRA:ED2:NEG:RDCUR`     | `<description placeholder>`
[132]    | `DRA:ED2:IPOS:CUR`     | `<description placeholder>`
[133]    | `DRA:ED2:INEG:CUR`     | `<description placeholder>`
[134]    | `DRA:XRAY2:RDCOUNT`     | `<description placeholder>`
[135]    | `DRA:BCM6:STATON`     | `<description placeholder>`
[136]    | `DRA:SM4X:CUR`     | `<description placeholder>`
[137]    | `DRA:SM4X:RDCUR`     | `<description placeholder>`
[138]    | `DRA:SM4Y:CUR`     | `<description placeholder>`
[139]    | `DRA:SM4Y:RDCUR`     | `<description placeholder>`
[140]    | `DRA:Q9:CUR`     | `<description placeholder>`
[141]    | `DRA:Q9:RDCUR`     | `<description placeholder>`
[142]    | `DRA:HALLQ9:STATGAIN0`     | `<description placeholder>`
[143]    | `DRA:HALLQ9:RDFIELD`     | `<description placeholder>`
[144]    | `DRA:Q10:CUR`     | `<description placeholder>`
[145]    | `DRA:Q10:RDCUR`     | `<description placeholder>`
[146]    | `DRA:HALLQ10:STATGAIN0`     | `<description placeholder>`
[147]    | `DRA:HALLQ10:RDFIELD`     | `<description placeholder>`
[148]    | `DRA:MCP0:MOTORWID.LLS`     | `<description placeholder>`
[149]    | `DRA:MCP0:MOTORWID.HLS`     | `<description placeholder>`
[150]    | `DRA:MCP0:MOTORWID.VAL`     | `<description placeholder>`
[151]    | `DRA:XSLITF:SCALECUR1`     | `<description placeholder>`
[152]    | `DRA:XSLITF:SCALECUR2`     | `<description placeholder>`
[153]    | `DRA:XSLITF:GAIN`     | `<description placeholder>`
[154]    | `DRA:XSLITF:POSSP`     | `<description placeholder>`
[155]    | `DRA:XSLITF:MOTION.G`     | `<description placeholder>`
[156]    | `DRA:XSLITF:WIDSP`     | `<description placeholder>`
[157]    | `DRA:XSLITF:MOTION.F`     | `<description placeholder>`
[158]    | `DRA:YSLITF:SCALECUR1`     | `<description placeholder>`
[159]    | `DRA:YSLITF:SCALECUR2`     | `<description placeholder>`
[160]    | `DRA:YSLITF:MOTION.F`     | `<description placeholder>`
[161]    | `DRA:YSLITF:WIDSP`     | `<description placeholder>`
[162]    | `DRA:YSLITF:MOTION.G`     | `<description placeholder>`
[163]    | `DRA:YSLITF:POSSP`     | `<description placeholder>`
[164]    | `DRA:FCF:SCALECUR`     | `<description placeholder>`
[165]    | `DRA:FCF:STATON`     | `<description placeholder>`
[166]    | `DRA:FCF:GAIN`     | `<description placeholder>`
[167]    | `DRA:FCFCI:SCALECUR`     | `<description placeholder>`
[168]    | `DRA:MCP1:MOTORWID.LLS`     | `<description placeholder>`
[169]    | `DRA:MCP1:MOTORWID.HLS`     | `<description placeholder>`
[170]    | `DRA:MCP1:MOTORWID.VAL`     | `<description placeholder>`
[171]    | `DRA:BIAS1:RDVOL`     | `<description placeholder>`
[172]    | `DRA:BIAS2:RDVOL`     | `<description placeholder>`
[173]    | `DRA:BIAS3:RDVOL`     | `<description placeholder>`
[174]    | `DRA:BIAS6:RDVOL`     | `<description placeholder>`
[175]    | `DRA:MEVREF`     | `<description placeholder>`
[176]    | `DRA:MASSREF`     | `<description placeholder>`
[177]    | `DRA:CHARGEREF`     | `<description placeholder>`
[178]    | `DRA:NMR1REF`     | `<description placeholder>`
[179]    | `DRA:NMR2REF`     | `<description placeholder>`
[180]    | `DRA:ED1VOLREF`     | `<description placeholder>`
[181]    | `DRA:ED2VOLREF`     | `<description placeholder>`
[182]    | `DRA:MOMREF`     | `<description placeholder>`
[183]    | `DRA:ETAREF`     | `<description placeholder>`
[184]    | `DRA:EPERUREF`     | `<description placeholder>`
[185]    | `DRA:RESPGREF`     | `<description placeholder>`
[186]    | `DRA:FREEZEMOMENTUM`     | `<description placeholder>`
[187]    | `DRA:MEVTUNE`     | `<description placeholder>`
[188]    | `DRA:MASSTUNE`     | `<description placeholder>`
[189]    | `DRA:CHARGETUNE`     | `<description placeholder>`
[190]    | `DRA:NMR1TUNE`     | `<description placeholder>`
[191]    | `DRA:NMR2TUNE`     | `<description placeholder>`
[192]    | `DRA:ED1SCALEVOL`     | `<description placeholder>`
[193]    | `DRA:ED2SCALEVOL`     | `<description placeholder>`
[194]    | `ISACPLC7:STATAUTO`     | `<description placeholder>`
[195]    | `DRA:BP31:STATON`     | `<description placeholder>`
[196]    | `DRA:PV31:STATON`     | `<description placeholder>`
[197]    | `DRA:CG31A:RDVAC`     | `<description placeholder>`
[198]    | `DRA:CG31B:RDVAC`     | `<description placeholder>`
[199]    | `HEBT2:IV8:STATON`     | `<description placeholder>`
[200]    | `DRA:RB1B:STATON`     | `<description placeholder>`
[201]    | `DRA:RB1B:RDTEMP`     | `<description placeholder>`
[202]    | `DRA:RB1A:STATON`     | `<description placeholder>`
[203]    | `DRA:RB1A:RDTEMP`     | `<description placeholder>`
[204]    | `DRA:RB2A:STATON`     | `<description placeholder>`
[205]    | `DRA:RB2A:RDTEMP`     | `<description placeholder>`
[206]    | `DRA:RB1:STATON`     | `<description placeholder>`
[207]    | `DRA:RB1:RDTEMP`     | `<description placeholder>`
[208]    | `DRA:RB2:STATON`     | `<description placeholder>`
[209]    | `DRA:RB2:RDTEMP`     | `<description placeholder>`
[210]    | `DRA:TP1:STATON`     | `<description placeholder>`
[211]    | `DRA:TP1:RDCUR`     | `<description placeholder>`
[212]    | `DRA:TP2:STATON`     | `<description placeholder>`
[213]    | `DRA:TP2:RDCUR`     | `<description placeholder>`
[214]    | `DRA:TP3:STATON`     | `<description placeholder>`
[215]    | `DRA:TP3:RDCUR`     | `<description placeholder>`
[216]    | `DRA:TP4:STATON`     | `<description placeholder>`
[217]    | `DRA:TP4:RDCUR`     | `<description placeholder>`
[218]    | `DRA:TP5:STATON`     | `<description placeholder>`
[219]    | `DRA:TP5:RDCUR`     | `<description placeholder>`
[220]    | `DRA:TP6:STATON`     | `<description placeholder>`
[221]    | `DRA:TP6:RDCUR`     | `<description placeholder>`
[222]    | `DRA:TP7:STATON`     | `<description placeholder>`
[223]    | `DRA:TP7:RDCUR`     | `<description placeholder>`
[224]    | `DRA:TP9:STATON`     | `<description placeholder>`
[225]    | `DRA:TP9:RDCUR`     | `<description placeholder>`
[226]    | `DRA:CG2:RDVAC`     | `<description placeholder>`
[227]    | `DRA:CG3:RDVAC`     | `<description placeholder>`
[228]    | `DRA:IGU3:RDVAC`     | `<description placeholder>`
[229]    | `DRA:IGD4:RDVAC`     | `<description placeholder>`
[230]    | `DRA:CMTRIN:RDVAC`     | `<description placeholder>`
[231]    | `DRA:CM1:RDVAC`     | `<description placeholder>`
[232]    | `DRA:XCG:RDVAC`     | `<description placeholder>`
[233]    | `DRA:CM4:RDVAC`     | `<description placeholder>`
[234]    | `DRA:CMBT:RDVAC`     | `<description placeholder>`
[235]    | `DRA:CGRP:RDVAC`     | `<description placeholder>`
[236]    | `DRA:VNT1:STATON`     | `<description placeholder>`
[237]    | `DRA:LNLVL1:STAT.BE`     | `<description placeholder>`
[238]    | `DRA:IV11:STATON`     | `<description placeholder>`
[239]    | `DRA:IV21:STATON`     | `<description placeholder>`
[240]    | `DRA:RP21:STATON`     | `<description placeholder>`
[241]    | `DRA:CG21B:RDVAC`     | `<description placeholder>`
[242]    | `DRA:PV21:STATON`     | `<description placeholder>`
[243]    | `DRA:RV31:STATON`     | `<description placeholder>`
[244]    | `DRA:VV21A:STATON`     | `<description placeholder>`
[245]    | `DRA:CG21A:RDVAC`     | `<description placeholder>`
[246]    | `DRA:RV21:DRVON`     | `<description placeholder>`
[247]    | `DRA:VV21:STATON`     | `<description placeholder>`
[248]    | `DRA:CG21:RDVAC`     | `<description placeholder>`
[249]    | `DRA:IG21:RDVAC`     | `<description placeholder>`
[250]    | `DRA:TP21:STATON`     | `<description placeholder>`
[251]    | `DRA:TP21:RDCUR`     | `<description placeholder>`
[252]    | `DRA:CG21C:RDVAC`     | `<description placeholder>`
[253]    | `DRA:BV21:STATON`     | `<description placeholder>`
[254]    | `DRA:VV31A:STATON`     | `<description placeholder>`
[255]    | `DRA:BV31:STATON`     | `<description placeholder>`
[256]    | `DRA:CG31C:RDVAC`     | `<description placeholder>`
[257]    | `DRA:TP31:STATON`     | `<description placeholder>`
[258]    | `DRA:TP31:RDCUR`     | `<description placeholder>`
[259]    | `DRA:GV31:STATON`     | `<description placeholder>`
[260]    | `DRA:CG31:RDVAC`     | `<description placeholder>`
[261]    | `DRA:IG31:RDVAC`     | `<description placeholder>`
[262]    | `DRA:IP31:RDVAC`     | `<description placeholder>`
[263]    | `DRA:IP31:STATON`     | `<description placeholder>`
[264]    | `DRA:CP33:STATON`     | `<description placeholder>`
[265]    | `DRA:CP33:SCALETEMP`     | `<description placeholder>`
[266]    | `DRA:CG33C:RDVAC`     | `<description placeholder>`
[267]    | `DRA:GV33:STATON`     | `<description placeholder>`
[268]    | `DRA:IV31:STATON`     | `<description placeholder>`
[269]    | `DRA:IG32:RDVAC`     | `<description placeholder>`
[270]    | `DRA:VV32:STATON`     | `<description placeholder>`
[271]    | `DRA:CG32:RDVAC`     | `<description placeholder>`
[272]    | `DRA:TP32:STATON`     | `<description placeholder>`
[273]    | `DRA:TP32:RDCUR`     | `<description placeholder>`
[274]    | `DRA:CG32C:RDVAC`     | `<description placeholder>`
[275]    | `DRA:BV32:STATON`     | `<description placeholder>`
[276]    | `DRA:IV41:STATON`     | `<description placeholder>`
[277]    | `DRA:CG51C:RDVAC`     | `<description placeholder>`
[278]    | `DRA:TP51:STATON`     | `<description placeholder>`
[279]    | `DRA:TP51:RDCUR`     | `<description placeholder>`
[280]    | `DRA:GV51:STATON`     | `<description placeholder>`
[281]    | `DRA:VV51:STATON`     | `<description placeholder>`
[282]    | `DRA:IP51:STATON`     | `<description placeholder>`
[283]    | `DRA:IP51:RDVAC`     | `<description placeholder>`
[284]    | `DRA:IG51:RDVAC`     | `<description placeholder>`
[285]    | `DRA:GV53:STATON`     | `<description placeholder>`
[286]    | `DRA:CP53:STATON`     | `<description placeholder>`
[287]    | `DRA:CP53:SCALETEMP`     | `<description placeholder>`
[288]    | `DRA:CG53C:RDVAC`     | `<description placeholder>`
[289]    | `DRA:RV53:STATON`     | `<description placeholder>`
[290]    | `DRA:IV51:STATON`     | `<description placeholder>`
[291]    | `DRA:IG52:RDVAC`     | `<description placeholder>`
[292]    | `DRA:CG52:RDVAC`     | `<description placeholder>`
[293]    | `DRA:VV52:STATON`     | `<description placeholder>`
[294]    | `DRA:TP52:STATON`     | `<description placeholder>`
[295]    | `DRA:TP52:RDCUR`     | `<description placeholder>`
[296]    | `DRA:CG52C:RDVAC`     | `<description placeholder>`
[297]    | `DRA:BV52:STATON`     | `<description placeholder>`
[298]    | `DRA:IV61:STATON`     | `<description placeholder>`
[299]    | `ISACPLC7:STATSSDENABLE`     | `<description placeholder>`
[300]    | `DRA:BV61:STATON`     | `<description placeholder>`
[301]    | `DRA:CG61C:RDVAC`     | `<description placeholder>`
[302]    | `DRA:TP61:STATON`     | `<description placeholder>`
[303]    | `DRA:TP61:RDCUR`     | `<description placeholder>`
[304]    | `DRA:CG61:RDVAC`     | `<description placeholder>`
[305]    | `DRA:VV61:STATON`     | `<description placeholder>`
[306]    | `ISACPLC7:STATICENABLE`     | `<description placeholder>`
[307]    | `DRA:INTICRGH:DRVON`     | `<description placeholder>`
[308]    | `DRA:INTICFILL:DRVON`     | `<description placeholder>`
[309]    | `DRA:INTICRUN:DRVON`     | `<description placeholder>`
[310]    | `DRA:INTICVNT:DRVON`     | `<description placeholder>`
[311]    | `DRA:RV61:STATON`     | `<description placeholder>`
[312]    | `DRA:RV62:STATON`     | `<description placeholder>`
[313]    | `DRA:RV63:STATON`     | `<description placeholder>`
[314]    | `DRA:SV72:STATON`     | `<description placeholder>`
[315]    | `DRA:FG71:RDFLOW`     | `<description placeholder>`
[316]    | `DRA:DPG61:RDPRESS`     | `<description placeholder>`
[317]    | `DRA:VV71:STATON`     | `<description placeholder>`
[318]    | `DRA:SV74:STATON`     | `<description placeholder>`
[319]    | `DRA:SV73:STATON`     | `<description placeholder>`
[320]    | `DRA:RP71:STATON`     | `<description placeholder>`
[321]    | `DRA:MP1:STATON`     | `<description placeholder>`
[322]    | `DRA:WFB1:STAT.BB`     | `<description placeholder>`
[323]    | `DRA:WFB1:STAT.BC`     | `<description placeholder>`
[324]    | `DRA:WFB1:STAT.BD`     | `<description placeholder>`
[325]    | `DRA:WFB1:STAT.BE`     | `<description placeholder>`
[326]    | `DRA:WFB1:STAT.BF`     | `<description placeholder>`
[327]    | `DRA:FAN1:STATON`     | `<description placeholder>`
[328]    | `DRA:FAN2:STATON`     | `<description placeholder>`
[329]    | `DRA:SERVA:STAT.BC`     | `<description placeholder>`
[330]    | `DRAIOC:CPU`     | `<description placeholder>`
[331]    | `DRAIOC:FD`     | `<description placeholder>`
[332]    | `DRAIOC:MEMORY`     | `<description placeholder>`
[333]    | `DRA:IGD4:STATON`     | `<description placeholder>`
[334]    | `DRA:IGD4:STATOFF`     | `<description placeholder>`
[335]    | `DRA:IGD4:STATOK`     | `<description placeholder>`
[336]    | `DRA:IGD4:STATTMO`     | `<description placeholder>`
[337]    | `DRA:IGD4:STATINT`     | `<description placeholder>`
[338]    | `DRA:IGU3:STATON`     | `<description placeholder>`
[339]    | `DRA:IGU3:STATOFF`     | `<description placeholder>`
[340]    | `DRA:IGU3:STATOK`     | `<description placeholder>`
[341]    | `DRA:IGU3:STATTMO`     | `<description placeholder>`
[342]    | `DRA:IGU3:STATINT`     | `<description placeholder>`
[343]    | `DRA:LNV1:STATDRV`     | `<description placeholder>`
[344]    | `DRA:LNV1:STATON`     | `<description placeholder>`
[345]    | `DRA:LNV1:STATOFF`     | `<description placeholder>`
[346]    | `DRA:LNV1:STATTMO`     | `<description placeholder>`
[347]    | `DRA:LNLVL1:STAT.BC`     | `<description placeholder>`
[348]    | `DRA:LNLVL1:STAT.BD`     | `<description placeholder>`
[349]    | `DRA:LNLVL1:STAT.BF`     | `<description placeholder>`


<!-- Local Variables: -->
<!-- mode: gfm -->
<!-- End: -->
