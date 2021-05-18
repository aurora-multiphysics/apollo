---
title: EddyAV
layout: template
filename: EddyAV.md
---
# EddyAV
The EddyAV submodule solves the time domain Maxwell equations in the low frequency limit for the magnetic vector potential $$\vec A$$ and the electric scalar potential $$V$$ subject to the Coulomb gauge $$\vec \nabla \cdot \vec A =0$$. The governing equations are given by

$$
\vec \nabla \times \left(\nu \vec \nabla \times \vec A\right) - \vec \nabla \left(\nu \nabla \cdot \vec A\right) +\sigma\left(\partial_t \vec A + \vec \nabla V\right) = \vec J_\text{s}
$$

$$
\vec \nabla \cdot \left[\sigma\left(-\partial_t \vec A - \vec \nabla V\right)\right] = 0
$$

where $$\vec J_\text{s}$$ is a divergence free source current density, $$\sigma$$ is the local conductivity, and $$\nu=\mu^{-1}$$ is the reluctivity, which currently must be constant across the system.

The physical electric field $$\vec E$$ and magnetic flux density $$\vec B$$ can be obtained from the potentials $$\vec A$$ and $$V$$ using the definitions $$\vec E = -\partial_t \vec A - \vec \nabla V$$ and $$\vec B = \vec \nabla \times \vec A$$

## Problem Set-up
Use of the custom action defined for the EddyAV submodule is recommended to set up the basic kernels and boundary conditions. Nodal elements (LAGRANGE_VEC for $$\vec A$$ and LAGRANGE for $$V$$) are recommended for this problem type.

Examples are provided in problems/EddyAV.