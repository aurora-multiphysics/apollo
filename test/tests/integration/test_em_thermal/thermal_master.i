[Mesh]
  type = CoupledMFEMMesh
  file = ./gold/cylinder-hex-q2.e
  dim = 3
[]

[Variables]
  [temperature]
    family = LAGRANGE
    order = FIRST
    initial_condition = 300
  []
[]

[AuxVariables]
  [joule_heating]
    family = MONOMIAL
    order = CONSTANT
  []
[]

[Kernels]
  [diff]
    type = Diffusion
    variable = temperature
  []
  [heat_source]
    type = CoupledForce
    variable = temperature
    v = joule_heating
  []
  [td]
    type = TimeDerivative
    variable = temperature
  []
[]

[BCs]
  [dirichlet_temp_bc]
    type = DirichletBC
    variable = temperature
    boundary = '1 2 3'
    value = 300
  []
[]

[Executioner]
  type = Transient
  dt = 0.5
  start_time = 0.0
  end_time = 1.0
[]

[Outputs]
  exodus = true
[]

[MultiApps]
  [sub_app]
    type = TransientMultiApp
    positions = '0 0 0'
    input_files = 'em_coupled.i'
    execute_on = timestep_begin
  []
[]

[Transfers]
  [push_temperature]
    type = MultiAppNearestNodeTransfer

    # Transfer to the sub-app from this app
    to_multi_app = sub_app

    # The name of the variable in this app
    source_variable = temperature

    # The name of the auxiliary variable in the sub-app
    variable = temperature
  []
  [pull_joule_heating]
    type = MultiAppNearestNodeTransfer

    # Transfer from the sub-app to this app
    from_multi_app = sub_app

    # The name of the variable in the sub-app
    source_variable = joule_heating

    # The name of the auxiliary variable in this app
    variable = joule_heating
  []
[]
