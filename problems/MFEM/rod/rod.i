[Mesh]
  type = MFEMMesh
  file = ./cylinder-hex-q2.gen
[]

[Problem]
  type = MFEMProblem
  formulation = Joule
  order = 2
  dt = 0.5
  end_time = 100.0
[]

[Functions]
  [./p_bc]
    type = ParsedFunction
    value = (z/abs(z))*cos(2.0*pi*freq*t)
    vars = 'freq'
    vals = '0.01666667'
  [../]
[]

[BCs]
  [./tangential_dEdt]
    type = MFEMBoundaryCondition
    boundary = '1 2 3'
  [../]
  [./thermal_flux]
    type = MFEMBoundaryCondition
    boundary = '1 2'
  []
  [./electric_potential]
    type = MFEMFunctionDirichletBC
    boundary = '1 2'
    function = p_bc
  [../]
[]

[Materials]
  [./copper]
    type = MFEMConductor
    electrical_conductivity = 62.83185
    thermal_conductivity = 0.01
    heat_capacity = 1.0
    block = 1
  [../]
  [./air]
    type = MFEMConductor
    electrical_conductivity = 62.83185e-6
    thermal_conductivity = 1e4
    heat_capacity = 1.0
    block = 2
  [../]
[]

[Executioner]
  type = Steady
[]
