[Mesh]
  [gen]
    type = FileMeshGenerator
    file = solenoid_ring.msh
  [../]
  second_order = true
[]

[Materials]
  [./solenoid]
    type = Conductor
    resistivity = 795774.715457 #rel
    permeability = 1e-10 # [m/S]
    block = 1
  [../]      
  [./ring]
    type = Conductor
    resistivity = 795774.715457
    permeability = 2.273e7 # [m/S]
    block = 3
  [../]
  [./air]
    type = Conductor
    resistivity = 795774.715457 #rel
    permeability = 1e-10 # [m/S]
    block = 2
  [../]
[]

[Variables]
  [./A]
    family = LAGRANGE_VEC
    order = FIRST
  [../]
  # [./V]
  #   family = LAGRANGE
  #   order = FIRST
  # [../]
[]

[Functions]
  [./solenoid_current_density]
    type = ParsedVectorFunction
    value_x = '-(1.0*y/sqrt(x*x+y*y))*t'
    value_y = '(1.0*x/sqrt(x*x+y*y))*t'
    value_z ='0.0'
  [../]
  [./V_surf]
    type = ParsedFunction
    value = '0'
  [../]       
[]

[Kernels]
  [./Faraday]
    type = EddyAVFaraday
    variable = A
    # scalar_potential = V
  [../]
  # [./Gauge]
  #   type = EddyAVGauss
  #   variable = V
  #   vector_potential = A
  # [../]
  [./body_force] # looks just like boundary form but solves slower
    type = VectorBodyForce
    variable = A
    function = solenoid_current_density
    block = 1
  [../]
[]


[BCs]
  # [./bnd3]
  #   type = FunctionDirichletBC
  #   variable = V
  #   boundary = '21 22 23 24 25 26'
  #   function = 'V_surf'
  # [../]
  # [./bnd2]
  #   type = VectorFunctionDirichletBC
  #   variable = T
  #   boundary = '1 2 3 4 5 6'
  #   function = 'Zero_Vector'
  # [../]  
  [./bnd2]
    type = VectorCurlPenaltyDirichletBC
    boundary = '21 22 23 24 25 26'
    penalty = 1e20
    function_z = '0'
    #function_z = 'z_sln'
    variable = A
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
    order = CONSTANT
    family = MONOMIAL_VEC
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
    electric_vector_potential = A
    # magnetic_scalar_potential = V
    variable = E
    execute_on = timestep_end
  []
  [B_field]
    type = CurrentDensity
    magnetic_field = A
    variable = B
    execute_on = timestep_end
  [../]
  # [magnetic_moment]
  #   type = MagneticMoment
  #   magnetic_field = T
  #   variable = magnetic_moment
  #   execute_on = timestep_end
  # [../]  
  # [magnetic_moment_z]
  #   type = VectorVariableComponentAux
  #   variable = magnetic_moment_z
  #   component = z
  #   execute_on = timestep_end
  #   vector_variable = magnetic_moment
  # [../]
[]

# [UserObjects]
#   [./block_magnetization]
#     type = BlockAverageValue
#     variable = magnetic_moment_z
#     execute_on = timestep_end
#     outputs = none
#   [../]
# []

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

# [Postprocessors]
#   [./H_applied]
#   type = FunctionValuePostprocessor
#   function = z_sln
#   execute_on = timestep_end
#   [../]
#   [./magnetization_domain]
#     type = BlockAveragePostprocessor
#     block_average_userobject = block_magnetization
#     block = 1
#   [../]
# []
[Outputs]
  exodus = true
  csv = true
[]
