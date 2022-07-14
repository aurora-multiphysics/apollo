import matplotlib.pyplot as plt
import mooseutils
from matplotlib import rc
rc('font',**{'family':'sans-serif','sans-serif':['Helvetica']})
## for Palatino and other serif fonts use:
#rc('font',**{'family':'serif','serif':['Palatino']})
rc('text', usetex=True)

# Create Figure and Axes
figure = plt.figure(facecolor='white',figsize=(12,6))
axes0 = figure.add_subplot(111)

# Read Postprocessor Data
data = mooseutils.PostprocessorReader('/home/alexander/Documents/Verification/hammerhead/bean_cylinder/3d_cyl_mag_out.csv')

x = data['H_applied']
y = data['magnetization_domain']
a = 0.5
b = 0.5
d = 0.4
V = 2*a*2*b*d

def m(rmax, jc, h):
  delta = h/(jc*rmax)
  m = -jc*rmax*(delta-delta**2 +(delta**3)/3.0 )
  return m 

RMAX = 1.0
JC = 1.0
H = x
m_virgin = m(RMAX, JC, H)
x_positive = x[x>0]
m_virgin_positive = m(RMAX, JC, x_positive)
#print(delta)
m_down =  m(RMAX, JC, 1)-2*m(RMAX, JC,(1-H)/2)
m_up =   -m(RMAX, JC, 1)+2*m(RMAX, JC,(1+H)/2)

#def m(jc, h):
#  delta = h/jc
#  m = -2.0*h*(2.0*a*b-(a+b)*delta +(2.0/3.0)*delta**2)*d/V
#  return m 

#jc = 2.0
#delta = x/2.0
#m_virgin = m(jc, x)#
#x_positive = x[x>0]
#m_virgin_positive = m(jc, x_positive)
#print(delta)
#m_down =  m(jc,  1)-2*m(jc,(1-x)/2)
#m_up =   -m(jc, 1)+2*m(jc,(1+x)/2)
#sat = m(a)
axes0.plot(x, y, label='MOOSE (Hammerhead)', linestyle='-', linewidth=1, color=[0.698, 0.875, 0.541, 1.0], marker='', markersize=1)
axes0.plot(x, m_down, linestyle='dotted', color='k', marker='')
axes0.plot(x, m_up, linestyle='dotted', color='k', marker='')
axes0.plot(x_positive, m_virgin_positive, label='Critical State Model', linestyle='dotted', color='k', marker='')

# x0-axis Settings
axes0.set_xlabel('$H_\mathrm{app}/a J_\mathrm{c}$')
axes0.set_xlim([-1.1, 1.1])

# y0-axis Settings
axes0.set_ylabel('$M_z/a J_\mathrm{c} $')
axes0.set_ylim([-0.34, 0.34])

# Show figure and write pdf
plt.legend()
plt.show()
figure.savefig("bean_hysteresis.png")
