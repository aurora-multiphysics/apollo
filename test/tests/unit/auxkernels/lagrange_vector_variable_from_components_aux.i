[Mesh]
  type = CoupledMFEMMesh
  file = gold/mug.e
  dim = 3
[]

[Variables]
  [moose_diffused]
    family = LAGRANGE
    order = FIRST
  []
[]

[AuxVariables]
  [test_vector]
    family = LAGRANGE_VEC
    order = FIRST
  []

  [target_vector]
    family = LAGRANGE_VEC
    order = FIRST
  []

  [target_x]
    family = LAGRANGE
    order = FIRST
  []

  [target_y]
    family = LAGRANGE
    order = FIRST
  []

  [target_z]
    family = LAGRANGE
    order = FIRST
  []
[]

[AuxKernels]
  [set_test_vector_from_target_components]
    type = VectorVariableFromComponentsAux
    variable = test_vector
    component_x = target_x
    component_y = target_y
    component_z = target_z
    execute_on = timestep_end
  []
[]

[Kernels]
  [diffusion]
    type = Diffusion
    variable = moose_diffused
  []
[]

[Functions]
  [function_target_x]
    type = ParsedFunction
    expression = '100*x'
  []

  [function_target_y]
    type = ParsedFunction
    expression = '100*y'
  []

  [function_target_z]
    type = ParsedFunction
    expression = '100*z'
  []

  [function_target]
    type = ParsedVectorFunction
    expression_x = '100*x'
    expression_y = '100*y'
    expression_z = '100*z'
  []
[]

[ICs]
  [set_target_x]
    type = FunctionIC
    variable = target_x
    function = function_target_x
  []

  [set_target_y]
    type = FunctionIC
    variable = target_y
    function = function_target_y
  []

  [set_target_z]
    type = FunctionIC
    variable = target_z
    function = function_target_z
  []

  [set_target_vector]
    type = VectorFunctionIC
    variable = target_vector
    function = function_target
  []
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
  [element_l2_difference]
    type = ElementVectorL2Difference
    variable = target_vector
    other_variable = test_vector
  []
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
