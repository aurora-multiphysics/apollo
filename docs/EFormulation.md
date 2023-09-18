---
title: EFormulation
layout: template
filename: EFormulation.md
---
# EFormulation
The $\vec E$ formulation is a $\partial_t \vec B$ conformal formulation, which strongly enforces $\vec ∇ \cdot \partial_t \vec B = 0$ in the solution. It is one of the time domain [curl-curl formulations](CurlCurl.md) available from Hephaestus.

The governing equation for this formulations is given by Ampere's law:

$$
\vec ∇× \left(ν \vec ∇× \vec E\right) +σ\partial_t \vec E = -\partial_t \vec J_\mathrm{ext}
$$

where $\vec E ∈ H(\mathrm{curl})$ and $\vec J_\mathrm{ext} ∈ H(\mathrm{div})$.

The material coefficients $σ$ and $ν$ vary spatially and represent the local electric conductivity and the magnetic reluctivity (the reciprocal of the magnetic permeability) respectively. In terms of these variables, the rate of change of the magnetic flux density $\partial_t \vec B = -\vec ∇ × \vec E$, the rate of change of the magnetic field $\partial_t \vec H = -ν \vec ∇× \vec E$, and the (total) electric current $\vec J = \vec J_\mathrm{ext} + σ \vec E$.


## Weak Form
The $\vec E$ formulation is solved using the weak form

$$
\langle ν \vec ∇ × \vec E, \vec ∇× \vec v \rangle_{\vec L^2(\Omega)} + \langle σ \partial_t \vec E, \vec v \rangle_{\vec L^2(\Omega)} + \langle \partial_t \vec J_\mathrm{ext}, \vec v\rangle_{\vec L^2(\Omega)} + \langle \partial_t \vec H × \vec n, \vec v\rangle_{\vec L^2(\partial \Omega)} = 0
$$

where the test function $v ∈ H(\mathrm{curl})$. Time discretisation is performed using a backwards Euler method, 

$$
\vec E_{n+1} = \vec E_{n} + \delta t \left(\partial_t \vec E\right)_{n+1}
$$

Substituting into the weak form evaluated at timestep $n+1$;

$$
\langle ν \delta t \vec ∇× \partial_t \vec E_{n+1}, \vec ∇× \vec v \rangle_{\vec L^2(\Omega)} +
\langle  σ \partial_t \vec E_{n+1}, \vec v \rangle_{\vec L^2(\Omega)} = -\langle \partial_t \vec J_\mathrm{ext}|_{n+1}, \vec v\rangle_{\vec L^2(\Omega)} -\langle ν \vec ∇× \vec E_n, \vec ∇× \vec v \rangle_{\vec L^2(\Omega)} - \langle \vec \partial_t  H_{n+1} × \vec n, \vec v\rangle_{\vec L^2(\partial \Omega)}
$$
