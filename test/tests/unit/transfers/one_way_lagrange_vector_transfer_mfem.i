[Mesh]
  type = CoupledMFEMMesh
  file = gold/mug.e
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
  [mfem_diffused]
    family = LAGRANGE
    order = FIRST
  []

  [mfem_lagrange_vector]
    family = LAGRANGE_VEC
    order = FIRST
  []

  # Components of MFEM Lagrange vector.
  [mfem_lagrange_vector_x]
    family = LAGRANGE
    order = FIRST
  []

  [mfem_lagrange_vector_y]
    family = LAGRANGE
    order = FIRST
  []

  [mfem_lagrange_vector_z]
    family = LAGRANGE
    order = FIRST
  []
[]

[Kernels]
  [diffusion]
    type = MFEMDiffusionKernel
    variable = mfem_diffused
    coefficient = one
  []
[]

[ICs]
  # 1. Setup the MFEM lagrange vector.
  [do_something_with_lagrange_vector]
    type = VectorFunctionIC
    variable = mfem_lagrange_vector
    function = update_lagrange_vector
  []
[]

[AuxKernels]
  # 2. Prepare for transfer back to master app. Need to update x y z components.
  [set_x_out]
    type = VectorVariableComponentAux
    vector_variable = mfem_lagrange_vector
    variable = mfem_lagrange_vector_x
    component = 'x'
    execute_on = timestep_end
  []

  [set_y_out]
    type = VectorVariableComponentAux
    vector_variable = mfem_lagrange_vector
    variable = mfem_lagrange_vector_y
    component = 'y'
    execute_on = timestep_end
  []

  [set_z_out]
    type = VectorVariableComponentAux
    vector_variable = mfem_lagrange_vector
    variable = mfem_lagrange_vector_z
    component = 'z'
    execute_on = timestep_end
  []
[]

[Functions]
  [update_lagrange_vector]
    type = ParsedVectorFunction
    expression_x = '100 * x * x'
    expression_y = '100 * y * y'
    expression_z = '100 * z * z'
  []

  [value_bottom]
    type = ParsedFunction
    expression = 1.0
  []

  [value_top]
    type = ParsedFunction
    expression = 0.0
  []
[]

[BCs]
  [bottom]
    type = MFEMScalarDirichletBC
    variable = mfem_diffused
    boundary = '1'
    coefficient = BottomValue
  []
  [low_terminal]
    type = MFEMScalarDirichletBC
    variable = mfem_diffused
    boundary = '2'
    coefficient = TopValue
  []
[]

[Coefficients]
  [one]
    type = MFEMConstantCoefficient
    value = 1.0
  []
  [TopValue]
    type = MFEMFunctionCoefficient
    function = value_top
  []
  [BottomValue]
    type = MFEMFunctionCoefficient
    function = value_bottom
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
