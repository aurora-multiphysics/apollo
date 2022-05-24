[Mesh]
  type = MFEMMesh
  file = ./coil.gen
[]

[Problem]
  type = MFEMProblem
  formulation = Joule
  order = 1
  dt = 0.5
  end_time = 1.0
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
    boundary = '1 2 3 4'
  [../]
  [./thermal_flux]
    type = MFEMBoundaryCondition
    boundary = '1 3'
  []
  [./electric_potential]
    type = MFEMFunctionDirichletBC
    boundary = '1 2 3'
    function = p_bc
  [../]
[]

[Materials]
  [./coil]
    type = MFEMConductor
    electrical_conductivity = 62.83185
    thermal_conductivity = 0.01
    heat_capacity = 1.0
    block = 1
  [../]
  [./core]
    type = MFEMConductor
    electrical_conductivity = 62.83185e-6
    thermal_conductivity = 1e4
    heat_capacity = 1.0
    block = 2
  [../]
  [./air]
    type = MFEMConductor
    electrical_conductivity = 62.83185e-6
    thermal_conductivity = 1e4
    heat_capacity = 1.0
    block = 3
  [../]
[]

[Executioner]
  type = Steady
[]

[Outputs]
  [datacollection]
    type = MFEMVisItDataCollection
    file_base = Joule
  []
[]