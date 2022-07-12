[Mesh]
  type = ExclusiveMFEMMesh
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
  [./potential_low]
    type = ParsedFunction
    value = 0
  [../]
  [./j_bdr]
    type = ParsedVectorFunction
    value_x = 0.0
    value_y = 0.0
    value_z = 2*sin(2.0*pi*freq*t)
    vars = 'freq'
    vals = '0.01666667'
  [../]
  [./tangential_dEdt]
    type = ParsedVectorFunction
    value_x = 0.0
    value_y = 0.0
    value_z = 0.0
  [../]
[]

[BCs]
  [./tangential_dEdt]
    type = MFEMVectorFunctionDirichletBC
    variable = electric_field
    boundary = '1 2 3'
    function = tangential_dEdt
  [../]
  [./high_terminal]
    type = MFEMVectorNormalIntegratedBC
    variable = electric_potential
    boundary = '1'
    function = j_bdr
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
