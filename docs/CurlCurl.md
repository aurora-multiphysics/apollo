---
title: CurlCurl
layout: template
filename: CurlCurl.md
---
# Transient Curl-Curl Formulations
The set of transient curl-curl formulations are intended to solve Maxwell's equations in low frequency limits in the time domain.
The governing equation for these formulations is given by

$$
\vec ∇× \left(α \vec ∇× \vec u\right) +\partial_t \left(β \vec u \right) = \vec q
$$

where $\vec u ∈ H(\mathrm{curl})$ and $\vec q ∈ H(\mathrm{div})$. $α$ and $β$ are material-dependent coefficients.
This is solved using the weak form

$$
\langle\alpha \vec ∇× \vec u, \vec ∇× \vec v \rangle_{\vec L^2(\Omega)} + \langle\beta \partial_t \vec u, \vec v \rangle_{\vec L^2(\Omega)} - \langle\vec q, \vec v\rangle_{\vec L^2(\Omega)} - \langle(α \vec ∇× \vec u) × \vec n, \vec v\rangle_{\vec L^2(\partial \Omega)} = 0
$$

where the test function $\vec v ∈ H(\mathrm{curl})$.

## Time Discretisation
Time discretisation is performed using a backwards Euler method, 

$$
\vec u_{n+1} = \vec u_{n} + \delta t \left(\partial_t \vec u\right)_{n+1}
$$

Substituting into the weak form evaluated at timestep $n+1$;

$$
\langle\alpha \delta t \vec ∇× \partial_t \vec u_{n+1}, \vec ∇× \vec v \rangle_{\vec L^2(\Omega)} +
\langle\beta \partial_t \vec u_{n+1}, \vec v \rangle_{\vec L^2(\Omega)} = \langle\vec q_{n+1}, \vec v\rangle_{\vec L^2(\Omega)} -\langle\alpha \vec ∇× \vec u_n, \vec ∇× \vec v \rangle_{\vec L^2(\Omega)} + \langle(α \vec ∇× \vec u_{n+1}) × \vec n, \vec v\rangle_{\vec L^2(\partial \Omega)}
$$

With suitable boundary conditions, one can therefore solve for $(\partial_t \vec u)_{n+1}$ and thus $\vec u_{n+1}$.

## Formulations
Formulations that are solved using this weak form are:
- [H Formulation](HFormulation.md) - an $\vec H$ conformal formulation, taking the trial function $\vec u$ as as the magnetic field $\vec H$ to strongly enforce $\vec ∇ \cdot \vec J = 0$ in the solution.
- [A Formulation](AFormulation.md) - a $\vec B$ conformal formulation, taking the trial function $\vec u$ as the magnetic vector potential $\vec A$ to strongly enforce $\vec ∇ \cdot \vec B = 0$ in the solution. 
- [E Formulation](EFormulation.md) - taking the trial function $\vec u$ as as the electric field $\vec E$ to strongly enforce $\vec ∇ \cdot \partial_t \vec B = 0$ in the solution.
