#!/usr/bin/env python

import sys
import os
import subprocess
import math
import time
import argparse
import glob

parser = argparse.ArgumentParser(description="Submit array jobs to GREX.")
parser.add_argument("--home", dest="home", action="store", required= False, default="/home/jmammei/REMOLL/analysis/rate")
parser.add_argument("-s", dest="src_dir", action="store", required=True, help="source folder where simulation data exists")
parser.add_argument("-j", dest="jsub_dir", action="store", required=False, default="", help="numerate the jsub script if submitting the same job type again")
parser.add_argument("-o", dest="out_dir", action="store", required=True, help="choose where to create the jsub, tmp, and out directories")
parser.add_argument("-g", dest="gen", action= "store", required=False, default="moller",  help="choose generator to use. Options are moller, elastic, inelastic, beam, etc.")
parser.add_argument("--time", dest="time", action= "store", required= False, default= "00:15:00", help= "provide the estimated run time. Ex: \"00:25:00\". Usually it is 10 minutes for 1000 moller events.")
parser.add_argument("-r", dest="run_range", action = "store", required=False, default="1", help="provide run range. Example: \"2-5\"")
parser.add_argument("--scale", dest="scale", action= "store", required= True, help= "provide the scale for directory purposes. Ex: \"1.225\"")


args=parser.parse_args()

full_src = args.src_dir+"/"+args.scale+"/"+args.gen+"/out"
full_jsub = args.out_dir+"/"+args.scale+"/"+args.gen+"/jsub"+args.jsub_dir
full_tmp = args.out_dir+"/"+args.scale+"/"+args.gen+"/tmp"
full_out = args.out_dir+"/"+args.scale+"/"+args.gen+"/out"


if not os.path.exists(full_jsub):
    os.system("mkdir -p "+full_jsub)
if not os.path.exists(full_tmp):
    os.system("mkdir -p "+full_tmp)
if not os.path.exists(full_out):
    os.system("mkdir -p "+full_out)

os.chdir(full_src)
list_dir=glob.glob("*_6.root")
print(list_dir)

out=os.path.realpath(full_out)

jsubf=open(full_jsub+"/"+args.gen+".sh", "w")
jsubf.write("#!/bin/bash\n")
jsubf.write("#SBATCH --account=rrg-jmammei\n")
#jsubf.write("#SBATCH --partition=compute\n")
jsubf.write("#SBATCH --job-name=remollAna\n")
jsubf.write("#SBATCH --time="+args.time+" \n")
jsubf.write("#SBATCH --nodes=1\n")
jsubf.write("#SBATCH --ntasks=1\n")
jsubf.write("#SBATCH --cpus-per-task=1\n")
jsubf.write("#SBATCH --mem=4G\n")
jsubf.write("#SBATCH --output="+full_tmp+"/"+args.gen+"_%A_%a.out\n")
jsubf.write("#SBATCH --error="+full_tmp+"/"+args.gen+"_%A_%a.err\n")
jsubf.write("source /home/garrettl/projects/rrg-jmammei/REMOLL/environment/cedar_env.sh \n")
for i in range(0, len(list_dir)):
     jsubf.write("FILE["+str(i)+"]=\""+str(list_dir[i])+"\"\n")
jsubf.write("cd $TMPDIR\n")
jsubf.write("cp "+args.home+"/* $TMPDIR\n")
jsubf.write("cp "+args.home+"/../reroot $TMPDIR\n")
jsubf.write("cp "+full_src+"/${FILE[${SLURM_ARRAY_TASK_ID}-1]} $TMPDIR\n")
jsubf.write("echo \"Current working directory is `pwd`\"\n")
jsubf.write("./reroot -q -b analyse.C\"(\\\"${FILE[${SLURM_ARRAY_TASK_ID}-1]}\\\",\\\"analysed_${FILE[${SLURM_ARRAY_TASK_ID}-1]}\\\", \\\""+args.gen+"\\\")\"\n")
jsubf.write("mv analysed_${FILE[${SLURM_ARRAY_TASK_ID}-1]} "+full_out+"\n")
jsubf.write("echo \"Program remoll finished with exit code $? at: `date`\"\n")
jsubf.close()
	        
                
subprocess.call("sbatch --array="+args.run_range+" "+full_jsub+"/"+args.gen+".sh",shell=True)
		
		

		
	
	
