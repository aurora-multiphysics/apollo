# Apollo

Apollo is a MOOSE-based application for electromagnetic problems as part of the Aurora multiphysics package.

Apollo is still under active development and is being updated frequently.

"Fork Apollo" to create a new MOOSE-based application.

For more information see: [http://mooseframework.org/create-an-app/](http://mooseframework.org/create-an-app/)

## Electromagnetic Models
Apollo currently provides three sub-modules for solving electromagnetic problems in MOOSE in different regimes:
- ComplexMaxwell, for solving the full-wave Maxwell equations in the frequency domain using edge-based FEs.
- EddyAV, for solving the low-frequency Maxwell equations for eddy current problems in the time domain using nodal FEs under the $A-V$ formulation.
- MaxwellHNonlinear, for solving low-frequency $H$-formulation problems for systems containing highly nonlinear conductors (such as superconductors).

More information on the models used can be found at https://aurora-multiphysics.github.io/apollo/
