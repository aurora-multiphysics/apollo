---
title: ComplexEFormulation
layout: default
filename: ComplexEFormulation.md
---
# ComplexEFormulation

The ComplexEFormulation solves the time harmonic Maxwell equations for the complex electric field $\vec E_c$ in the frequency domain,

$$
\vec ∇× \left(ν_c \vec ∇× \vec E_c\right) + \imath ω σ_c \vec E_c - ω^2 ε_c \vec E_c = -i ω \vec J_c^\text{ext}
$$

where $\vec E_c$ is the complex electric field, $\omega$ is the angular frequency of the mode under study, and $\vec J_c^\text{ext}$ is the (complex) imposed external source current density. The material coefficients $σ_c$, $ν_c$, and $ε_c$ vary spatially and represent the local complex electric conductivity, the complex magnetic reluctivity (the reciprocal of the magnetic permeability), and the complex permittivity respectively.

The physical electric and magnetic fields $\vec E$ and $\vec H$ can be obtained from the complex field $\vec E_c$ since 

$$
\vec E = \text{Re}\left[\vec E_c e^{i \omega t}\right]
$$ 

and 

$$
\vec H = \text{Re}\left[\frac{-ν_c}{\imath ω} \vec \nabla \times \vec E_c\right]
$$

## Weak Form
The complex $\vec E$ formulation is solved using the weak form

$$
\langle ν_c \vec ∇ × \vec E_c, \vec ∇× \vec v \rangle_{\vec L^2(\Omega)} + \langle \imath ω σ_c \vec E_c, \vec v \rangle_{\vec L^2(\Omega)} - \langle ω^2 ε_c \vec E_c, \vec v \rangle_{\vec L^2(\Omega)} + \left\langle \left( ν_c \vec ∇ × \vec E_c \right) × \vec n, \vec v\right\rangle_{\vec L^2(\partial \Omega)} = -\langle \imath ω \vec J_c^\mathrm{ext}, \vec v \rangle_{\vec L^2(\Omega)}
$$

where the complex valued test function $\vec v ∈ H(\mathrm{curl})$.
