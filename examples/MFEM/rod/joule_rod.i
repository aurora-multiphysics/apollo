[Mesh]
  type = ExclusiveMFEMMesh
  file = ./cylinder-hex-q2.gen
[]

[Problem]
  type = MFEMProblem
  formulation = Joule
  order = 2
  dt = 0.5
  end_time = 2.5
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
    variable = tangential_dEdt
    boundary = '1 2 3'
  [../]
  [./thermal_flux]
    type = MFEMBoundaryCondition
    variable = thermal_flux
    boundary = '1 2'
  []
  [./electric_potential]
    type = MFEMFunctionDirichletBC
    variable = electric_potential
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

[Outputs]
  [VisItDataCollection]
    type = MFEMVisItDataCollection
    file_base = OutputData/JouleRodVisIt
  []
  [ParaViewDataCollection]
    type = MFEMParaViewDataCollection
    file_base = OutputData/JouleRodParaView
    refinements = 1
    high_order_output = true
  []
[]
