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

  [moose_monomial_vector]
    family = MONOMIAL_VEC
    order = CONSTANT
  []

  [mfem_lagrange_vector]
    family = LAGRANGE_VEC
    order = FIRST
  []

  [mfem_monomial_vector]
    family = MONOMIAL_VEC
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
  # 1. Setup the MOOSE lagrange vectors.
  [do_something_with_lagrange_vector]
    type = VectorFunctionIC
    variable = moose_lagrange_vector
    function = update_lagrange_vector
  []

  [do_something_with_monomial_vector]
    type = VectorFunctionIC
    variable = moose_monomial_vector
    function = update_monomial_vector
  []
[]

[Functions]
  [update_lagrange_vector]
    type = ParsedVectorFunction
    expression_x = '100 * x * x'
    expression_y = '100 * y * y'
    expression_z = '100 * z * z'
  []

  [update_monomial_vector]
    type = ParsedVectorFunction
    expression_x = '1000 * x * x'
    expression_y = '1000 * y * y'
    expression_z = '1000 * z * z'
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
  [lagrange_vector_l2_difference]
    type = ElementVectorL2Difference
    variable = moose_lagrange_vector
    other_variable = mfem_lagrange_vector
  []

  [monomial_vector_l2_difference]
    type = ElementVectorL2Difference
    variable = moose_monomial_vector
    other_variable = mfem_monomial_vector
  []
[]

[MultiApps]
  [sub_app]
    type = TransientMultiApp
    positions = '0 0 0'
    input_files = 'two_way_twin_vectors_multiapp_vector_copy_transfer_mfem.i'
    execute_on = timestep_begin
  []
[]

[Transfers]
  [push]
    type = MultiAppVectorCopyTransfer
    to_multi_app = sub_app
    source_variable = 'moose_lagrange_vector moose_monomial_vector'
    variable = 'mfem_lagrange_vector mfem_monomial_vector'
  []

  [pull]
    type = MultiAppVectorCopyTransfer
    from_multi_app = sub_app
    source_variable = 'mfem_lagrange_vector mfem_monomial_vector'
    variable = 'mfem_lagrange_vector mfem_monomial_vector'
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
