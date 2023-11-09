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

  [test_x]
    family = LAGRANGE
    order = FIRST
  []

  [test_y]
    family = LAGRANGE
    order = FIRST
  []

  [test_z]
    family = LAGRANGE
    order = FIRST
  []
[]

[Functions]
  [function_target]
    type = ParsedVectorFunction
    expression_x = '100*x'
    expression_y = '100*y'
    expression_z = '100*z'
  []
[]

[ICs]
  # 1. Setup our target vector.
  [set_target_vector]
    type = VectorFunctionIC
    variable = target_vector
    function = function_target
  []
[]

[AuxKernels]
  # 2. Set x y z target components using MOOSE AuxKernel.
  # These will be used for comparison with test_x/y/z.
  [set_target_x]
    type = VectorVariableComponentAux
    variable = target_x
    vector_variable = target_vector
    component = 'x'
    execute_on = timestep_end
  []

  [set_target_y]
    type = VectorVariableComponentAux
    variable = target_y
    vector_variable = target_vector
    component = 'y'
    execute_on = timestep_end
  []

  [set_target_z]
    type = VectorVariableComponentAux
    variable = target_z
    vector_variable = target_vector
    component = 'z'
    execute_on = timestep_end
  []

  # 3. Set x y z test components in one step using custom AuxKernel.
  [set_components_from_target_vector]
    type = VectorVariableToComponentsAux
    variable = target_vector
    component_x = test_x
    component_y = test_y
    component_z = test_z
    execute_on = timestep_end
  []
[]

[Kernels]
  [diffusion]
    type = Diffusion
    variable = moose_diffused
  []
[]

[Postprocessors]
  # 4. Compare test and target components. Should match!
  [compare_x]
    type = ElementL2Difference
    variable = test_x
    other_variable = target_x
  []

  [compare_y]
    type = ElementL2Difference
    variable = test_y
    other_variable = target_y
  []

  [compare_z]
    type = ElementL2Difference
    variable = test_z
    other_variable = target_z
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
