[Mesh]
  [gen]
    type = FileMeshGenerator
    file = team7.e
  [../]
[]

[Modules]
  [AVFormulation]
    vector_family = LAGRANGE_VEC
    vector_order = FIRST

    scalar_family = LAGRANGE
    scalar_order = FIRST

    zero_flux_boundaries = 'top bottom left right front back'
    zero_flux_penalty = 1e20

    electric_current_boundaries = 'CoilGroundPlane'
    surface_electric_currents = 'coil_current_density'

    # electric_potential_boundaries = 'CoilGroundPlane'
    # surface_electric_potentials = '0'

  []
[]

[Materials]
  [./vacuum]
    type = Conductor
    electrical_conductivity = 1 # [S/m]
    block = 'vacuum'
  [../]
  [./copper]
    type = Conductor
    electrical_conductivity = 5.7e7 #[S/m]
    block = 'coilFL coilFR coilBL coilBR'
  [../]
  [./aluminium]
    type = Conductor
    electrical_conductivity = 3.526e7 #[S/m]
    block = 'plate'
  [../]
[]

[Functions]
  [./coil_current_density]
    type = ParsedFunction
    value =  ' 1.0968e6*sin(2*pi*100*t)'
  [../]
  # [./coil_current_density]
  #   type = ParsedVectorFunction
  #   value_x = '-(1.0*y/sqrt(x*x+y*y))*t'
  #   value_y = '(1.0*x/sqrt(x*x+y*y))*t'
  #   value_z ='0.0'
  # [../]
  [./zv]
    type = ParsedVectorFunction
    value_x = '0.0'
    value_y = '0.0'
    value_z ='0.0'
  [../]
[]

# [Kernels]
#   [./body_force] # looks just like boundary form but solves slower
#     type = VectorBodyForce
#     variable = magnetic_vector_potential
#     function = solenoid_current_density
#     block = 1
#   [../]
# []

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
  # [./Applied_V]
  #   type = FunctionAux
  #   function = V_top
  #   variable = V_applied
  # [../]

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
    # petsc_options_iname = '-pc_type -sub_pc_type -sub_pc_factor_shift_type -pc_asm_overlap'
    # petsc_options_value = ' asm      lu           NONZERO                   2'
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
