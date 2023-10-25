[Mesh]
  type = CoupledMFEMMesh
  file = gold/mug-hex27.e
  dim = 3
[]

[Problem]
  type = MFEMProblem
  use_glvis = true
[]

[Formulation]
  type = CustomFormulation
[]

[AuxVariables]
  [./mfem_diffused]
    family = LAGRANGE
    order = FIRST
  [../]

  [./test_variable]
    fespace_name = _H1FESpace
    fespace_type = H1
    family = LAGRANGE_VEC
    order = FIRST
  [../]
[]

[Functions]
  [./value_bottom]
    type = ParsedFunction
    expression = 1.0
  [../]

  [./value_top]
    type = ParsedFunction
    expression = 0.0
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
