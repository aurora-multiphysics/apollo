[Mesh]
  type = CoupledMFEMMesh
  file = ./cylinder-hex-q2.gen
[]

[Problem]
  type = MFEMProblem
  formulation = EForm
  order = 2
  dt = 0.5
  end_time = 2.5
[]

[Functions]
  [./potential_high]
    type = ParsedFunction
    value = (z/abs(z))*cos(2.0*pi*freq*t)
    vars = 'freq'
    vals = '0.01666667'
  [../]
  [./potential_ground]
    type = ParsedFunction
    value = 0.0
  [../]
[]

[BCs]
  [./tangential_dEdt]
    type = MFEMVectorFunctionDirichletBC
    variable = electric_field
    boundary = '1 2 3'
  [../]
  [./high_terminal]
    type = MFEMFunctionDirichletBC
    variable = electric_potential
    boundary = '1'
    function = potential_high
  [../]
  [./ground_terminal]
    type = MFEMFunctionDirichletBC
    variable = electric_potential
    boundary = '2'
    function = potential_ground
  [../]
[]

[Materials]
  [./copper]
    type = MFEMConductor
    electrical_conductivity = 62.83185
    block = 1
  [../]
  [./air]
    type = MFEMConductor
    electrical_conductivity = 62.83185e-6
    block = 2
  [../]
[]

[Executioner]
  type = Steady
[]

[Outputs]
  [VisItDataCollection]
    type = MFEMVisItDataCollection
    file_base = OutputData/EFormRodVisIt
  []
  [ParaViewDataCollection]
    type = MFEMParaViewDataCollection
    file_base = OutputData/EFormRodParaView
    refinements = 1
    high_order_output = true
  []
[]
