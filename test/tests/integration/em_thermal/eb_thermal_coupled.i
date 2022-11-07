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

[AuxVariables]
  [temperature]
    family = LAGRANGE
    order = FIRST
  []
  [./joule_heating]
    family = MONOMIAL
    order = CONSTANT
    initial_condition = 50
  [../]  
[]

[Materials]
  [./copper]
    type = MFEMConductor
    electrical_conductivity = 5.96e7
    magnetic_permeability = 1.25663706e-6
    block = 1
  [../]
  [./air]
    type = MFEMConductor
    electrical_conductivity = 1
    magnetic_permeability = 1.25663706e-6
    block = 2
  [../]
[]

[Executioner]
  type = Transient
  dt = 0.5
  start_time = 0.0
  end_time = 1.0
[]

[Outputs]
  [VisItDataCollection]
    type = MFEMVisItDataCollection
    file_base = OutputData/NodalCoupledMFEMVarTest
  []
[]
