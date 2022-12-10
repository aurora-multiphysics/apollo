[Mesh]
  type = ExclusiveMFEMMesh
  file = ./team7_small.g
[]

[Problem]
  type = MFEMProblem
  formulation = AForm
  order = 2
[]

[UserObjects]
  [./SourceCoil]
    type = MFEMSource
  [../]
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

[Materials]
  [./air]
    type = MFEMConductor
    electrical_conductivity = 1.0
    magnetic_permeability = 12.5663706144e-7
    block = '1 3 4 5 6'
  [../]
  [./plate]
    type = MFEMConductor
    electrical_conductivity = 3.526e7
    magnetic_permeability = 12.5663706144e-7
    block = 2
  [../]
[]

[Executioner]
  type = Transient
  dt = 0.001
  start_time = 0.0
  end_time = 0.002
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
