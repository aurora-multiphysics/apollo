[Mesh]
  type = CoupledMFEMMesh
  file = mug.e
  dim = 3
[]

[Problem]
  type = MFEMProblem
  formulation = Custom
  order = 2
  use_glvis=true
[]

[AuxVariables]
  [./diffused]
    type = MFEMVariable
    fespace = H1
    order = SECOND
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
    variable = diffused
    boundary = '1'
    function = value_bottom
  [../]
  [./low_terminal]
    type = MFEMFunctionDirichletBC
    variable = diffused
    boundary = '2'
    function = value_top
  [../]
[]

[UserObjects]
  [./one]
    type = MFEMConstantCoefficient
    value = 1.0
  [../]
[]

[Kernels]
  [diff]
    type = MFEMDiffusionKernel
    variable = diffused
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
