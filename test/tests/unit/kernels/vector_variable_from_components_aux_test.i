[Mesh]
  type = CoupledMFEMMesh
  file = gold/mug.e
  dim = 3
[]

[Variables]
  [./moose_diffused]
    family = LAGRANGE
    order = FIRST
  [../]
[]

[AuxVariables]
  [./lagrange_vector]
    family = LAGRANGE_VEC
    order = FIRST
  [../]

  # xyz target components.
  [./x_initial]
    family = LAGRANGE
    order = FIRST
  [../]

  [./y_initial]
    family = LAGRANGE
    order = FIRST
  [../]

  [./z_initial]
    family = LAGRANGE
    order = FIRST
  [../]

  # xyz actual components from lagrange_vector.
  [./x_final]
    family = LAGRANGE
    order = FIRST
  [../]

  [./y_final]
    family = LAGRANGE
    order = FIRST
  [../]

  [./z_final]
    family = LAGRANGE
    order = FIRST
  [../]
[]

[AuxKernels]
  [./set_lagrange_vector]
    type = VectorVariableFromComponentsAux
    variable = lagrange_vector
    component_variables = 'x_initial y_initial z_initial'
  [../]

# Set lagrange vector from target 
  # Set xyz components from lagrange vector.
  [./set_x_final]
    type = VectorVariableComponentAux
    variable = x_final
    vector_variable = lagrange_vector
    component = 'x'
  [../]

  [./set_y_final]
    type = VectorVariableComponentAux
    variable = y_final
    vector_variable = lagrange_vector
    component = 'y'
  [../]

  [./set_z_final]
    type = VectorVariableComponentAux
    variable = z_final
    vector_variable = lagrange_vector
    component = 'z'
  [../]
[]

[Kernels]
  [diff]
    type = Diffusion
    variable = moose_diffused
  []
[]

[Functions]
  [function_x_ic]
    type = ParsedFunction
    expression = '100*x'
  []

  [function_y_ic]
    type = ParsedFunction
    expression = '100*y'
  []

  [function_z_ic]
    type = ParsedFunction
    expression = '100*z'
  []

  [function_lagrange_vector_ic]
    type = ParsedVectorFunction
    expression_x = '0'
    expression_y = '0'
    expression_z = '0'
  []
[]

[ICs]
  [./x_ic]
    type = FunctionIC
    variable = x_initial
    function = function_x_ic
  [../]

  [./y_ic]
    type = FunctionIC
    variable = y_initial
    function = function_y_ic
  [../]

  [./z_ic]
    type = FunctionIC
    variable = z_initial
    function = function_z_ic
  [../]

  [/.lagrange_vector_ic]
    type = VectorFunctionIC
    variable = lagrange_vector
    function = function_lagrange_vector_ic
  [../]
[]

[BCs]
  [bottom]
    type = DirichletBC
    variable = moose_diffused
    boundary = 'bottom'
    value = 1
  []

  [top]
    type = DirichletBC
    variable = moose_diffused
    boundary = 'top'
    value = 0
  []
[]

[Postprocessors]
  [./max_x_difference]
    type = ElementL2Difference
    variable = x_initial
    other_variable = x_final
  [../]

  [./max_y_difference]
    type = ElementL2Difference
    variable = y_initial
    other_variable = y_final
  [../]

  [./max_z_difference]
    type = ElementL2Difference
    variable = z_initial
    other_variable = z_final
  [../]
[]

[Executioner]
  type = Transient
  dt = 1.0
  start_time = 0.0
  end_time = 1.0
[]

[Outputs]
  csv = true
  exodus = false
[]
