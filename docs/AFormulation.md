---
title: AFormulation
layout: template
filename: AFormulation.md
---
# AFormulation
The $\vec A$ formulation is an $\vec B$ conformal formulation, which strongly enforces $\vec ∇ \cdot \vec B = 0$ in the solution. It is one of the time domain [curl-curl formulations](CurlCurl.md) available from Hephaestus.

The governing equation for this formulations is given by Ampere's law:

$$
\vec ∇× \left(ν \vec ∇× \vec A\right) +σ \partial_t \vec A = \vec J_\mathrm{ext}
$$

where $\vec A ∈ H(\mathrm{curl})$ and $\vec J_\mathrm{ext} ∈ H(\mathrm{div})$.

The material coefficients $σ$ and $ν$ vary spatially and represent the local electric conductivity and the magnetic reluctivity (the reciprocal of the magnetic permeability) respectively. In terms of these variables, the magnetic flux density $\vec B = \vec ∇ × \vec A$, the magnetic field $\vec H = ν \vec ∇× \vec A$, the (induced) electric field $\vec E = - \partial_t \vec A$, and the (total) electric current $\vec J = \vec J_\mathrm{ext} - σ\partial_t \vec A$.


## Weak Form
The $\vec A$ formulation is solved using the weak form

$$
\langle ν \vec ∇× \vec A, \vec ∇× \vec v \rangle_{\vec L^2(\Omega)} + \langle σ \partial_t \vec A, \vec v \rangle_{\vec L^2(\Omega)} - \langle \vec J_\mathrm{ext}, \vec v\rangle_{\vec L^2(\Omega)} - \langle \vec H × \vec n, \vec v\rangle_{\vec L^2(\partial \Omega)} = 0
$$

where the test function $v ∈ H(\mathrm{curl})$. Time discretisation is performed using a backwards Euler method, 

$$
\vec A_{n+1} = \vec A_{n} + \delta t \left(\partial_t \vec A\right)_{n+1}
$$

Substituting into the weak form evaluated at timestep $n+1$;

$$
\langle ν \delta t \vec ∇× \partial_t \vec A_{n+1}, \vec ∇× \vec v \rangle_{\vec L^2(\Omega)} +
\langle  σ \partial_t \vec A_{n+1}, \vec v \rangle_{\vec L^2(\Omega)} = \langle \vec J_\mathrm{ext}|_{n+1}, \vec v\rangle_{\vec L^2(\Omega)} -\langle ν \vec ∇× \vec A_n, \vec ∇× \vec v \rangle_{\vec L^2(\Omega)} + \langle \vec H_{n+1} × \vec n, \vec v\rangle_{\vec L^2(\partial \Omega)}
$$
