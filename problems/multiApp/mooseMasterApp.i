[Mesh]
  type = CoupledMFEMMesh
  file = ./multiAppCube.e
  dim = 3
  boundary_id = '1 2 3'
[]

[Problem]
  type = FEProblem
  coord_type = XYZ
[]

[Kernels]
  [heat_conduction]
    type = ADHeatConduction
    variable = temperature
  []
[]

[Variables]
  [temperature]
    initial_condition = 293.15 # Start at room temperature
  []
[]


[Functions]

[]

[BCs]
  [inlet_temperature]
    type = DirichletBC
    variable = temperature
    boundary = 1
    value = 293.15
  []

  [outlet_temperature]
    type = DirichletBC
    variable = temperature
    boundary = 2
    value = 350
  []
[]

[Materials]
  [material]
    type = ADHeatConductionMaterial
    temp = temperature
    specific_heat = 2000
    thermal_conductivity = 500 
  []
[]

[MultiApps]
  [sub]
    type = FullSolveMultiApp
    input_files = mfemApp.i
    clone_parent_mesh = true
  []
[]

[Transfers]
  [./from_sub]
    type = MultiAppCopyTransfer
    source_variable = u
    variable = u
    from_multi_app = sub
  [../]
[]

[Executioner]
  type = Steady
  steady_state_tolerance = 1e-6
  steady_state_detection = true
  automatic_scaling = true
  compute_scaling_once = false
  solve_type = PJFNK
  petsc_options_iname = '-ksp_gmres_restart -pc_type -sub_pc_type'
  petsc_options_value = '300                 asm      cholesky'
  nl_rel_tol = 1e-8
  nl_abs_tol = 1e-10
  l_tol = 1e-8
  l_abs_tol = 1e-10
  line_search = none
[]

[Outputs]
  [out]
    type = Exodus
    elemental_as_nodal = true
  []
[]