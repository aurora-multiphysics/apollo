---
title: ComplexAFormulation
layout: template
filename: ComplexAFormulation.md
---
# ComplexAFormulation

The ComplexAFormulation solves the time harmonic Maxwell equations for the complex magnetic vector potential \(\vec A_c\) in the frequency domain,

$$
\vec ∇× \left(ν_c \vec ∇× \vec A_c\right) + \imath ω σ_c \vec A_c - ω^2 ε_c \vec A_c = \vec J_c^\text{ext}
$$

where \(\vec A_c\) is the complex magnetic vector potential, \(\omega\) is the angular frequency of the mode under study, and \(\vec J_c^\text{ext}\) is the (complex) imposed external source current density. The material coefficients \(σ_c\), \(ν_c\), and \(ε_c\) vary spatially and represent the local complex electric conductivity, the complex magnetic reluctivity (the reciprocal of the magnetic permeability), and the complex permittivity respectively.

$$
\vec B = \text{Re}\left[\vec ∇× A_c e^{i \omega t}\right]
$$

The physical electric field \(\vec E\) and magnetic field \(\vec H\) can be obtained from the complex magnetic vector potential \(\vec A_c\) via 

$$
\vec E = \text{Re}\left[-\imath ω\vec A_c e^{i \omega t}\right]
$$ 

$$
\vec H = \text{Re}\left[ν_c \vec ∇× A_c e^{i \omega t}\right]
$$


## Weak Form
The complex \(\vec A\) formulation is solved using the weak form

$$
\langle ν_c \vec ∇ × \vec A_c, \vec ∇× \vec v \rangle_{\vec L^2(\Omega)} + \langle \imath ω σ_c \vec A_c, \vec v \rangle_{\vec L^2(\Omega)} - \langle ω^2 ε_c \vec A_c, \vec v \rangle_{\vec L^2(\Omega)} + \left\langle \left( ν_c \vec ∇ × \vec A_c \right) × \vec n, \vec v\right\rangle_{\vec L^2(\partial \Omega)} = \langle \vec J_c^\mathrm{ext}, \vec v \rangle_{\vec L^2(\Omega)}
$$

where the complex valued test function \(\vec v ∈ H(\mathrm{curl})\).
