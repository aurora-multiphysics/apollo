---
title: AVFormulation
layout: template
filename: AVFormulation.md
---
# AVFormulation
The $\vec A-V$ formulation is a $\vec B$ conformal formulation, which strongly enforces $\vec ∇ \cdot \vec B = 0$ in the solution. It is a mixed time domain formulation available from Hephaestus, similar to the [AFormulation](AFormulation.md) but with the direct inclusion of the electric scalar potential $V$, which can improve numerical stability for domains containing large variations in resisitivity carrying persistent currents. 

The governing equations for this formulations is given by Ampere's law:
$$
\vec ∇× \left(ν \vec ∇× \vec A\right) +σ\left(\partial_t \vec A + \vec ∇ V \right) = \vec J_\mathrm{ext}
$$

and the continuity of current
$$
\vec ∇ \cdot \left[-σ\left(\partial_t \vec A + \vec ∇ V \right) \right] = 0
$$

where $\vec A ∈ H(\mathrm{curl})$, $V ∈ H^1$ and $\vec J_\mathrm{ext} ∈ H(\mathrm{div})$.

The material coefficients $σ$ and $ν$ vary spatially and represent the local electric conductivity and the magnetic reluctivity (the reciprocal of the magnetic permeability) respectively. In terms of these variables, the magnetic flux density $\vec B = \vec ∇ × \vec A$, the magnetic field $\vec H = ν \vec ∇× \vec A$, the (induced) electric field $\vec E = - \partial_t \vec A - \vec ∇ V $, the induced electric current $\vec J_\mathrm{ind} = - σ\left(\partial_t \vec A + \vec ∇ V\right)$, and the total electric current $\vec J = \vec J_\mathrm{ext} - σ\left(\partial_t \vec A + \vec ∇ V\right)$.


## Weak Form
The $\vec A-V$ formulation is solved using the weak form
$$
\langle ν \vec ∇× \vec A, \vec ∇× \vec v \rangle_{\vec L^2(\Omega)} + \left\langle σ \left(\partial_t \vec A + \vec ∇ V\right), \vec v \right\rangle_{\vec L^2(\Omega)} - \langle \vec J_\mathrm{ext}, \vec v\rangle_{\vec L^2(\Omega)} - \langle \vec H × \vec n, \vec v\rangle_{\vec L^2(\partial \Omega)} = 0
$$

$$
\left\langle σ \left(\partial_t \vec A + \vec ∇ V\right), \vec ∇ q \right\rangle_{\vec L^2(\Omega)} - \left\langle σ \left(\partial_t \vec A + \vec ∇ V\right)\cdot \vec n, q \right\rangle_{\vec L^2(\partial \Omega)} = 0
$$

where the test functions $v ∈ H(\mathrm{curl})$, $q ∈ H^1$. Time discretisation is performed using a backwards Euler method, 
$$
\vec A_{n+1} = \vec A_{n} + \delta t \left(\partial_t \vec A\right)_{n+1}
$$

Substituting into the weak form evaluated at timestep $n+1$;
$$
\langle ν \delta t \vec ∇× \partial_t \vec A_{n+1}, \vec ∇× \vec v \rangle_{\vec L^2(\Omega)} +
\langle  σ \partial_t \vec A_{n+1}, \vec v \rangle_{\vec L^2(\Omega)} + \langle  σ \vec ∇ V_{n+1}, \vec v \rangle_{\vec L^2(\Omega)} = \langle \vec J_\mathrm{ext}|_{n+1}, \vec v\rangle_{\vec L^2(\Omega)} -\langle ν \vec ∇× \vec A_n, \vec ∇× \vec v \rangle_{\vec L^2(\Omega)} + \langle \vec H_{n+1} × \vec n, \vec v\rangle_{\vec L^2(\partial \Omega)}
$$

$$
\left\langle σ \left(\partial_t \vec A_{n+1} + \vec ∇ V_{n+1}\right), \vec ∇ q \right\rangle_{\vec L^2(\Omega)} + \left\langle  J_\mathrm{ind}|_{n+1} \cdot \vec n, q \right\rangle_{\vec L^2(\partial \Omega)} = 0
$$

which can be solved simultaneously for $\partial_t \vec A_{n+1}$ and $V_{n+1}$.