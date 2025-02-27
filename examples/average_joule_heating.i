[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 10
  ny = 10
  nz = 10
  xmin = 0.0
  xmax = 1.0
  ymin = 0.0
  ymax = 1.0
  zmin = 0.0
  zmax = 1.0
[]

[Problem]
  type = MFEMProblem
[]

[Formulation]
  type = EBFormulation
  e_field_name = electric_field
  electric_conductivity_name = electrical_conductivity
[]

[FESpaces]
  [H1FESpace]
    type = MFEMFESpace
    fespace_type = H1
    order = SECOND
  []
  [HCurlFESpace]
    type = MFEMFESpace
    fespace_type = ND
    order = FIRST
  []
  [L2FESpace]
    type = MFEMFESpace
    fespace_type = L2
    order = CONSTANT
  []
[]

[Variables]
  [electric_potential]
    type = MFEMVariable
    fespace = H1FESpace
  []
[]

[AuxVariables]
  [electric_field]
    type = MFEMVariable
    fespace = HCurlFESpace
  []
  [avg_joule_heating]
    type = MFEMVariable
    fespace = L2FESpace
  []
[]

[Functions]
  [source_potential]
    type = ParsedFunction
    value = 1.0
  []
  [drain_potential]
    type = ParsedFunction
    value = 0.0
  []
[]

[BCs]
  [source_bc]
    type = MFEMScalarDirichletBC
    variable = electric_potential
    function = source_potential
    boundary = 1  # Left face (xmin)
  []
  [drain_bc]
    type = MFEMScalarDirichletBC
    variable = electric_potential
    function = drain_potential
    boundary = 2  # Right face (xmax)
  []
[]

[Materials]
  [block_material]
    type = MFEMConductor
    electrical_conductivity_coeff = BlockEConductivity
    block = 1
  []
[]

[Coefficients]
  [BlockEConductivity]
    type = MFEMConstantCoefficient
    value = 1.0
  []
[]

[AuxKernels]
  [./avg_joule_heating]
    type = TimeAverageJouleHeatingAux
    electric_field_name = electric_field
    conductivity_coef_name = BlockEConductivity
    skip = 0.2
    average = true
  [../]
[]

[Executioner]
  type = Transient
  start_time = 0.0
  end_time = 1.0
  dt = 0.1

  l_tol = 1e-10
  l_max_its = 1000
  nl_max_its = 50
  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-6
[]

[Outputs]
  [ParaViewDataCollection]
    type = MFEMParaViewDataCollection
    file_base = OutputData/AverageJouleHeating
  []
[]
