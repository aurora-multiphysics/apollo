[Mesh]
  type = ExclusiveMFEMMesh
  file = ./cylinder-hex-q2.gen
[]

[Problem]
  type = MFEMProblem
  formulation = EBForm
  order = 2
  dt = 0.5
  end_time = 2.5
[]

[Functions]
  [./potential_high]
    type = ParsedFunction
    value = cos(2.0*pi*freq*t)
    vars = 'freq'
    vals = '0.01666667'
  [../]
  [./potential_low]
    type = ParsedFunction
    value = -cos(2.0*pi*freq*t)
    vars = 'freq'
    vals = '0.01666667'
  [../]
  [./tangential_E]
    type = ParsedVectorFunction
    value_x = 0.0
    value_y = 0.0
    value_z = 0.0
  [../]
[]

[BCs]
  [./tangential_E_bdr]
    type = MFEMVectorFunctionDirichletBC
    variable = electric_field
    boundary = '1 2 3'
    function = tangential_E
  [../]
  [./high_terminal]
    type = MFEMFunctionDirichletBC
    variable = electric_potential
    boundary = '1'
    function = potential_high
  [../]
  [./low_terminal]
    type = MFEMFunctionDirichletBC
    variable = electric_potential
    boundary = '2'
    function = potential_low
  [../]
[]

[Materials]
  [./copper]
    type = MFEMConductor
    electrical_conductivity = 62.83185
    magnetic_permeability = 1.0
    block = 1
  [../]
  [./air]
    type = MFEMConductor
    electrical_conductivity = 62.83185e-6
    magnetic_permeability = 1.0
    block = 2
  [../]
[]

[Executioner]
  type = Steady
[]

[Outputs]
  [VisItDataCollection]
    type = MFEMVisItDataCollection
    file_base = OutputData/EBFormRodVisIt
  []
  [ParaViewDataCollection]
    type = MFEMParaViewDataCollection
    file_base = OutputData/EBFormRodParaView
    refinements = 1
    high_order_output = true
  []
[]
