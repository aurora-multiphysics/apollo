---
title: ComplexMaxwell
layout: template
filename: ComplexMaxwell.md
---
# ComplexMaxwell

The ComplexMaxwell submodule solves the full wave Maxwell equations for the complex electric field $$\vec E_c$$ in the frequency domain,

$$
\vec \nabla \times \left(\frac{1}{\mu_c} \vec \nabla \times \vec E_c\right) - \omega^2 \varepsilon_c \vec E_\text{c} = i\omega \vec J_\text{imp}
$$

where $$\omega$$ is the angular frequency of the mode under study, $$\vec J_\text{imp}$$ is the (complex) imposed current density, and $$\mu_c$$ and $$\varepsilon_c$$ are the complex permeability and permittivity.

The physical electric and magnetic fields $$\vec E$$ and $$\vec H$$ can be obtained from the complex field $$\vec E_c$$ since $$\vec E = \text{Re}\left\{\vec E_c e^{-i \omega t}\right\}$$ and $$\vec H = \text{Re}\left\{\frac{1}{i\omega\mu_\text{c}} \vec \nabla \times \vec E_c\right\}$$

## Problem Set-up
As MOOSE does not currently support complex numbers natively, separate kernels for the real and imaginary components of $$\vec E_c$$ must be constructed, along with their associated boundary conditions.

Use of the custom action defined for the ComplexMaxwell submodule is recommended to set up the basic kernels and boundary conditions. Edge elements (NEDELEC_ONE) are recommended for this problem type.

Examples are provided in problems/ComplexMaxwell.