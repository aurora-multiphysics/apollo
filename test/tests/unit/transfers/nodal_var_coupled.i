[Mesh]
  type = CoupledMFEMMesh
  file = ./gold/cylinder-hex-q2.e
  dim = 3
[]

[Problem]
  type = MFEMProblem
  formulation = EBForm
  order = 2
[]

[AuxVariables]
  [./dummy_mfem]
    type = MFEMVariable
    fespace = ND
    order = SECOND
  [../]
  [./dummy_moose_nodal]
    family = LAGRANGE
    order = SECOND
  [../]  
[../]

[ICs]
  [./dummy_moose_nodal_ic]
    type = FunctionIC
    variable = dummy_moose_nodal
    function = 2-x*x
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
  end_time = 0.5
[]

[Outputs]
  exodus=true
  [VisItDataCollection]
    type = MFEMVisItDataCollection
    file_base = OutputData/NodalCoupledMFEMVarTest
  []
[]
