function mpc = case118solved
%CASE118SOLVED

%% MATPOWER Case Format : Version 2
mpc.version = '2';

%%-----  Power Flow Data  -----%%
%% system MVA base
mpc.baseMVA = 100;

%% bus data
%	bus_i	type	Pd	Qd	Gs	Bs	area	Vm	Va	baseKV	zone	Vmax	Vmin
mpc.bus = [
	1	2	51	27	0	0	1	0.955	10.97274	138	1	1.06	0.94;
	2	1	20	9	0	0	1	0.971392795	11.5125474	138	1	1.06	0.94;
	3	1	39	10	0	0	1	0.967691944	11.85619	138	1	1.06	0.94;
	4	2	39	12	0	0	1	0.998	15.5740898	138	1	1.06	0.94;
	5	1	0	0	0	-40	1	1.00198464	16.0191785	138	1	1.06	0.94;
	6	2	52	22	0	0	1	0.99	13.2918718	138	1	1.06	0.94;
	7	1	19	2	0	0	1	0.989327888	12.8473378	138	1	1.06	0.94;
	8	2	28	0	0	0	1	1.015	21.0405844	345	1	1.06	0.94;
	9	1	0	0	0	0	1	1.04291821	28.2946894	345	1	1.06	0.94;
	10	2	0	0	0	0	1	1.05	35.8755986	345	1	1.06	0.94;
	11	1	70	23	0	0	1	0.985088548	13.0058004	138	1	1.06	0.94;
	12	2	47	10	0	0	1	0.99	12.488912	138	1	1.06	0.94;
	13	1	34	16	0	0	1	0.968302035	11.6297476	138	1	1.06	0.94;
	14	1	14	1	0	0	1	0.983591024	11.7714586	138	1	1.06	0.94;
	15	2	90	30	0	0	1	0.97	11.4741466	138	1	1.06	0.94;
	16	1	25	10	0	0	1	0.983897364	12.1873384	138	1	1.06	0.94;
	17	1	11	3	0	0	1	0.995088532	13.9952215	138	1	1.06	0.94;
	18	2	60	34	0	0	1	0.973	11.7807921	138	1	1.06	0.94;
	19	2	45	25	0	0	1	0.962	11.3146478	138	1	1.06	0.94;
	20	1	18	3	0	0	1	0.956934313	12.1910039	138	1	1.06	0.94;
	21	1	14	8	0	0	1	0.957724895	13.7779864	138	1	1.06	0.94;
	22	1	10	5	0	0	1	0.969019077	16.3316365	138	1	1.06	0.94;
	23	1	7	3	0	0	1	0.999469323	21.2487039	138	1	1.06	0.94;
	24	2	13	0	0	0	1	0.992	21.1138692	138	1	1.06	0.94;
	25	2	0	0	0	0	1	1.05	28.1798388	138	1	1.06	0.94;
	26	2	0	0	0	0	1	1.015	29.9601954	345	1	1.06	0.94;
	27	2	71	13	0	0	1	0.968	15.6044388	138	1	1.06	0.94;
	28	1	17	7	0	0	1	0.961568103	13.8788715	138	1	1.06	0.94;
	29	1	24	4	0	0	1	0.963216333	12.8854198	138	1	1.06	0.94;
	30	1	0	0	0	0	1	0.985332612	19.0337534	345	1	1.06	0.94;
	31	2	43	27	0	0	1	0.967	13.0018985	138	1	1.06	0.94;
	32	2	59	23	0	0	1	0.963	15.0606405	138	1	1.06	0.94;
	33	1	23	9	0	0	1	0.970934141	10.853773	138	1	1.06	0.94;
	34	2	59	26	0	14	1	0.984	11.5114163	138	1	1.06	0.94;
	35	1	33	9	0	0	1	0.98045246	11.0550502	138	1	1.06	0.94;
	36	2	31	17	0	0	1	0.98	11.0555118	138	1	1.06	0.94;
	37	1	0	0	0	-25	1	0.990661352	11.9666792	138	1	1.06	0.94;
	38	1	0	0	0	0	1	0.961285734	17.1075901	345	1	1.06	0.94;
	39	1	27	11	0	0	1	0.969961083	8.57658401	138	1	1.06	0.94;
	40	2	66	23	0	0	1	0.97	7.49550549	138	1	1.06	0.94;
	41	1	37	10	0	0	1	0.966832469	7.05155072	138	1	1.06	0.94;
	42	2	96	23	0	0	1	0.985	8.65289038	138	1	1.06	0.94;
	43	1	18	7	0	0	1	0.977121468	11.4603594	138	1	1.06	0.94;
	44	1	16	8	0	10	1	0.984436022	13.9432796	138	1	1.06	0.94;
	45	1	53	22	0	10	1	0.986382562	15.7725838	138	1	1.06	0.94;
	46	2	28	10	0	10	1	1.005	18.575733	138	1	1.06	0.94;
	47	1	34	0	0	0	1	1.01705181	20.7991252	138	1	1.06	0.94;
	48	1	20	11	0	15	1	1.02063338	20.0185031	138	1	1.06	0.94;
	49	2	87	30	0	0	1	1.025	21.0216132	138	1	1.06	0.94;
	50	1	17	4	0	0	1	1.00108276	18.982855	138	1	1.06	0.94;
	51	1	17	8	0	0	1	0.966876693	16.364183	138	1	1.06	0.94;
	52	1	18	5	0	0	1	0.956817989	15.4108513	138	1	1.06	0.94;
	53	1	23	11	0	0	1	0.9459829	14.4361487	138	1	1.06	0.94;
	54	2	113	32	0	0	1	0.955	15.3480527	138	1	1.06	0.94;
	55	2	63	22	0	0	1	0.952	15.0582168	138	1	1.06	0.94;
	56	2	84	18	0	0	1	0.954	15.2448702	138	1	1.06	0.94;
	57	1	12	3	0	0	1	0.970582529	16.4492307	138	1	1.06	0.94;
	58	1	12	3	0	0	1	0.959038672	15.5924843	138	1	1.06	0.94;
	59	2	277	113	0	0	1	0.985	19.4484652	138	1	1.06	0.94;
	60	1	78	3	0	0	1	0.993156251	23.2301203	138	1	1.06	0.94;
	61	2	0	0	0	0	1	0.995	24.1214859	138	1	1.06	0.94;
	62	2	77	14	0	0	1	0.998	23.5048339	138	1	1.06	0.94;
	63	1	0	0	0	0	1	0.968737013	22.827409	345	1	1.06	0.94;
	64	1	0	0	0	0	1	0.983738598	24.5933613	345	1	1.06	0.94;
	65	2	0	0	0	0	1	1.005	27.7191033	345	1	1.06	0.94;
	66	2	39	18	0	0	1	1.05	27.5586788	138	1	1.06	0.94;
	67	1	28	7	0	0	1	1.01968176	24.9189663	138	1	1.06	0.94;
	68	1	0	0	0	0	1	1.00324942	27.5978328	345	1	1.06	0.94;
	69	3	0	0	0	0	1	1.035	30	138	1	1.06	0.94;
	70	2	66	20	0	0	1	0.984	22.6179203	138	1	1.06	0.94;
	71	1	0	0	0	0	1	0.986844527	22.2069027	138	1	1.06	0.94;
	72	2	12	0	0	0	1	0.98	21.1085614	138	1	1.06	0.94;
	73	2	6	0	0	0	1	0.991	21.9954094	138	1	1.06	0.94;
	74	2	68	27	0	12	1	0.958	21.6685623	138	1	1.06	0.94;
	75	1	47	11	0	0	1	0.967331885	22.9302107	138	1	1.06	0.94;
	76	2	68	36	0	0	1	0.943	21.7987874	138	1	1.06	0.94;
	77	2	61	28	0	0	1	1.006	26.7506368	138	1	1.06	0.94;
	78	1	71	26	0	0	1	1.00342372	26.4466127	138	1	1.06	0.94;
	79	1	39	32	0	20	1	1.00922307	26.7454369	138	1	1.06	0.94;
	80	2	130	26	0	0	1	1.04	28.9900692	138	1	1.06	0.94;
	81	1	0	0	0	0	1	0.99680664	28.1448898	345	1	1.06	0.94;
	82	1	54	27	0	20	1	0.988545249	27.2717372	138	1	1.06	0.94;
	83	1	20	10	0	10	1	0.98437707	28.463946	138	1	1.06	0.94;
	84	1	11	7	0	0	1	0.979703861	31.000309	138	1	1.06	0.94;
	85	2	24	15	0	0	1	0.985	32.5556213	138	1	1.06	0.94;
	86	1	21	10	0	0	1	0.986690746	31.1861726	138	1	1.06	0.94;
	87	2	0	0	0	0	1	1.015	31.445385	161	1	1.06	0.94;
	88	1	48	10	0	0	1	0.987453302	35.690351	138	1	1.06	0.94;
	89	2	0	0	0	0	1	1.005	39.7483434	138	1	1.06	0.94;
	90	2	163	42	0	0	1	0.985	33.3383701	138	1	1.06	0.94;
	91	2	10	0	0	0	1	0.98	33.3506353	138	1	1.06	0.94;
	92	2	65	10	0	0	1	0.99	33.8807994	138	1	1.06	0.94;
	93	1	12	7	0	0	1	0.985433167	30.8490952	138	1	1.06	0.94;
	94	1	30	16	0	0	1	0.989830478	28.682205	138	1	1.06	0.94;
	95	1	42	31	0	0	1	0.980331873	27.7095564	138	1	1.06	0.94;
	96	1	38	15	0	0	1	0.992282652	27.5425859	138	1	1.06	0.94;
	97	1	15	9	0	0	1	1.01116617	27.915843	138	1	1.06	0.94;
	98	1	34	8	0	0	1	1.0235086	27.4333494	138	1	1.06	0.94;
	99	2	42	0	0	0	1	1.01	27.0667671	138	1	1.06	0.94;
	100	2	37	18	0	0	1	1.017	28.058842	138	1	1.06	0.94;
	101	1	22	15	0	0	1	0.991419613	29.6468823	138	1	1.06	0.94;
	102	1	5	3	0	0	1	0.989130815	32.3649861	138	1	1.06	0.94;
	103	2	23	16	0	0	1	1.01	24.3177521	138	1	1.06	0.94;
	104	2	38	25	0	0	1	0.971	21.747767	138	1	1.06	0.94;
	105	2	31	26	0	20	1	0.965	20.6435653	138	1	1.06	0.94;
	106	1	43	16	0	0	1	0.961146318	20.3834044	138	1	1.06	0.94;
	107	2	50	12	0	6	1	0.952	17.5826678	138	1	1.06	0.94;
	108	1	2	1	0	0	1	0.966211754	19.4434846	138	1	1.06	0.94;
	109	1	8	3	0	0	1	0.967025527	18.9908864	138	1	1.06	0.94;
	110	2	39	30	0	6	1	0.973	18.1440269	138	1	1.06	0.94;
	111	2	0	0	0	0	1	0.98	19.7891078	138	1	1.06	0.94;
	112	2	68	13	0	0	1	0.975	15.0448093	138	1	1.06	0.94;
	113	2	6	0	0	0	1	0.993	13.9925822	138	1	1.06	0.94;
	114	1	8	3	0	0	1	0.960093071	14.7264065	138	1	1.06	0.94;
	115	1	22	7	0	0	1	0.960022864	14.7181263	138	1	1.06	0.94;
	116	2	184	0	0	0	1	1.005	27.1628446	138	1	1.06	0.94;
	117	1	20	8	0	0	1	0.973824447	10.9479116	138	1	1.06	0.94;
	118	1	33	15	0	0	1	0.949437532	21.9418666	138	1	1.06	0.94;
];

