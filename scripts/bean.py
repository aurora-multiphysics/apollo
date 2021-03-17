import matplotlib.pyplot as plt
import mooseutils

# Create Figure and Axes
figure = plt.figure(facecolor='white')
axes0 = figure.add_subplot(111)

# Read Postprocessor Data
data = mooseutils.PostprocessorReader('/home/alexander/Development/moose_projects/apollo/problems/3d_sc_trel_out.csv')

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
jc = 2.0
m_virgin = m(RMAX, JC, H)
x_positive = x[x>0]
m_virgin_positive = m(RMAX, JC, x_positive)
#print(delta)
m_down =  m(RMAX, JC, 1)-2*m(RMAX, JC,(1-H)/2)
m_up =   -m(RMAX, JC, 1)+2*m(RMAX, JC,(1+H)/2)
#sat = m(a)
axes0.plot(x, y, label='magnetization_domain', linestyle='-', linewidth=1, color=[0.698, 0.875, 0.541, 1.0], marker='', markersize=1)
axes0.plot(x, m_down, label='magnetization_theory', linestyle='dotted', color='k', marker='')
axes0.plot(x, m_up, label='magnetization_theory', linestyle='dotted', color='k', marker='')
axes0.plot(x_positive, m_virgin_positive, label='magnetization_theory', linestyle='dotted', color='k', marker='')

# x0-axis Settings
axes0.set_xlabel('H_applied')
axes0.set_xlim([-1.1, 1.1])

# y0-axis Settings
axes0.set_ylabel('magnetization_domain')
axes0.set_ylim([-0.34, 0.34])

# Show figure and write pdf
plt.show()
figure.savefig("output.pdf")
