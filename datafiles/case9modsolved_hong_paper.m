function mpc = case9modsolved
%CASE9MODSOLVED

%% MATPOWER Case Format : Version 2
mpc.version = '2';

%%-----  Power Flow Data  -----%%
%% system MVA base
mpc.baseMVA = 100;

%% bus data
%   bus_i   type    Pd  Qd  Gs  Bs  area    Vm  Va  baseKV  zone    Vmax    Vmin
mpc.bus = [
 1    3   0   0   0   0   1   1.04    0   345 1   1.1 0.9;
 2    2   0   0   0   0   1   1.025   9.27974388  345 1   1.1 0.9;
 3    2   0   0   0   0   1   1.025   4.66472921  345 1   1.1 0.9;
 4    1   0   0   0   0   1   1.02570288  -2.21668125 345 1   1.1 0.9;
 5    1   125 50  0   0   1   0.995522393 -3.98984468 345 1   1.1 0.9;
 6    1   90  30  0   0   1   1.01256119  -3.68813683 345 1   1.1 0.9;
 7    1   0   0   0   0   1   1.02563256  3.71905785  345 1   1.1 0.9;
 8    1   100 35  0   0   1   1.015759    0.726257755 345 1   1.1 0.9;
 9    1   0   0   0   0   1   1.03225648  1.96585425  345 1   1.1 0.9;
];

%% generator data
%   bus Pg  Qg  Qmax    Qmin    Vg  mBase   status  Pmax    Pmin    Pc1 Pc2 Qc1min  Qc1max  Qc2min  Qc2max  ramp_agc    ramp_10 ramp_30 ramp_q  apf
mpc.gen = [
 1    71.6786142  27.070218   300 -300    1.04    100 1   250 10  0   0   0   0   0   0   0   0   0   0   0;
 2    163         6.61201278  300 -300    1.025   100 1   300 10  0   0   0   0   0   0   0   0   0   0   0;
 3    85         -10.8402222  300 -300    1.025   100 1   270 10  0   0   0   0   0   0   0   0   0   0   0;
];

%% branch data
%   fbus    tbus    r   x   b   rateA   rateB   rateC   ratio   angle   status  angmin  angmax  Pf  Qf  Pt  Qt
mpc.branch = [
 1 4   0.0001  0.0576  0.0001  250 250 250 0   0   1   -360    360 71.6786 27.0702 -71.6732    -23.9544;
 2 7   0.0001  0.0625  0.0001  250 250 250 0   0   1   -360    360 163.0000    6.6120  -162.9747   9.2090;
 3 9   0.0001  0.0586  0.0001  300 300 250 0   0   1   -360    360 85.0000 -10.8402    -84.9930    14.9250;
 4 5   0.01    0.085   0.176   250 250 250 0   0   1   -360    360 40.9569 22.9177 -40.6991    -38.7055;
 4 6   0.017   0.092   0.158   250 250 250 0   0   1   -360    360 30.7163 1.0367  -30.5497    -16.5463;
 5 7   0.032   0.161   0.306   250 250 250 0   0   1   -360    360 -84.3009    -11.2945    86.6004 -8.3940;
 6 9   0.039   0.17    0.358   150 150 150 0   0   1   -360    360 -59.4503    -13.4537    60.8039 -18.0722;
 7 8   0.0085  0.072   0.149   250 250 250 0   0   1   -360    360 76.3743 -0.8150 -75.8990    -10.6823;
 8 9   0.0119  0.1008  0.209   150 150 150 0   0   1   -360    360 -24.1010    -24.3177    24.1892 3.1472;
];

%%-----  OPF Data  -----%%
%% area data
%   area    refbus
mpc.areas = [
 1  1;
];

%% generator cost data
%   1   startup shutdown    n   x1  y1  ... xn  yn
%   2   startup shutdown    n   c(n-1)  ... c0
mpc.gencost = [
 2    1500    0   3   0.11    5   150;
 2    2000    0   3   0.085   1.2 600;
 2    3000    0   3   0.1225  1   335;
];