%% generator data
%	bus	Pg	Qg	Qmax	Qmin	Vg	mBase	status	Pmax	Pmin	Pc1	Pc2	Qc1min	Qc1max	Qc2min	Qc2max	ramp_agc	ramp_10	ramp_30	ramp_q	apf
mpc.gen = [
	1	0	-3.10409708	15	-5	0.955	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	4	0	-15.0096466	300	-300	0.998	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	6	0	15.9297662	50	-13	0.99	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	8	0	63.1384439	300	-300	1.015	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	10	450	-51.0421516	200	-147	1.05	100	1	550	0	0	0	0	0	0	0	0	0	0	0	0;
	12	85	91.2917365	120	-35	0.99	100	1	185	0	0	0	0	0	0	0	0	0	0	0	0;
	15	0	7.16045443	30	-10	0.97	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	18	0	28.4263239	50	-16	0.973	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	19	0	-14.2741715	24	-8	0.962	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	24	0	-14.9076212	300	-300	0.992	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	25	220	50.0432568	140	-47	1.05	100	1	320	0	0	0	0	0	0	0	0	0	0	0	0;
	26	314	10.1247236	1000	-1000	1.015	100	1	414	0	0	0	0	0	0	0	0	0	0	0	0;
	27	0	3.98234974	300	-300	0.968	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	31	7	32.5860144	300	-300	0.967	100	1	107	0	0	0	0	0	0	0	0	0	0	0	0;
	32	0	-16.2847747	42	-14	0.963	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	34	0	-20.8271117	24	-8	0.984	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	36	0	7.72507858	24	-8	0.98	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	40	0	28.4540703	300	-300	0.97	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	42	0	41.0295754	300	-300	0.985	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	46	19	-5.02945734	100	-100	1.005	100	1	119	0	0	0	0	0	0	0	0	0	0	0	0;
	49	204	115.84513	210	-85	1.025	100	1	304	0	0	0	0	0	0	0	0	0	0	0	0;
	54	48	3.90106448	300	-300	0.955	100	1	148	0	0	0	0	0	0	0	0	0	0	0	0;
	55	0	4.66417162	23	-8	0.952	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	56	0	-2.28574889	15	-8	0.954	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	59	155	76.8339522	180	-60	0.985	100	1	255	0	0	0	0	0	0	0	0	0	0	0	0;
	61	160	-40.3940115	300	-100	0.995	100	1	260	0	0	0	0	0	0	0	0	0	0	0	0;
	62	0	1.25837284	20	-20	0.998	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	65	391	81.510159	200	-67	1.005	100	1	491	0	0	0	0	0	0	0	0	0	0	0	0;
	66	392	-1.95658664	200	-67	1.05	100	1	492	0	0	0	0	0	0	0	0	0	0	0	0;
	69	513.862872	-82.4240573	300	-300	1.035	100	1	805.2	0	0	0	0	0	0	0	0	0	0	0	0;
	70	0	9.66929217	32	-10	0.984	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	72	0	-11.1300551	100	-100	0.98	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	73	0	9.65137065	100	-100	0.991	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	74	0	-5.62520886	9	-6	0.958	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	76	0	5.26810088	23	-8	0.943	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	77	0	12.1703643	70	-20	1.006	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	80	477	105.466465	280	-165	1.04	100	1	577	0	0	0	0	0	0	0	0	0	0	0	0;
	85	0	-5.60658918	23	-8	0.985	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	87	4	11.0216074	1000	-100	1.015	100	1	104	0	0	0	0	0	0	0	0	0	0	0	0;
	89	607	-5.90495471	300	-210	1.005	100	1	707	0	0	0	0	0	0	0	0	0	0	0	0;
	90	0	59.3084075	300	-300	0.985	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	91	0	-13.0880226	100	-100	0.98	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	92	0	-13.9562482	9	-3	0.99	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	99	0	-17.5355895	100	-100	1.01	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	100	252	95.5521155	155	-50	1.017	100	1	352	0	0	0	0	0	0	0	0	0	0	0	0;
	103	40	75.4223636	40	-15	1.01	100	1	140	0	0	0	0	0	0	0	0	0	0	0	0;
	104	0	2.38825178	23	-8	0.971	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	105	0	-18.3345253	23	-8	0.965	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	107	0	6.55781883	200	-200	0.952	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	110	0	0.280833632	23	-8	0.973	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	111	36	-1.84382478	1000	-100	0.98	100	1	136	0	0	0	0	0	0	0	0	0	0	0	0;
	112	0	41.5116934	1000	-100	0.975	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	113	0	6.75262609	200	-100	0.993	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
	116	0	51.3224772	1000	-1000	1.005	100	1	100	0	0	0	0	0	0	0	0	0	0	0	0;
];

