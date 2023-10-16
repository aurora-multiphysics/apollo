[Mesh]
  type = CoupledMFEMMesh
  file = gold/mug-tet10.e
  dim = 3
[]

[Variables]
  [./moose_diffused]
    family = LAGRANGE
    order = SECOND
  [../]
[]

[AuxVariables]
  [./temperature_moose]
    family = LAGRANGE
    order = SECOND
  [../]

  [./temperature_mfem]
    family = LAGRANGE
    order = SECOND
  [../]
[]

[Kernels]
  [diff]
    type = Diffusion
    variable = moose_diffused
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
  [./l2_difference]
    type = ElementL2Difference
    variable = temperature_mfem
    other_variable = temperature_moose
  [../]
[]

[ICs]
  [./temperature_moose_ic]
    type = FunctionIC
    variable = temperature_moose
    function = 2-x*x
  [../]

  [./temperature_mfem_ic]
    type = ConstantIC
    variable = temperature_mfem
    value = 0.0
  [../]
[]

[MultiApps]
  [sub_app]
    type = TransientMultiApp
    positions = '0 0 0'
    input_files = 'diffusion_mfem.i'
    execute_on = timestep_begin
  []
[]

[Transfers]
  [push_temperature]
    type = MultiAppNearestNodeTransfer

    # Transfer to the sub-app from this app.
    to_multi_app = sub_app

    # The name of the variable in this app to transfer.
    source_variable = temperature_moose

    # The name of the auxiliary variable in the sub-app.
    variable = temperature
  []

  [pull_temperature]
    type = MultiAppNearestNodeTransfer

    # Transfer from the sub-app to this app.
    from_multi_app = sub_app

    # The name of the variable in the sub-app.
    source_variable = temperature

    # The name of the auxiliary variable in this app.
    variable = temperature_mfem
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
  exodus = true
[]
