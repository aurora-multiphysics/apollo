---
title: Mixed
layout: template
filename: Mixed.md
---
# Mixed Formulations
Mixed formulations from hephaestus solve equations systems containing multiple different finite element types in the equation system solved for.

Currently mixed EM formulations available from Hephaestus are the:
 - [AV formulation](AVFormulation.md) - a \(\vec B\) conformal formulation in the time domain, which solves for the (time derivative of the) magnetic vector potential \(\vec A\) and electric scalar potential V to strongly enforce \(\vec ∇ \cdot \vec B = 0\) in the solution.
  - [EB dual formulation](EBFormulation.md) - a dual formulation in the time domain, which solves for an \(H(\mathrm{curl})\) representation of the electric field \(\vec E\) and an \(H(\mathrm{div})\) representation of the magnetic flux density \(\vec B\). 
