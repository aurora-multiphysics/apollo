---
title: MaxwellHNonlinear
layout: template
filename: MaxwellHNonlinear.md
---
# MaxwellHNonlinear
The MaxwellHNonlinear submodule solves the time domain Maxwell equations in the low frequency limit for the magnetic field $$\vec H$$. It is particularly suitable for systems containing highly nonlinear conductors (such as superconductors). The governing equation is given by

$$
\vec \nabla \times \left(\rho \vec \nabla \times \vec H\right) +\partial_t \left(\mu \vec H \right) = 0
$$

where $$\mu$$ is the magnetic permeability of the system and $$\rho$$ is the material resistivity.

The physical electric field $$\vec E$$ and magnetic flux density $$\vec B$$ are calculated from the magnetic field $$\vec H$$ since $$\vec E = \rho \vec \nabla \times \vec H $$ and $$\vec B = \mu \vec H$$.

## Problem Set-up
Edge elements (NEDELEC_ONE) are recommended for this problem type.

Examples are provided in problems/MaxwellHNonlinear.