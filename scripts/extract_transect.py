"""
Script to extract field values along a transect in the TEAM Problem 7 benchmark.
Before running this script, ensure that the get-values MFEM miniapp is built,
which can be done by running

cd /opt/mfem/build/miniapps/tools/ && make

in the container provided.
"""
import numpy as np
import subprocess
from time import sleep

nprocs = 3 # Number of processors 
npoints = 300 # Number of points to sample along transect.
cycle = 1 #Cycle (timestep) to extract transect for. Should correspond to a timestep at the desired phase of current source.

visit_data_folder = "/home/results/OutputData/TEAM7VisIt" # Path to folder containing VisIt DataCollection
get_values_path = "/opt/mfem/build/miniapps/tools/get-values" # Path to the built get-values MFEM miniapp 
sample_point_file = "sample_points.txt" # Name of text file to save list of x,y,z coordinates of sample points to
transect_data_file = "transect.txt" # Name of text file to save field data along transect, as output of get-values app
fieldname = "magnetic_flux_density" # Name of field to extract along transect.

# Create array of sample points along transect A1-B1. Units in m.
spatial_dim = 3
xvals = np.linspace(0.00, 0.30, num=npoints, endpoint=True) # 0 mm < x < 300 mm 
yvals = 0.072*np.ones(npoints) # y = 72 mm
zvals = 0.034*np.ones(npoints) # z = 34 mm
sample_points = np.column_stack((xvals, yvals, zvals))
#Save sample points
np.savetxt(sample_point_file, sample_points, fmt='%.12f', header= '%d %d' % (npoints, spatial_dim), comments='') 

# Prepare call to get-values miniapp
call_getvalues = "mpirun -np {nprocs} {get_values_path} -r {visit_data_folder} -c {cycle} -pf {sample_point_file} -fn {fieldname} -o {transect_data_file}".format(
    nprocs=nprocs,
    get_values_path=get_values_path, 
    visit_data_folder=visit_data_folder,
    cycle=cycle, 
    sample_point_file=sample_point_file,
    fieldname=fieldname,
    transect_data_file=transect_data_file)

#Call get-values miniapp
subprocess.run(call_getvalues, shell=True, check=True)