%% branch data
%	fbus	tbus	r	x	b	rateA	rateB	rateC	ratio	angle	status	angmin	angmax	Pf	Qf	Pt	Qt
mpc.branch = [
	1	2	0.0303	0.0999	0.0254	0	0	0	0	0	1	-360	360	-12.3528	-13.0412	12.4504	11.0064;
	1	3	0.0129	0.0424	0.01082	0	0	0	0	0	1	-360	360	-38.6472	-17.0629	38.8973	16.8849;
	4	5	0.00176	0.00798	0.0021	0	0	0	0	0	1	-360	360	-103.2297	-26.7919	103.4306	27.4928;
	3	5	0.0241	0.108	0.0284	0	0	0	0	0	1	-360	360	-68.1105	-14.4889	69.3490	17.2835;
	5	6	0.0119	0.054	0.01426	0	0	0	0	0	1	-360	360	88.4704	4.1064	-87.5399	-1.2987;
	6	7	0.00459	0.0208	0.0055	0	0	0	0	0	1	-360	360	35.5399	-4.7715	-35.4798	4.5052;
	8	9	0.00244	0.0305	1.162	0	0	0	0	0	1	-360	360	-440.6350	-89.7336	445.2546	24.4289;
	8	5	0	0.0267	0	0	0	0	0.985	0	1	-360	360	338.4747	124.7268	-338.4747	-92.0077;
	9	10	0.00258	0.0322	1.23	0	0	0	0	0	1	-360	360	-445.2546	-24.4289	450.0000	-51.0422;
	4	11	0.0209	0.0688	0.01748	0	0	0	0	0	1	-360	360	64.2297	-0.2177	-63.3640	1.3491;
	5	11	0.0203	0.0682	0.01738	0	0	0	0	0	1	-360	360	77.2247	2.9660	-76.0159	-0.6206;
	11	12	0.00595	0.0196	0.00502	0	0	0	0	0	1	-360	360	34.2946	-35.1367	-34.1479	35.1306;
	2	12	0.0187	0.0616	0.01572	0	0	0	0	0	1	-360	360	-32.4504	-20.0064	32.7327	19.4240;
	3	12	0.0484	0.16	0.0406	0	0	0	0	0	1	-360	360	-9.7868	-12.3960	9.8932	8.8573;
	7	12	0.00862	0.034	0.00874	0	0	0	0	0	1	-360	360	16.4798	-6.5052	-16.4527	5.7563;
	11	13	0.02225	0.0731	0.01876	0	0	0	0	0	1	-360	360	35.0852	11.4083	-34.7681	-12.1564;
	12	14	0.0215	0.0707	0.01816	0	0	0	0	0	1	-360	360	18.3144	2.6230	-18.2381	-4.1405;
	13	15	0.0744	0.2444	0.06268	0	0	0	0	0	1	-360	360	0.7681	-3.8436	-0.7670	-2.0400;
	14	15	0.0595	0.195	0.0502	0	0	0	0	0	1	-360	360	4.2381	3.1405	-4.2080	-7.8318;
	12	16	0.0212	0.0834	0.0214	0	0	0	0	0	1	-360	360	7.5078	4.3032	-7.4894	-6.3153;
	15	17	0.0132	0.0437	0.0444	0	0	0	0	0	1	-360	360	-103.8568	-24.2685	105.4390	25.2196;
	16	17	0.0454	0.1801	0.0466	0	0	0	0	0	1	-360	360	-17.5106	-3.6847	17.6554	-0.3038;
	17	18	0.0123	0.0505	0.01298	0	0	0	0	0	1	-360	360	80.2702	24.7630	-79.3896	-22.4049;
	18	19	0.01119	0.0493	0.01142	0	0	0	0	0	1	-360	360	19.3896	16.8312	-19.3095	-17.5473;
	19	20	0.0252	0.117	0.0298	0	0	0	0	0	1	-360	360	-10.6243	5.1665	10.6667	-7.7130;
	15	19	0.012	0.0394	0.0101	0	0	0	0	0	1	-360	360	11.5253	15.7192	-11.4749	-16.4963;
	20	21	0.0183	0.0849	0.0216	0	0	0	0	0	1	-360	360	-28.6667	4.7130	28.8374	-5.9006;
	21	22	0.0209	0.097	0.0246	0	0	0	0	0	1	-360	360	-42.8374	-2.0994	43.2557	1.7579;
	22	23	0.0342	0.159	0.0404	0	0	0	0	0	1	-360	360	-53.2557	-6.7579	54.2973	7.6857;
	23	24	0.0135	0.0492	0.0498	0	0	0	0	0	1	-360	360	8.2837	10.4188	-8.2519	-15.2406;
	23	25	0.0156	0.08	0.0864	0	0	0	0	0	1	-360	360	-162.5630	-26.1587	166.7645	38.6264;
	26	25	0	0.0382	0	0	0	0	0.96	0	1	-360	360	90.2891	21.5846	-90.2891	-18.6396;
	25	27	0.0318	0.163	0.1764	0	0	0	0	0	1	-360	360	143.5246	30.0565	-137.1266	-15.2502;
	27	28	0.01913	0.0855	0.0216	0	0	0	0	0	1	-360	360	32.8758	-0.5921	-32.6551	-0.4321;
	28	29	0.0237	0.0943	0.0238	0	0	0	0	0	1	-360	360	15.6551	-6.5679	-15.5846	4.6439;
	30	17	0	0.0388	0	0	0	0	0.96	0	1	-360	360	231.1868	92.9695	-231.1868	-70.1012;
	8	30	0.00431	0.0504	0.514	0	0	0	0	0	1	-360	360	74.1603	28.1452	-73.8054	-75.4235;
	26	30	0.00799	0.086	0.908	0	0	0	0	0	1	-360	360	223.7109	-11.4599	-219.7328	-36.5720;
	17	31	0.0474	0.1563	0.0399	0	0	0	0	0	1	-360	360	14.7659	11.5218	-14.5744	-14.7310;
	29	31	0.0108	0.0331	0.0083	0	0	0	0	0	1	-360	360	-8.4154	-8.6439	8.4316	7.9205;
	23	32	0.0317	0.1153	0.1173	0	0	0	0	0	1	-360	360	92.9820	5.0542	-90.2006	-6.2355;
	31	32	0.0298	0.0985	0.0251	0	0	0	0	0	1	-360	360	-29.8572	12.3966	30.2000	-13.6009;
	27	32	0.0229	0.0755	0.01926	0	0	0	0	0	1	-360	360	12.5271	1.7642	-12.4870	-3.4275;
	15	33	0.038	0.1244	0.03194	0	0	0	0	0	1	-360	360	7.3065	-4.4185	-7.2815	1.4922;
	19	34	0.0752	0.247	0.0632	0	0	0	0	0	1	-360	360	-3.5913	-10.3972	3.6471	4.5966;
	35	36	0.00224	0.0102	0.00268	0	0	0	0	0	1	-360	360	0.8388	4.0362	-0.8383	-4.2917;
	35	37	0.011	0.0497	0.01318	0	0	0	0	0	1	-360	360	-33.8388	-13.0362	33.9874	12.4275;
	33	37	0.0415	0.142	0.0366	0	0	0	0	0	1	-360	360	-15.7185	-10.4922	15.8611	7.4590;
	34	36	0.00871	0.0268	0.00568	0	0	0	0	0	1	-360	360	30.2462	4.6955	-30.1617	-4.9832;
	34	37	0.00256	0.0094	0.00984	0	0	0	0	0	1	-360	360	-94.3070	-44.1970	94.5926	44.2868;
	38	37	0	0.0375	0	0	0	0	0.935	0	1	-360	360	243.3713	113.6047	-243.3713	-88.0129;
	37	39	0.0321	0.106	0.027	0	0	0	0	0	1	-360	360	54.9090	2.9795	-53.9168	-2.2981;
	37	40	0.0593	0.168	0.042	0	0	0	0	0	1	-360	360	44.0211	-3.6750	-42.8486	2.9599;
	30	38	0.00464	0.054	0.422	0	0	0	0	0	1	-360	360	62.3514	19.0260	-62.0910	-55.9788;
	39	40	0.0184	0.0605	0.01552	0	0	0	0	0	1	-360	360	26.9168	-8.7019	-26.7627	7.7485;
	40	41	0.0145	0.0487	0.01222	0	0	0	0	0	1	-360	360	15.4475	1.1926	-15.4103	-2.2135;
	40	42	0.0555	0.183	0.0466	0	0	0	0	0	1	-360	360	-11.8362	-6.4469	11.9295	2.3017;
	41	42	0.041	0.135	0.0344	0	0	0	0	0	1	-360	360	-21.5897	-7.7865	21.8109	5.2382;
	43	44	0.0608	0.2454	0.06068	0	0	0	0	0	1	-360	360	-16.5930	-1.3302	16.7699	-3.7929;
	34	43	0.0413	0.1681	0.04226	0	0	0	0	0	1	-360	360	1.4137	1.6334	-1.4070	-5.6698;
	44	45	0.0224	0.0901	0.0224	0	0	0	0	0	1	-360	360	-32.7699	5.4840	33.0280	-6.6206;
	45	46	0.04	0.1356	0.0332	0	0	0	0	0	1	-360	360	-36.3279	-3.5669	36.8720	2.1197;
	46	47	0.038	0.127	0.0316	0	0	0	0	0	1	-360	360	-31.1111	-1.2182	31.4753	-0.7948;
	46	48	0.0601	0.189	0.0472	0	0	0	0	0	1	-360	360	-14.7609	-5.8308	14.8976	1.4187;
	47	49	0.0191	0.0625	0.01604	0	0	0	0	0	1	-360	360	-9.5352	-10.8370	9.5705	9.2803;
	42	49	0.0715	0.323	0.086	0	0	0	0	0	1	-360	360	-64.8702	5.2448	68.0367	0.3702;
	42	49	0.0715	0.323	0.086	0	0	0	0	0	1	-360	360	-64.8702	5.2448	68.0367	0.3702;
	45	49	0.0684	0.186	0.0444	0	0	0	0	0	1	-360	360	-49.7002	-2.0830	51.4367	2.3128;
	48	49	0.0179	0.0505	0.01258	0	0	0	0	0	1	-360	360	-34.8976	3.2067	35.1095	-3.9252;
	49	50	0.0267	0.0752	0.01874	0	0	0	0	0	1	-360	360	53.6600	13.4271	-52.8754	-13.1410;
	49	51	0.0486	0.137	0.0342	0	0	0	0	0	1	-360	360	66.6274	20.4428	-64.3451	-17.4044;
	51	52	0.0203	0.0588	0.01396	0	0	0	0	0	1	-360	360	28.5588	6.2457	-28.3714	-6.9943;
	52	53	0.0405	0.1635	0.04058	0	0	0	0	0	1	-360	360	10.3714	1.9943	-10.3172	-5.4490;
	53	54	0.0263	0.122	0.031	0	0	0	0	0	1	-360	360	-12.6828	-5.5510	12.7352	2.9932;
	49	54	0.073	0.289	0.0738	0	0	0	0	0	1	-360	360	37.7658	13.0700	-36.5753	-15.5989;
	49	54	0.0869	0.291	0.073	0	0	0	0	0	1	-360	360	37.7442	11.1980	-36.3789	-13.7899;
	54	55	0.0169	0.0707	0.0202	0	0	0	0	0	1	-360	360	7.0734	1.4568	-7.0631	-3.2502;
	54	56	0.00275	0.00955	0.00732	0	0	0	0	0	1	-360	360	18.5280	4.3464	-18.5170	-4.9750;
	55	56	0.00488	0.0151	0.00374	0	0	0	0	0	1	-360	360	-21.4213	-5.8239	21.4477	5.5660;
	56	57	0.0343	0.0966	0.0242	0	0	0	0	0	1	-360	360	-22.9882	-9.1036	23.2115	7.4913;
	50	57	0.0474	0.134	0.0332	0	0	0	0	0	1	-360	360	35.8754	9.1410	-35.2115	-10.4913;
	56	58	0.0343	0.0966	0.0242	0	0	0	0	0	1	-360	360	-6.6664	-3.6928	6.6857	1.5330;
	51	58	0.0255	0.0719	0.01788	0	0	0	0	0	1	-360	360	18.7863	3.1587	-18.6857	-4.5330;
	54	59	0.0503	0.2293	0.0598	0	0	0	0	0	1	-360	360	-30.3824	-7.5066	30.9041	4.2570;
	56	59	0.0825	0.251	0.0569	0	0	0	0	0	1	-360	360	-27.9630	-4.1736	28.6741	0.9874;
	56	59	0.0803	0.239	0.0536	0	0	0	0	0	1	-360	360	-29.3132	-3.9067	30.0732	1.1295;
	55	59	0.04739	0.2158	0.05646	0	0	0	0	0	1	-360	360	-34.5156	-8.2617	35.1555	5.8784;
	59	60	0.0317	0.145	0.0376	0	0	0	0	0	1	-360	360	-43.3167	3.5743	43.9393	-4.4049;
	59	61	0.0328	0.15	0.0388	0	0	0	0	0	1	-360	360	-51.7187	5.0322	52.6391	-4.6258;
	60	61	0.00264	0.0135	0.01456	0	0	0	0	0	1	-360	360	-112.0671	8.5191	112.4055	-8.2274;
	60	62	0.0123	0.0561	0.01468	0	0	0	0	0	1	-360	360	-9.8722	-7.1142	9.8894	5.7378;
	61	62	0.00824	0.0376	0.0098	0	0	0	0	0	1	-360	360	25.4927	-13.8577	-25.4238	13.1993;
	63	59	0	0.0386	0	0	0	0	0.96	0	1	-360	360	151.7715	67.4828	-151.7715	-57.0248;
	63	64	0.00172	0.02	0.216	0	0	0	0	0	1	-360	360	-151.7715	-67.4828	152.2539	52.5058;
	64	61	0	0.0268	0	0	0	0	0.985	0	1	-360	360	30.5374	13.9863	-30.5374	-13.6832;
	38	65	0.00901	0.0986	1.046	0	0	0	0	0	1	-360	360	-181.2804	-57.6258	184.4930	-8.3702;
	64	65	0.00269	0.0302	0.38	0	0	0	0	0	1	-360	360	-182.7913	-66.4921	183.7844	40.0637;
	49	66	0.018	0.0919	0.0248	0	0	0	0	0	1	-360	360	-132.2237	4.3257	135.2244	8.3249;
	49	66	0.018	0.0919	0.0248	0	0	0	0	0	1	-360	360	-132.2237	4.3257	135.2244	8.3249;
	62	66	0.0482	0.218	0.0578	0	0	0	0	0	1	-360	360	-37.1625	-17.2646	37.9310	14.6757;
	62	67	0.0258	0.117	0.031	0	0	0	0	0	1	-360	360	-24.3032	-14.4141	24.4991	12.1471;
	65	66	0	0.037	0	0	0	0	0.935	0	1	-360	360	8.5406	72.2497	-8.5406	-70.5546;
	66	67	0.0224	0.1015	0.02682	0	0	0	0	0	1	-360	360	53.1607	19.2725	-52.4991	-19.1471;
	65	68	0.00138	0.016	0.638	0	0	0	0	0	1	-360	360	14.1820	-22.4331	-14.1779	-41.8474;
	47	69	0.0844	0.2778	0.07092	0	0	0	0	0	1	-360	360	-55.9401	11.6318	58.6844	-10.0656;
	49	69	0.0985	0.324	0.0828	0	0	0	0	0	1	-360	360	-46.5401	10.6475	48.7817	-12.0587;
	68	69	0	0.037	0	0	0	0	0.935	0	1	-360	360	-125.8025	112.8196	125.8025	-103.6430;
	69	70	0.03	0.127	0.122	0	0	0	0	0	1	-360	360	108.3759	16.0747	-104.9435	-13.9847;
	24	70	0.00221	0.4115	0.10198	0	0	0	0	0	1	-360	360	-6.2153	-2.9741	6.2162	-6.8018;
	70	71	0.00882	0.0355	0.00878	0	0	0	0	0	1	-360	360	16.6529	-12.3767	-16.6146	11.6781;
	24	72	0.0488	0.196	0.0488	0	0	0	0	0	1	-360	360	1.4672	3.3071	-1.4499	-7.9824;
	71	72	0.0446	0.18	0.04444	0	0	0	0	0	1	-360	360	10.6022	-0.9397	-10.5501	-3.1477;
	71	73	0.00866	0.0454	0.01178	0	0	0	0	0	1	-360	360	6.0124	-10.7384	-6.0000	9.6514;
	70	74	0.0401	0.1323	0.03368	0	0	0	0	0	1	-360	360	16.2076	12.8926	-16.0114	-15.4216;
	70	75	0.0428	0.141	0.036	0	0	0	0	0	1	-360	360	-0.1332	9.9398	0.1936	-13.1682;
	69	75	0.0405	0.122	0.124	0	0	0	0	0	1	-360	360	110.0090	20.4854	-105.1553	-18.3077;
	74	75	0.0123	0.0406	0.01034	0	0	0	0	0	1	-360	360	-51.9886	-6.1905	52.3552	6.4423;
	76	77	0.0444	0.148	0.0368	0	0	0	0	0	1	-360	360	-61.1500	-21.0400	63.2051	24.3917;
	69	77	0.0309	0.101	0.1038	0	0	0	0	0	1	-360	360	62.2094	6.7832	-61.0491	-13.8028;
	75	77	0.0601	0.1999	0.04978	0	0	0	0	0	1	-360	360	-34.6083	-9.5541	35.4111	7.3763;
	77	78	0.00376	0.0124	0.01264	0	0	0	0	0	1	-360	360	45.3947	6.6113	-45.3162	-7.6283;
	78	79	0.00546	0.0244	0.00648	0	0	0	0	0	1	-360	360	-25.6838	-18.3717	25.7372	17.9543;
	77	80	0.017	0.0485	0.0472	0	0	0	0	0	1	-360	360	-96.5709	-37.4150	98.3435	37.5312;
	77	80	0.0294	0.105	0.0228	0	0	0	0	0	1	-360	360	-44.3653	-20.5457	45.0464	20.5912;
	79	80	0.0156	0.0704	0.0187	0	0	0	0	0	1	-360	360	-64.7372	-29.5836	65.5047	31.0833;
	68	81	0.00175	0.0202	0.808	0	0	0	0	0	1	-360	360	-44.1455	-4.6142	44.2019	-75.5392;
	81	80	0	0.037	0	0	0	0	0.935	0	1	-360	360	-44.2019	75.5392	44.2019	-73.0456;
	77	82	0.0298	0.0853	0.08174	0	0	0	0	0	1	-360	360	-3.0255	17.5545	3.1667	-25.2804;
	82	83	0.0112	0.03665	0.03796	0	0	0	0	0	1	-360	360	-47.2224	24.3935	47.5569	-26.9927;
	83	84	0.0625	0.132	0.0258	0	0	0	0	0	1	-360	360	-24.7860	14.6866	25.3461	-15.9919;
	83	85	0.043	0.148	0.0348	0	0	0	0	0	1	-360	360	-42.7709	11.9961	43.6657	-12.2904;
	84	85	0.0302	0.0641	0.01234	0	0	0	0	0	1	-360	360	-36.3461	8.9919	36.7907	-9.2391;
	85	86	0.035	0.123	0.0276	0	0	0	0	0	1	-360	360	17.1725	-7.3537	-17.0531	5.0910;
	86	87	0.02828	0.2074	0.0445	0	0	0	0	0	1	-360	360	-3.9469	-15.0910	4.0000	11.0216;
	85	88	0.02	0.102	0.0276	0	0	0	0	0	1	-360	360	-50.3924	7.5997	50.9324	-7.5305;
	85	89	0.0239	0.173	0.047	0	0	0	0	0	1	-360	360	-71.2364	0.6770	72.4886	3.7336;
	88	89	0.0139	0.0712	0.01934	0	0	0	0	0	1	-360	360	-98.9324	-2.4695	100.3280	7.6986;
	89	90	0.0518	0.188	0.0528	0	0	0	0	0	1	-360	360	58.2187	-4.7243	-56.4782	5.8132;
	89	90	0.0238	0.0997	0.106	0	0	0	0	0	1	-360	360	110.8284	-5.4421	-107.9341	7.0713;
	90	91	0.0254	0.0836	0.0214	0	0	0	0	0	1	-360	360	1.4123	4.4239	-1.4040	-6.4623;
	89	92	0.0099	0.0505	0.0548	0	0	0	0	0	1	-360	360	201.5415	-2.1040	-197.5601	16.9603;
	89	92	0.0393	0.1581	0.0414	0	0	0	0	0	1	-360	360	63.5947	-5.0668	-62.0176	7.2916;
	91	92	0.0387	0.1272	0.03268	0	0	0	0	0	1	-360	360	-8.5960	-6.6258	8.6361	3.5867;
	92	93	0.0258	0.0848	0.0218	0	0	0	0	0	1	-360	360	57.6235	-11.6583	-56.7199	12.5015;
	92	94	0.0481	0.158	0.0406	0	0	0	0	0	1	-360	360	52.1701	-15.2144	-50.7485	15.9054;
	93	94	0.0223	0.0732	0.01876	0	0	0	0	0	1	-360	360	44.7199	-19.5015	-44.1813	19.4396;
	94	95	0.0132	0.0434	0.0111	0	0	0	0	0	1	-360	360	40.8610	9.0142	-40.6237	-9.3113;
	80	96	0.0356	0.182	0.0494	0	0	0	0	0	1	-360	360	18.9665	21.0665	-18.6627	-24.6166;
	82	96	0.0162	0.053	0.0544	0	0	0	0	0	1	-360	360	-9.9443	-6.5687	9.9633	1.2944;
	94	96	0.0269	0.0869	0.023	0	0	0	0	0	1	-360	360	19.7879	-9.8217	-19.6596	7.9770;
	80	97	0.0183	0.0934	0.0254	0	0	0	0	0	1	-360	360	26.4235	25.7533	-26.1808	-27.1870;
	80	98	0.0238	0.108	0.0286	0	0	0	0	0	1	-360	360	28.9493	8.3181	-28.7435	-10.4288;
	80	99	0.0454	0.206	0.0546	0	0	0	0	0	1	-360	360	19.5642	8.1684	-19.3516	-12.9415;
	92	100	0.0648	0.295	0.0472	0	0	0	0	0	1	-360	360	31.4971	-16.5323	-30.7075	15.3729;
	94	100	0.0178	0.058	0.0604	0	0	0	0	0	1	-360	360	4.2809	-50.5376	-3.8663	45.8061;
	95	96	0.0171	0.0547	0.01474	0	0	0	0	0	1	-360	360	-1.3763	-21.6887	1.4549	20.5063;
	96	97	0.0173	0.0885	0.024	0	0	0	0	0	1	-360	360	-11.0959	-20.1611	11.1808	18.1870;
	98	100	0.0397	0.179	0.0476	0	0	0	0	0	1	-360	360	-5.2565	2.4288	5.2762	-7.2951;
	99	100	0.018	0.0813	0.0216	0	0	0	0	0	1	-360	360	-22.6484	-4.5941	22.7411	2.7939;
	100	101	0.0277	0.1262	0.0328	0	0	0	0	0	1	-360	360	-16.7426	22.8998	16.9797	-25.1278;
	92	102	0.0123	0.0559	0.01464	0	0	0	0	0	1	-360	360	44.6511	-8.3899	-44.3935	8.1270;
	101	102	0.0246	0.112	0.0294	0	0	0	0	0	1	-360	360	-38.9797	10.1278	39.3935	-11.1270;
	100	103	0.016	0.0525	0.0536	0	0	0	0	0	1	-360	360	121.7533	-22.1485	-119.4020	24.3578;
	100	104	0.0451	0.204	0.0541	0	0	0	0	0	1	-360	360	56.1847	10.6470	-54.7294	-9.4124;
	103	104	0.0466	0.1584	0.0407	0	0	0	0	0	1	-360	360	32.4521	13.8671	-31.8549	-15.8317;
	103	105	0.0535	0.1625	0.0408	0	0	0	0	0	1	-360	360	43.3511	12.8485	-42.2486	-13.4804;
	100	106	0.0605	0.229	0.062	0	0	0	0	0	1	-360	360	60.3612	9.4760	-58.1359	-7.1230;
	104	105	0.00994	0.0378	0.00986	0	0	0	0	0	1	-360	360	48.5843	2.6323	-48.3344	-2.6061;
	105	106	0.014	0.0547	0.01434	0	0	0	0	0	1	-360	360	8.8631	3.8799	-8.8482	-5.1517;
	105	107	0.053	0.183	0.0472	0	0	0	0	0	1	-360	360	26.7547	-2.3749	-26.3473	-0.5549;
	105	108	0.0261	0.0703	0.01844	0	0	0	0	0	1	-360	360	23.9651	-11.1285	-23.7746	9.9223;
	106	107	0.053	0.183	0.0472	0	0	0	0	0	1	-360	360	23.9841	-3.7253	-23.6527	0.5505;
	108	109	0.0105	0.0288	0.0076	0	0	0	0	0	1	-360	360	21.7746	-10.9223	-21.7087	10.3929;
	103	110	0.03906	0.1813	0.0461	0	0	0	0	0	1	-360	360	60.5988	8.3489	-59.1489	-6.1524;
	109	110	0.0278	0.0762	0.0202	0	0	0	0	0	1	-360	360	13.7087	-13.3929	-13.6068	11.7716;
	110	111	0.022	0.0755	0.02	0	0	0	0	0	1	-360	360	-35.7029	0.9561	36.0000	-1.8438;
	110	112	0.0247	0.064	0.062	0	0	0	0	0	1	-360	360	69.4586	-30.6142	-68.0000	28.5117;
	17	113	0.00913	0.0301	0.00768	0	0	0	0	0	1	-360	360	2.0563	5.9006	-2.0523	-6.6462;
	32	113	0.0615	0.203	0.0518	0	0	0	0	0	1	-360	360	4.1161	-17.7986	-3.9477	13.3988;
	32	114	0.0135	0.0612	0.01628	0	0	0	0	0	1	-360	360	9.3715	1.7777	-9.3577	-3.2207;
	27	115	0.0164	0.0741	0.01972	0	0	0	0	0	1	-360	360	20.7237	5.0604	-20.6423	-6.5251;
	114	115	0.0023	0.0104	0.00276	0	0	0	0	0	1	-360	360	1.3577	0.2207	-1.3577	-0.4749;
	68	116	0.00034	0.00405	0.164	0	0	0	0	0	1	-360	360	184.1259	-66.3580	-184.0000	51.3225;
	12	117	0.0329	0.14	0.0358	0	0	0	0	0	1	-360	360	20.1525	5.1973	-20.0000	-8.0000;
	75	118	0.0145	0.0481	0.01198	0	0	0	0	0	1	-360	360	40.2148	23.5877	-39.8739	-23.5571;
	76	118	0.0164	0.0544	0.01356	0	0	0	0	0	1	-360	360	-6.8500	-9.6919	6.8739	8.5571;
];

