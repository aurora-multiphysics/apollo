---
title: ComplexMaxwell
filename: ComplexMaxwell.md
---
# Time Harmonic Curl-Curl Formulations
The set of time harmonic curl-curl formulations in the frequency domain are intended to solve Maxwell's equations in low or high frequency regimes driven by time-harmonic sources and boundary conditions.
The governing equation for these formulations is given by the curl-curl Maxwell equation

$$
\vec ∇× \left(α \vec ∇× \vec u\right) + \imath ω β \vec u - ω^2 ζ \vec u = \vec g
$$

where $\vec u$ is the complex-valued trial function, $\omega$ is the angular frequency of the mode under study, and $\vec g$ is a (complex) imposed external source field. The material coefficients $α$, $β$, and $ζ$ are complex valued and vary spatially.

The double curl Maxwell equation in general is not positive definite, and is difficult to precondition effectively. Direct solvers are used by default, and are recommended for small system sizes.

## Weak Form
The complex $\vec E$ formulation is solved using the weak form

$$
\langle α \vec ∇ × \vec u, \vec ∇× \vec v \rangle_{\vec L^2(\Omega)} + \langle \imath ω β \vec u, \vec v \rangle_{\vec L^2(\Omega)} - \langle ω^2 ζ \vec u, \vec v \rangle_{\vec L^2(\Omega)} + \left\langle \left( α \vec ∇ × \vec u \right) × \vec n, \vec v\right\rangle_{\vec L^2(\partial \Omega)} = \langle \vec g, \vec v \rangle_{\vec L^2(\Omega)}
$$

where the complex valued test function $\vec v ∈ H(\mathrm{curl})$.

## Formulations
Formulations that are solved using this weak form are:
- [Complex E Formulation](ComplexEFormulation.md) - taking the trial function $\vec u$ as as the complex electric field $\vec E_c$.
- [Complex A Formulation](ComplexAFormulation.md) - taking the trial function $\vec u$ as the complex magnetic vector potential $\vec A_c$.