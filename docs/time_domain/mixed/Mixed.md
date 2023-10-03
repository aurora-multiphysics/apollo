---
layout: default
title: Mixed Formulations
filename: Mixed.md
nav_order: 3
has_children: true
---
# Mixed Formulations
Mixed formulations from hephaestus solve equations systems containing multiple different finite element types in the equation system solved for.

Currently mixed EM formulations available from Hephaestus are the:
 - [AV formulation]({% link time_domain/mixed/AVFormulation.md %}) - a $\vec B$ conformal formulation in the time domain, which solves for the (time derivative of the) magnetic vector potential $\vec A$ and electric scalar potential V to strongly enforce $\vec ∇ \cdot \vec B = 0$ in the solution.
  - [EB dual formulation]({% link time_domain/mixed/EBFormulation.md %}) - a dual formulation in the time domain, which solves for an $H(\mathrm{curl})$ representation of the electric field $\vec E$ and an $H(\mathrm{div})$ representation of the magnetic flux density $\vec B$. 
