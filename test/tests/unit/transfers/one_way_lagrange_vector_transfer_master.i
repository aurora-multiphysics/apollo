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
  [moose_lagrange_vector]
    family = LAGRANGE_VEC
    order = FIRST
  []
[]

[TransferLagrangeVec]
  [mfem_lagrange_vector]
    order = FIRST
  []
[]

[Kernels]
  [diffusion]
    type = Diffusion
    variable = moose_diffused
  []
[]

[ICs]
  # 1. Setup the MOOSE lagrange vector for comparison purposes.
  [do_something_with_lagrange_vector]
    type = VectorFunctionIC
    variable = moose_lagrange_vector
    function = update_lagrange_vector
  []
[]

[AuxKernels]
  # 2. Rebuild received MFEM lagrange vector from hidden components after transfer finished.
  [rebuild_mfem_lagrange_vector]
    type = VectorVariableFromComponentsAux
    variable = mfem_lagrange_vector
    component_variables = 'mfem_lagrange_vector_x mfem_lagrange_vector_y mfem_lagrange_vector_z'
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
  # Compare moose and mfem lagrange vectors.
  [vector_l2_difference]
    type = ElementVectorL2Difference
    var = moose_lagrange_vector
    other_var = mfem_lagrange_vector
  []
[]

[MultiApps]
  [sub_app]
    type = TransientMultiApp
    positions = '0 0 0'
    input_files = 'one_way_lagrange_vector_transfer_mfem.i'
    execute_on = timestep_begin
  []
[]

[Transfers]
  # Pull the MFEM lagrange vector from the miniapp.
  [pull]
    type = MultiAppGeneralFieldNearestLocationTransfer
    from_multi_app = sub_app
    source_type = 'variable_default variable_default variable_default'
    source_variable = 'mfem_lagrange_vector_x mfem_lagrange_vector_y mfem_lagrange_vector_z'
    variable = 'mfem_lagrange_vector_x mfem_lagrange_vector_y mfem_lagrange_vector_z'
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
