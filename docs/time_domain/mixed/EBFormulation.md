---
layout: default
title: EBFormulation
filename: EBFormulation.md
parent: Mixed Formulations
nav_order: 2
---
# EBFormulation
The governing equations for this formulation are given by Ampere's law and Faraday's law:

$$
\vec ∇× \left(ν \vec B\right) -σ\vec E = \vec J^\mathrm{ext}
$$

$$
\partial_t \vec B = -\vec ∇× \vec E
$$

where $\vec E ∈ H(\mathrm{curl})$, $\vec B ∈ H(\mathrm{div})$, and $\vec J^\mathrm{ext} ∈ H(\mathrm{div})$.

The material coefficients $σ$ and $ν$ vary spatially and represent the local electric conductivity and the magnetic reluctivity (the reciprocal of the magnetic permeability) respectively. In terms of these variables, the magnetic field $\vec H = ν \vec B$, and the (total) electric current $\vec J = \vec J^\mathrm{ext} + σ \vec E$.


## Weak Form
The $\vec E \vec B$ formulation is solved using the weak form

$$
\langle ν \vec B, \vec ∇× \vec u \rangle_{\vec L^2(\Omega)} - \langle σ \vec E, \vec u \rangle_{\vec L^2(\Omega)} - \langle \vec J^\mathrm{ext}, \vec u\rangle_{\vec L^2(\Omega)} + \langle \vec H × \vec n, \vec u\rangle_{\vec L^2(\partial \Omega)} = 0
$$

$$
\langle \partial_t \vec B, \vec v \rangle_{\vec L^2(\Omega)} + \langle \vec ∇× \vec E, \vec v \rangle_{\vec L^2(\Omega)} = 0
$$

where the test functions $\vec u ∈ H(\mathrm{curl})$ and $\vec v ∈ H(\mathrm{div})$. Time discretisation is performed using a backwards Euler method, 

$$
\vec B_{n+1} = \vec B_{n} + \delta t \partial_t \vec B_{n+1} = \vec B_{n} - \delta t \left(\vec ∇× \vec E\right)_{n+1} 
$$

Substituting into the weak form evaluated at timestep $n+1$;

$$
 \langle  ν \delta t \vec ∇× \vec E_{n+1}, \vec ∇× \vec u \rangle_{\vec L^2(\Omega)} + \langle  σ \vec E_{n+1}, \vec u \rangle_{\vec L^2(\Omega)} = \langle ν \vec B_n, \vec ∇× \vec u \rangle_{\vec L^2(\Omega)} - \langle \vec J_{n+1}^\mathrm{ext}, \vec u\rangle_{\vec L^2(\Omega)} + \langle \vec H_{n+1} × \vec n, \vec u\rangle_{\vec L^2(\partial \Omega)}
$$

$$
\langle \partial_t \vec B_{n+1}, \vec v \rangle_{\vec L^2(\Omega)} = -
\langle  \vec ∇× \vec E_{n+1}, \vec v \rangle_{\vec L^2(\Omega)}
$$

which can be solved simultaneously for $\vec E_{n+1}$ and $\partial_t \vec B_{n+1}$.
