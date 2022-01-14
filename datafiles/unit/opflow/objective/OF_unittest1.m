% Case saved by PowerWorld Simulator, version 22, build date March 15, 2021
function mpc = OF-unittest1
mpc.version = '2';
mpc.baseMVA =  100.00;

%% bus data 
mpc.bus = [
1	1	0.00	0.00	0.00	0.00	1	1.0000000	-0.000056	138.00	1	1.100	0.900
2	1	0.00	0.00	0.00	10.00	1	1.0000000	-0.000056	138.00	1	1.100	0.900
3	3	0.00	0.00	0.00	0.00	1	1.0000000	0.000001	13.80	1	1.100	0.900
4	1	10.00	10.00	0.00	0.00	1	0.9999990	-0.000113	138.00	1	1.100	0.900
5	1	0.00	0.00	0.00	0.00	1	0.9999990	-0.000113	138.00	1	1.100	0.900
];

%% generator data 
mpc.gen = [
3	10.00	0.00	9900.00	-9900.00	1.0000	100.00	1	10.00	10.00	0.00	0.00	0.00	0.00	0.00	0.00	0	0	0	0	10.0000
];

%% generator cost data
mpc.gencost = [
2	0	0	4	0.0000	0.010	0.100	8.00
];

%% branch data
mpc.branch = [
1	2	0.000000	0.000010	0.00000	0.00	0.00	0.00	0.00000	0.000	1	0.00	0.00	0.00	0.00	0.00	0.00
2	3	0.000000	0.000010	0.00000	0.00	0.00	0.00	1.00000	0.000	1	0.00	0.00	-10.00	-0.00	10.00	0.00
2	4	0.000000	0.000010	0.00000	0.00	0.00	0.00	0.00000	0.000	1	0.00	0.00	10.00	10.00	-10.00	-10.00
4	5	0.000000	0.000010	0.00000	0.00	0.00	0.00	0.00000	0.000	1	0.00	0.00	0.00	0.00	0.00	0.00
];

%% bus names
mpc.bus_name = {
'1';
'2';
'3';
'4';
'5';
};

%% Generator Unit Types
mpc.gentype = {
'UN';
};

%% Generator Fuel Types
mpc.genfuel = {
'unknown';
};