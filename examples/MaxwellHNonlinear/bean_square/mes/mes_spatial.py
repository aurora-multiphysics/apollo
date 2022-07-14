#!/usr/bin/env python3
#* This file is part of the MOOSE framework
#* https://www.mooseframework.org
#*
#* All rights reserved, see COPYRIGHT for full restrictions
#* https://github.com/idaholab/moose/blob/master/COPYRIGHT
#*
#* Licensed under LGPL 2.1, please see LICENSE for details
#* https://www.gnu.org/licenses/lgpl-2.1.html

import mms
df1 = mms.run_spatial('3d_mes.i', 3, executable='/home/alexander/Development/moose_projects/hammerhead/hammerhead-opt', file_base = '3d_strip_out_{}')

fig = mms.ConvergencePlot(xlabel='Element Size ($h$)', ylabel='$L_2$ Error')
fig.plot(df1, label='1st Order', marker='o', markersize=8)
fig.save('bean_mms.png')