%%-----  OPF Data  -----%%
%% generator cost data
%	1	startup	shutdown	n	x1	y1	...	xn	yn
%	2	startup	shutdown	n	c(n-1)	...	c0
mpc.gencost = [
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.0222222222	20	0;
	2	0	0	3	0.117647059	20	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.0454545455	20	0;
	2	0	0	3	0.0318471338	20	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	1.42857143	20	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.526315789	20	0;
	2	0	0	3	0.0490196078	20	0;
	2	0	0	3	0.208333333	20	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.064516129	20	0;
	2	0	0	3	0.0625	20	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.0255754476	20	0;
	2	0	0	3	0.0255102041	20	0;
	2	0	0	3	0.0193648335	20	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.0209643606	20	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	2.5	20	0;
	2	0	0	3	0.0164744646	20	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.0396825397	20	0;
	2	0	0	3	0.25	20	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.277777778	20	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
	2	0	0	3	0.01	40	0;
];

%% bus names
bus_name = {
	'Riversde  V2';
	'Pokagon   V2';
	'HickryCk  V2';
	'NwCarlsl  V2';
	'Olive     V2';
	'Kankakee  V2';
	'JacksnRd  V2';
	'Olive     V1';
	'Bequine   V1';
	'Breed     V1';
	'SouthBnd  V2';
	'TwinBrch  V2';
	'Concord   V2';
	'GoshenJt  V2';
	'FtWayne   V2';
	'N. E.     V2';
	'Sorenson  V2';
	'McKinley  V2';
	'Lincoln   V2';
	'Adams     V2';
	'Jay       V2';
	'Randolph  V2';
	'CollCrnr  V2';
	'Trenton   V2';
	'TannrsCk  V2';
	'TannrsCk  V1';
	'Madison   V2';
	'Mullin    V2';
	'Grant     V2';
	'Sorenson  V1';
	'DeerCrk   V2';
	'Delaware  V2';
	'Haviland  V2';
	'Rockhill  V2';
	'WestLima  V2';
	'Sterling  V2';
	'EastLima  V2';
	'EastLima  V1';
	'NwLibrty  V2';
	'West End  V2';
	'S.Tiffin  V2';
	'Howard    V2';
	'S.Kenton  V2';
	'WMVernon  V2';
	'N.Newark  V2';
	'W.Lancst  V2';
	'Crooksvl  V2';
	'Zanesvll  V2';
	'Philo     V2';
	'WCambrdg  V2';
	'Newcmrst  V2';
	'SCoshoct  V2';
	'Wooster   V2';
	'Torrey    V2';
	'Wagenhls  V2';
	'Sunnysde  V2';
	'WNwPhil1  V2';
	'WNwPhil2  V2';
	'Tidd      V2';
	'SWKammer  V2';
	'W.Kammer  V2';
	'Natrium   V2';
	'Tidd      V1';
	'Kammer    V1';
	'Muskngum  V1';
	'Muskngum  V2';
	'Summerfl  V2';
	'Sporn     V1';
	'Sporn     V2';
	'Portsmth  V2';
	'NPortsmt  V2';
	'Hillsbro  V2';
	'Sargents  V2';
	'Bellefnt  V2';
	'SthPoint  V2';
	'Darrah    V2';
	'Turner    V2';
	'Chemical  V2';
	'CapitlHl  V2';
	'CabinCrk  V2';
	'Kanawha   V1';
	'Logan     V2';
	'Sprigg    V2';
	'BetsyLne  V2';
	'BeaverCk  V2';
	'Hazard    V2';
	'Pinevlle  V3';
	'Fremont   V2';
	'ClinchRv  V2';
	'Holston   V2';
	'HolstonT  V2';
	'Saltvlle  V2';
	'Tazewell  V2';
	'Switchbk  V2';
	'Caldwell  V2';
	'Baileysv  V2';
	'Sundial   V2';
	'Bradley   V2';
	'Hinton    V2';
	'Glen Lyn  V2';
	'Wythe     V2';
	'Smythe    V2';
	'Claytor   V2';
	'Hancock   V2';
	'Roanoke   V2';
	'Cloverdl  V2';
	'Reusens   V2';
	'Blaine    V2';
	'Franklin  V2';
	'Fieldale  V2';
	'DanRiver  V2';
	'Danville  V2';
	'Deer Crk  V2';
	'WMedford  V2';
	'Medford   V2';
	'KygerCrk  V2';
	'Corey     V2';
	'WHuntngd  V2';
};
