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
  # Monomial vector on MOOSE side.
  [moose_monomial_vector]
    family = MONOMIAL_VEC
    order = CONSTANT
  []

  # Monomial vector from MFEM side.
  [mfem_monomial_vector]
    family = MONOMIAL_VEC
    order = CONSTANT
  []

  # MFEM monomial vector components.
  [mfem_monomial_vector_x]
    family = MONOMIAL
    order = CONSTANT
  []

  [mfem_monomial_vector_y]
    family = MONOMIAL
    order = CONSTANT
  []

  [mfem_monomial_vector_z]
    family = MONOMIAL
    order = CONSTANT
  []
[]

[Kernels]
  [diffusion]
    type = Diffusion
    variable = moose_diffused
  []
[]

[ICs]
  # 1. Setup the MOOSE monomial vector for comparison purposes.
  [do_something_with_monomial_vector]
    type = VectorFunctionIC
    variable = moose_monomial_vector
    function = update_monomial_vector
  []
[]

[AuxKernels]
  # 2. Rebuild received MFEM monomial vector from components after transfer finished.
  [rebuild_mfem_monomial_vector]
    type = VectorVariableFromComponentsAux
    variable = mfem_monomial_vector
    component_x = mfem_monomial_vector_x
    component_y = mfem_monomial_vector_y
    component_z = mfem_monomial_vector_z
    execute_on = timestep_end
  []
[]

[Functions]
  [update_monomial_vector]
    type = ParsedVectorFunction
    expression_x = '100 * x * x'
    expression_y = '100 * y * y'
    expression_z = '100 * z * z'
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
  # Compare MOOSE and MFEM monomial vectors.
  [vector_l2_difference]
    type = ElementVectorL2Difference
    variable = moose_monomial_vector
    other_variable = mfem_monomial_vector
  []
[]

[MultiApps]
  [sub_app]
    type = TransientMultiApp
    positions = '0 0 0'
    input_files = 'one_way_monomial_vector_transfer_mfem.i'
    execute_on = timestep_begin
  []
[]

[Transfers]
  # Pull the MFEM monomial vector from the miniapp.
  [pull]
    type = MultiAppGeneralFieldNearestLocationTransfer
    from_multi_app = sub_app
    source_type = 'variable_default variable_default variable_default'
    source_variable = 'mfem_monomial_vector_x mfem_monomial_vector_y mfem_monomial_vector_z'
    variable = 'mfem_monomial_vector_x mfem_monomial_vector_y mfem_monomial_vector_z'
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
