[Mesh]
  type = CoupledMFEMMesh
  file = gold/mug-tet10.e
  dim = 3
[]

[Problem]
  type = MFEMProblem
[]

[Formulation]
  type = CustomFormulation
[]

[Variables]
  [./mfem_diffused]
    family = LAGRANGE
    order = SECOND
  [../]
[]

[AuxVariables]
  [./temperature]
    family = LAGRANGE
    order = SECOND
    initial_condition = 0
  [../]
[]

[Functions]
  [./value_bottom]
    type = ParsedFunction
    value = 1.0
  [../]
  [./value_top]
    type = ParsedFunction
    value = 0.0
  [../]
[]

[BCs]
  [./bottom]
    type = MFEMFunctionDirichletBC
    variable = mfem_diffused
    boundary = '1'
    function = value_bottom
  [../]
  [./low_terminal]
    type = MFEMFunctionDirichletBC
    variable = mfem_diffused
    boundary = '2'
    function = value_top
  [../]
[]

[Coefficients]
  [./one]
    type = MFEMConstantCoefficient
    value = 1.0
  [../]
[]

[Kernels]
  [diff]
    type = MFEMDiffusionKernel
    variable = mfem_diffused
    coefficient = one
  []
[]

[Executioner]
  type = Transient
  dt = 1.0
  start_time = 0.0
  end_time = 1.0

  l_tol = 1e-16
  l_max_its = 1000
[]

[Outputs]
  [VisItDataCollection]
    type = MFEMVisItDataCollection
    file_base = OutputData/Diffusion
  []
[]
