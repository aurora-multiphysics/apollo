[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 3
    nx = 21
    ny = 21
    nz = 21
    xmin = -0.010
    ymin = -0.010
    zmin = -0.010    
    xmax = 0.010
    ymax = 0.010    
    zmax = 0.010
    elem_type = TET4 # give different accuracy
    #elem_type = HEX20
  []
  [./bottom]
    input = gen
    type = SubdomainBoundingBoxGenerator
    location = inside
    bottom_left = '-0.005 -0.005 -0.005'
    top_right = '0.005 0.005 0.005'
    block_id = 1
  [../]
[]

[Materials]
  [./block0]
    type = Conductor
    block = 0
    resistivity = 1 # [m/S]
    permeability = 1.25663706144e-6
  [../]
  [./block1]
    type = Superconductor
    block = 1
    nonlinearity_parameter = 100
    critical_current_density = 1e8 # Critical current density [A/mm2]
    critical_electric_field = 1e-4 # Critical electric field [muV/m]
    magnetic_field = T # [kA/mm]
    permeability = 1.25663706144e-6
  [../]
[]



[Variables]
  [./T]
    family = NEDELEC_ONE
    order = FIRST
  [../]
  [./phi]
    family = LAGRANGE
    order = FIRST
  [../]
[]

[Functions]
  [./Applied_H]
    type = ParsedVectorFunction
    value_x = '0.0'
    value_y = '0.0'
    value_z = '(bmax/(4*pi*1e-7))*sin(2*pi*f*t)'
    vars = 'bmax f'
    vals = '0.2 50'
    # value_z ='cos(0.1*pi*t)/(0.1*pi)'
  [../]
  [./Applied_dBdt]
    type = ParsedVectorFunction
    value_x = '0.0'
    value_y = '0.0'
    value_z ='bmax*2*pi*f*cos(2*pi*f*t)'
    vars = 'bmax f'
    vals = '0.2 50'
  [../]
  
  [./z_sln]
    type = ParsedFunction
    value = '(bmax/(4*pi*1e-7))*sin(2*pi*f*t)'
    vars = 'bmax f'
    vals = '0.2 50'
  [../]   
[]

[Kernels]
  [./Faraday]
    type = ExplicitGaugedMaxwell
    variable = T
    scalar_potential = phi
  [../]
  [./Gauge]
    type = MaxwellDivergenceFree
    variable = phi
    vector_potential = T
  [../]
  # [./body_force] # looks just like boundary form but solves slower
  #   type = VectorBodyForce
  #   variable = T
  #   value = -1
  #   function = Applied_dBdt
  # [../]
[]


[BCs]
  [./bnd]
    type = FunctionFluxBC
    variable = phi
    boundary = 'left right top bottom front back'
    function = 'Applied_dBdt'
  [../]
  [./bnd2]
    type = VectorCurlPenaltyDirichletBC
    boundary = 'left right top bottom front back'
    penalty = 1e20
    # function_z = '0'
    function_z = 'z_sln'
    variable = T
  [../]  
[]

[AuxVariables]
  [./H] 
    # Cannot use Lagrange, as components may be discontinuous
    # at material boundaries.
    family = MONOMIAL_VEC
    order = FIRST
  [../]
  [./H_applied]
    family = MONOMIAL_VEC
    order = FIRST
  [../]
  [current_density_x]
    order = CONSTANT
    family = MONOMIAL
  []
  [current_density_y]
    order = CONSTANT
    family = MONOMIAL
  []
  [current_density_z]
    order = CONSTANT
    family = MONOMIAL
  []
  [current_density]
    order = CONSTANT
    family = MONOMIAL_VEC
  []
  [magnetic_moment]
    order = CONSTANT
    family = MONOMIAL_VEC
  []
  [magnetic_moment_z]
    order = CONSTANT
    family = MONOMIAL
  []  
[]

[AuxKernels]
  [./Applied_H]
    type = VectorFunctionAux
    function = Applied_H
    variable = H_applied
  [../]

  [magnetic_field]
    type = MagneticField
    electric_vector_potential = T
    magnetic_scalar_potential = phi
    variable = H
    execute_on = timestep_end
  []
  [current_density]
    type = CurrentDensity
    magnetic_field = T
    variable = current_density
    execute_on = timestep_end
  [../]
  [magnetic_moment]
    type = MagneticMoment
    magnetic_field = T
    variable = magnetic_moment
    execute_on = timestep_end
  [../]  
  [magnetic_moment_z]
    type = VectorVariableComponentAux
    variable = magnetic_moment_z
    component = z
    execute_on = timestep_end
    vector_variable = magnetic_moment
  [../]
[]

[UserObjects]
  [./block_magnetization]
    type = BlockAverageValue
    variable = magnetic_moment_z
    execute_on = timestep_end
    outputs = none
  [../]
[]

[Preconditioning]
  [./pre]
    type = SMP
    full = true
    petsc_options = '-ksp_diagonal_scale -ksp_diagonal_scale_fix'
    petsc_options_iname = '-pc_type -sub_pc_type -sub_pc_factor_shift_type -pc_asm_overlap'
    petsc_options_value = ' asm      lu           NONZERO                   2'    
  [../]
[]

[Executioner]
  type = Transient
  dt = 0.000125 # doesn't remove discrepancies
  start_time = 0.0
  end_time = 0.025  
  automatic_scaling = true

  # petsc_options_iname = '-pc_type -pc_hypre_type'
  # petsc_options_value = 'hypre boomeramg'

  solve_type = 'NEWTON'
  # nl_abs_tol = 1e-5
  # nl_rel_tol = 1e-5
  # solve_type = PJFNK  
  # petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  # petsc_options_value = 'lu mumps'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
[]

[Postprocessors]
  [./H_applied]
  type = FunctionValuePostprocessor
  function = z_sln
  execute_on = timestep_end
  [../]
  [./magnetization_domain]
    type = BlockAveragePostprocessor
    block_average_userobject = block_magnetization
    block = 1
  [../]
[]
[Outputs]
  exodus = true
  csv = true
[]
