[Mesh]
  type = CoupledMFEMMesh
  file = ./gold/cylinder_fo.e
  dim = 3
[]

[Problem]
  type = MFEMProblem
[]

[Formulation]
  type = MFEMFormulation
  formulation_name = EBForm
[]

[AuxVariables]
  [./dummy_mfem]
    type = MFEMVariable
    fespace_name = HCurl
    fespace_type = ND
    order = SECOND
  [../]
  [./dummy_moose_nodal]
    family = LAGRANGE
    order = FIRST
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
    electrical_conductivity_coeff = CopperEConductivity
    electric_permittivity_coeff = CopperPermittivity
    magnetic_permeability_coeff = CopperPermeability
    block = 1
  [../]
[]

[Coefficients]
  [./CopperEConductivity]
    type = MFEMConstantCoefficient
    value = 5.96e7
  [../]
  [./CopperPermeability]
    type = MFEMConstantCoefficient
    value = 1.25663706e-6
  [../]
  [./CopperPermittivity]
    type = MFEMConstantCoefficient
    value = 0.0
  [../]
[]

[Executioner]
  type = Transient
  dt = 0.5
  start_time = 0.0
  end_time = 0.5

  l_tol = 1e-16
  l_max_its = 1000
[]

[Outputs]
  exodus=true
  [VisItDataCollection]
    type = MFEMVisItDataCollection
    file_base = OutputData/NodalCoupledMFEMVarTest
  []
[]
