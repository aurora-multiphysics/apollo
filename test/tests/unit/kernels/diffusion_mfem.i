[Mesh]
  type = CoupledMFEMMesh
  file = gold/mug.e
  dim = 3
[]

[Problem]
  type = MFEMProblem
  use_glvis=true
[]

[Formulation]
  type = CustomFormulation
  e_field_name = electric_field
  b_field_name = magnetic_flux_density
  magnetic_permeability_name = magnetic_permeability
  electric_conductivity_name = electrical_conductivity
[]

[AuxVariables]
  [./mfem_diffused]
    family = LAGRANGE
    order = FIRST
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
