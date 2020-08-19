#!/usr/bin/env python

import sys
import os
import subprocess
import math
import time
import argparse
import glob

parser = argparse.ArgumentParser(description="Submit array jobs to GREX.")
parser.add_argument("--home", dest="home", action="store", required= False, default="/home/garrettl/projects/rrg-jmammei/garrettl/analysis/current-scan/int_rate")
parser.add_argument("-j", dest="jsub_dir", action="store", required=False, default="", help="numerate the jsub script if submitting the same job type again")
parser.add_argument("-o", dest="out_dir", action="store", required=True, help="choose where to create the jsub, tmp, and out directories")
parser.add_argument("-g", dest="gen", action="store", required=True, help="choose the generator")
parser.add_argument("--time", dest="time", action= "store", required= False, default= "00:15:00", help= "provide the estimated run time. Ex: \"00:25:00\". Usually it is 10 minutes for 1000 moller events.")


args=parser.parse_args()

full_out = args.out_dir

if not os.path.exists(full_out):
    os.system("mkdir -p "+full_out)

out=os.path.realpath(full_out)

jsubf=open(full_out+"/"+args.gen+"_int.sh", "w")
jsubf.write("#!/bin/bash\n")
jsubf.write("#SBATCH --account=rrg-jmammei\n")
#jsubf.write("#SBATCH --partition=compute\n")
jsubf.write("#SBATCH --job-name=remollInt\n")
jsubf.write("#SBATCH --time="+args.time+" \n")
jsubf.write("#SBATCH --nodes=1\n")
jsubf.write("#SBATCH --ntasks=1\n")
jsubf.write("#SBATCH --cpus-per-task=1\n")
jsubf.write("#SBATCH --mem=10G\n")
jsubf.write("#SBATCH --output="+full_out+"/int_"+args.gen+"_%A_%a.out\n")
jsubf.write("#SBATCH --error="+full_out+"/int_"+args.gen+"_%A_%a.err\n")
jsubf.write("source /home/garrettl/projects/rrg-jmammei/REMOLL/environment/cedar_env.sh \n")
jsubf.write("cd $TMPDIR\n")
jsubf.write("cp "+args.home+"/* $TMPDIR\n")
jsubf.write("echo \"Current working directory is `pwd`\"\n")
jsubf.write("root -q -b integrate.C\"(\\\""+args.gen+"\\\")\"\n")
jsubf.write("mv int_rate.root "+full_out+"/"+args.gen+".root\n")
jsubf.write("echo \"Program remoll finished with exit code $? at: `date`\"\n")
jsubf.close()
	        
                
subprocess.call("sbatch "+full_out+"/"+args.gen+"_int.sh",shell=True)
		
		

		
	
    
