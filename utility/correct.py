
import ROOT as R
import glob
import os
import sys

scales = [0.700, 0.775, 0.850, 0.925, 1.000, 1.075, 1.150, 1.225, 1.300]
#gens = ["moller", "elastic", "inelastic", "beam"]
gens = ["beam"]

for scale in scales:
    for gen in gens:        
        file_list= glob.glob("/home/garrettl/projects/rrg-jmammei/garrettl/mag_over10M/raw_data/{0:.3f}/{1}/out/*.root".format(scale, gen))


        corrupt= 0
        good= 0
        for file_name in file_list:
          f= R.TFile(file_name)
          good=good+1
          if f.IsZombie() or  f.GetSize()<1000: 
            print(f.GetName()+" is zombie \n")
            corrupt=corrupt+1
            os.system("rm "+file_name)
            good=good-1
          if f.TestBit(R.TFile.kRecovered):
            print(f.GetName()+" is corrupt but recovered\n")
            corrupt=corrupt+1
            os.system("rm "+file_name)
            good=good-1
        
        print("{0:.3f}/{1}".format(scale, gen))
        print("Corrupt: "+str(corrupt))
        print("Good: "+str(good))


