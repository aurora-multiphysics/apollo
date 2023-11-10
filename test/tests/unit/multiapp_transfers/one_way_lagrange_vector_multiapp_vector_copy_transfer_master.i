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

  [mfem_lagrange_vector]
    family = LAGRANGE_VEC
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
  # Compare MOOSE and MFEM lagrange vectors.
  [vector_l2_difference]
    type = ElementVectorL2Difference
    variable = moose_lagrange_vector
    other_variable = mfem_lagrange_vector
  []
[]

[MultiApps]
  [sub_app]
    type = TransientMultiApp
    positions = '0 0 0'
    input_files = 'one_way_lagrange_vector_multiapp_vector_copy_transfer_mfem.i'
    execute_on = timestep_begin
  []
[]

[Transfers]
  [pull]
    type = MultiAppVectorCopyTransfer
    source_variable = mfem_lagrange_vector
    variable = mfem_lagrange_vector
    from_multi_app = sub_app
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
