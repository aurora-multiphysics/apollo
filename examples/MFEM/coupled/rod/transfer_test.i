[Mesh]
  type = CoupledMFEMMesh
  file = ./cylinder-hex-q2.e
  dim = 3
[]

[Problem]
  type = MFEMProblem
  formulation = EBForm
  order = 2
[]

[Executioner]
  type = Transient
  dt = 0.5
  start_time = 0.0
  end_time = 2.5
[]

[AuxVariables]
  [./dummy_mfem]
    type = MFEMVariable
    fespace = ND
    order = SECOND
  [../]
  [./dummy_moose]
    family = LAGRANGE
    order = SECOND
  [../]
[../]

[ICs]
  [./dummy_moose_ic]
    type = FunctionIC
    variable = dummy_moose
    function = (y^2)*(2-x^2)
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
  [./copper]
    type = MFEMConductor
    electrical_conductivity = 5.96e7
    magnetic_permeability = 1.25663706e-6
    block = 1
  [../]
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

