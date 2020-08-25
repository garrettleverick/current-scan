# Magnet Current Scan Analysis
### Author: Garrett Leverick
### Repository created: 03 July 2020
### README Last Edited: 25 July 2020
Description: This repository stores analysis code for a magnet current scan study for the spectrometer to be used in the moller experiment at Jefferson Lab.

### File Description 
rate/analyse.C: First batch of analysis. This code reads raw simulation data files, creates histograms for each septant, sector, ring combination according 
to the partitions defined by p_type and p_nrg as declared in constant.h. The program fills the histograms and saves them. These histograms store data about
the rate at which the particles hit specific parts of the spectrometer detector. This code is submitted using macro.py. Once all the data is processed the 
histograms must be combined using combine.sh or hadd. Code has capability to cut based on hits on another detector by using trid.

macro.py: This script submits batch jobs using SLURM job submission. It submits the analyse.C script so raw data files can be analysed in parallel. 

int_rata/integrate.C: Second batch of analysis. This code takes in output files from analyse.C (after they are combined with hadd). It integrates all the 
histograms in the combined output of analyse.C and outputs the result in a TGraphErrors. It can be submitted as a batch job using int_macro.py, and the jobs
are done for each generator.

collect/collect.C: Collects the 3 output files from integrate.C into one file. Also collects the sectors in rings 1,2,3,4,6,7 so they represent the actual
quartz tiles. Creates a directory based on what field map you tell it (asym, sym)

beam_secondary_e/beam_secondary_e.C: Takes output from collect.C and calculates the beam secondary electrons by subtracting the physics generator primaries
from all the beam electrons.

muli_g_add/multi_g_add.C: Takes the TGraphErrors from collect.C and plots them on the same graph. Also allows the naming of each graph (for legend purposes)
, scaling (for display purposes), and the addition of graphs using two different adding modes (detailed in the script). 

one_canvas/one_canvas.C: Takes canvases from symmetric and asymmetric cases and plots them on the same canvas. 

nosep_rate/analyse.C: Exactly like rate/analyse.C but doesn't partition the septants. A branch will be created for this code (as all other scripts must be
changed). A branch was not created, this code is obsolete now.

