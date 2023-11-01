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

[TransferLagrangeVec]
  [mfem_lagrange_vector_in]
    order = FIRST
  []

  [mfem_lagrange_vector_out]
    order = FIRST
  []
[]

[AuxVariables]
  [mfem_diffused]
    family = LAGRANGE
    order = FIRST
  []
[]

[AuxKernels]
  [test_aux_kernel]
    type = VectorVariableFromComponentsAux
    variable = mfem_lagrange_vector_in
    component_variables = 'mfem_lagrange_vector_in_x mfem_lagrange_vector_in_y mfem_lagrange_vector_in_z'
  []

  [set_x_out]
    type = VectorVariableComponentAux
    vector_variable = mfem_lagrange_vector_in
    variable = mfem_lagrange_vector_out_x
    component = 'x'
  []

  [set_y_out]
    type = VectorVariableComponentAux
    vector_variable = mfem_lagrange_vector_in
    variable = mfem_lagrange_vector_out_y
    component = 'y'
  []

  [set_z_out]
    type = VectorVariableComponentAux
    vector_variable = mfem_lagrange_vector_in
    variable = mfem_lagrange_vector_out_z
    component = 'z'
  []
[]

[Kernels]
  [diff]
    type = MFEMDiffusionKernel
    variable = mfem_diffused
    coefficient = one
  []
[]

[Functions]
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
    type = MFEMFunctionDirichletBC
    variable = mfem_diffused
    boundary = '1'
    function = value_bottom
  []
  [low_terminal]
    type = MFEMFunctionDirichletBC
    variable = mfem_diffused
    boundary = '2'
    function = value_top
  []
[]

[Coefficients]
  [one]
    type = MFEMConstantCoefficient
    value = 1.0
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
