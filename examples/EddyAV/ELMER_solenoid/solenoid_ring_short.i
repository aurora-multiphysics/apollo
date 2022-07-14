[Mesh]
  [gen]
    type = FileMeshGenerator
    file = gold/solenoid_ring.msh
  [../]
  second_order = true
[]

[Modules]
  [AVFormulation]
    vector_family = LAGRANGE_VEC
    vector_order = FIRST

    scalar_family = LAGRANGE
    scalar_order = FIRST

    tangent_h_boundaries = '21 22 23 24 25 26'
    tangent_h_penalty = 1e20
    surface_h_fields = 'zv zv zv zv zv zv'

    # zero_flux_boundaries = '21 22 23 24 25 26'
    # zero_flux_penalty = 1e20

    electric_potential_boundaries = '21 22 23 24 25 26'
    surface_electric_potentials = '0 0 0 0 0 0'
  []
[]

[Materials]
  [./solenoid]
    type = Conductor
    electrical_conductivity = 1e-10 #[S/m]
    block = 1
  [../]
  [./ring]
    type = Conductor
    electrical_conductivity = 2.273e7 # [S/m]
    block = 3
  [../]
  [./air]
    type = Conductor
    electrical_conductivity = 1e-10 #[S/m]
    block = 2
  [../]
[]

[Functions]
  [./solenoid_current_density]
    type = ParsedVectorFunction
    value_x = '-(1.0*y/sqrt(x*x+y*y))*t'
    value_y = '(1.0*x/sqrt(x*x+y*y))*t'
    value_z ='0.0'
  [../]
  [./zv]
    type = ParsedVectorFunction
    value_x = '0.0'
    value_y = '0.0'
    value_z ='0.0'
  [../]
  # [./V_surf]
  #   type = ParsedFunction
  #   value = '0'
  # [../]
[]

[Kernels]
  [./body_force] # looks just like boundary form but solves slower
    type = VectorBodyForce
    variable = magnetic_vector_potential
    function = solenoid_current_density
    block = 1
  [../]
[]

[AuxVariables]
  [E]
    # Cannot use Lagrange, as components may be discontinuous
    # at material boundaries.
    family = MONOMIAL_VEC
    order = FIRST
  []
  [V_applied]
    family = MONOMIAL
    order = FIRST
  []
  [B_x]
    order = CONSTANT
    family = MONOMIAL
  []
  [B_y]
    order = CONSTANT
    family = MONOMIAL
  []
  [B_z]
    order = CONSTANT
    family = MONOMIAL
  []
  [B]
    order = FIRST
    family = NEDELEC_ONE
  []
[]

[AuxKernels]
  # [./Applied_V]
  #   type = FunctionAux
  #   function = V_top
  #   variable = V_applied
  # [../]

  [E_field]
    type = MagneticField
    electric_vector_potential = magnetic_vector_potential
    # magnetic_scalar_potential = V
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
    # petsc_options_iname = '-pc_type -sub_pc_type -sub_pc_factor_shift_type -pc_asm_overlap'
    # petsc_options_value = ' asm      lu           NONZERO                   2'
  [../]
[]

[Executioner]
  type = Transient
  dt = 0.1 # doesn't remove discrepancies
  start_time = 0.0
  end_time = 1.0
  automatic_scaling = true
  scheme=BDF2
  # petsc_options_iname = '-pc_type -pc_hypre_type'
  # petsc_options_value = 'hypre boomeramg'

  solve_type = 'NEWTON'
  # nl_rel_tol = 1e-5
  # solve_type = JFNK
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu mumps'
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
