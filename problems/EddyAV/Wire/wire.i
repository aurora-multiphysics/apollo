[Mesh]
  [gen]
    type = FileMeshGenerator
    file = wire.e
  [../]
  second_order=true
[]

[Modules]
  [AVFormulation]
    vector_family = LAGRANGE_VEC
    vector_order = FIRST

    scalar_family = LAGRANGE
    scalar_order = FIRST

    zero_flux_boundaries = 'Far TopWire BottomWire TopAir BottomAir'
    zero_flux_penalty = 1e30

    electric_current_boundaries = 'TopWire'
    surface_electric_currents = 'wire_current_density'

    electric_potential_boundaries = 'BottomWire BottomAir'
    surface_electric_potentials = '0.0 0.0'
  []
[]

[Materials]
  [./vacuum]
    type = Conductor
    electrical_conductivity = 0 # [S/m]
    block = 'Air'
  [../]
  [./copper]
    type = Conductor
    electrical_conductivity = 1e7 #[S/m]
    block = 'Wire'
  [../]
[]

[Functions]
  [./wire_current_density]
    type = ParsedVectorFunction
    value_x = '0.0'
    value_y = '0.0'
    value_z = '1e6'
  [../]
  [./zv]
    type = ParsedVectorFunction
    value_x = '0.0'
    value_y = '0.0'
    value_z ='0.0'
  [../]
[]

[Kernels]
  [./body_force] # looks just like boundary form but solves slower
    type = VectorBodyForce
    variable = magnetic_vector_potential
    function = wire_current_density
    block = 'Wire'
  [../]
[]

[AuxVariables]
  [E]
    # Cannot use Lagrange, as components may be discontinuous
    # at material boundaries.
    family = MONOMIAL_VEC
    order = FIRST
  []
  # [V_applied]
  #   family = MONOMIAL
  #   order = FIRST
  # []
  # [B_x]
  #   order = CONSTANT
  #   family = MONOMIAL
  # []
  # [B_y]
  #   order = CONSTANT
  #   family = MONOMIAL
  # []
  # [B_z]
  #   order = CONSTANT
  #   family = MONOMIAL
  # []
  [B]
    family = MONOMIAL_VEC
    order = FIRST
  []
[]

[AuxKernels]
  [E_field]
    type = ElectricField
    magnetic_vector_potential = magnetic_vector_potential
    electric_scalar_potential = electric_scalar_potential
    variable = E
    execute_on = timestep_end
  []
  [B_field]
    type = CurrentDensity
    magnetic_field = magnetic_vector_potential
    variable = B
    execute_on = timestep_end
  [../]
[]

[Preconditioning]
  [./pre]
    type = SMP
    full = true
    # petsc_options = '-ksp_diagonal_scale -ksp_diagonal_scale_fix'
    petsc_options_iname = '-pc_type -sub_pc_type -sub_pc_factor_shift_type -pc_asm_overlap'
    petsc_options_value = ' asm      lu           NONZERO                   2'
  [../]
[]

[Executioner]
  type = Transient
  dt = 0.001 # doesn't remove discrepancies
  start_time = 0.0
  end_time = 0.01
  automatic_scaling = true
  scheme=BDF2
  # petsc_options_iname = '-pc_type -pc_hypre_type'
  # petsc_options_value = 'hypre boomeramg'

  solve_type = 'LINEAR'
  # nl_rel_tol = 1e-5
  # solve_type = JFNK
  # petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  # petsc_options_value = 'lu mumps'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  # [./Adaptivity]
  #   active = 'solenoid'
  #   refine_fraction = 0.3
  #   max_h_level = 7
  #   cycles_per_step = 2
  # [../]
[]

[Outputs]
  exodus = true
  csv = true
[]
