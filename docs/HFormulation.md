---
title: HFormulation
layout: template
filename: HFormulation.md
---
# HFormulation
The $\vec H$ formulation is an $\vec H$ conformal formulation, which strongly enforces $\vec ∇ \cdot \vec J = 0$ in the solution. It is one of the time domain [curl-curl formulations](CurlCurl.md) available from Hephaestus.

The governing equation for this formulations is given by Faraday's law:

$$
\vec ∇× \left(ρ \vec ∇× \vec H\right) + \mu\partial_t \vec H = -\partial_t \vec B_\mathrm{ext}
$$

where $\vec H ∈ H(\mathrm{curl})$ and $\vec B_\mathrm{ext} ∈ H(\mathrm{div})$.

The material coefficients $\mu$ and $ρ$ vary spatially and represent the local magnetic permeability and the electric resistivity respectively. In terms of these variables, the electric current $\vec J = \vec ∇ × \vec H$, the electric field $\vec E = ρ \vec ∇ × \vec H$, and the (total) magnetic flux density $\vec B = \vec B_\mathrm{ext} + \mu \vec H$.


## Weak Form
The $\vec H$ formulation is solved using the weak form

$$
\langleρ \vec ∇× \vec H, \vec ∇× \vec v \rangle_{\vec L^2(\Omega)} + \langleμ \partial_t \vec H, \vec v \rangle_{\vec L^2(\Omega)} + \langle \vec \partial_t \vec B_\mathrm{ext}, \vec v\rangle_{\vec L^2(\Omega)} - \langle \vec E × \vec n, \vec v\rangle_{\vec L^2(\partial \Omega)} = 0
$$

where the test function $v ∈ H(\mathrm{curl})$. Time discretisation is performed using a backwards Euler method, 

$$
\vec H_{n+1} = \vec H_{n} + \delta t \left(\partial_t \vec H\right)_{n+1}
$$

Substituting into the weak form evaluated at timestep $n+1$;

$$
\langle ρ \delta t \vec ∇× \partial_t \vec H_{n+1}, \vec ∇× \vec v \rangle_{\vec L^2(\Omega)} +
\langle μ \partial_t \vec H_{n+1}, \vec v \rangle_{\vec L^2(\Omega)} = -\langle\vec \partial_t \vec B_\mathrm{ext}|_{n+1}, \vec v\rangle_{\vec L^2(\Omega)} -\langleρ \vec ∇× \vec H_n, \vec ∇× \vec v \rangle_{\vec L^2(\Omega)} + \langle\vec E_{n+1} × \vec n, \vec v\rangle_{\vec L^2(\partial \Omega)}
$$
