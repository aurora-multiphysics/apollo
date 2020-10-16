# This example reproduces the libmesh vector_fe example 3 results

[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 3
    nx = 10
    ny = 10
    nz = 1
    xmin = -1
    ymin = -1
    xmax = 1
    ymax = 1    
    zmin = -0.2
    zmax = 0.2
    # elem_type = TET10
    elem_type = HEX20
  []
  [./bottom]
    input = gen
    type = SubdomainBoundingBoxGenerator
    location = inside
    bottom_left = '-0.5 -0.5 -0.2'
    top_right = '0.5 0.5 0.2'
    block_id = 1
  [../]
[]

[Materials]
  [./block0]
    type = Conductor
    block = 0
    resistivity = 1e10
    permeability = 1.0
  [../]
  [./block1]
    type = Superconductor
    block = 1
    nonlinearity_parameter = 10
    critical_current_density = 2
    critical_electric_field = 0.1
    permeability = 1.0
    magnetic_field = T
  [../]
[]

[Functions]
  [./Applied_H]
    type = ParsedVectorFunction
    value_x = '0.0'
    value_y = '0.0'
    value_z = 'sin(0.1*pi*t)'
    # value_z ='cos(0.1*pi*t)/(0.1*pi)'
  [../]
  [./Applied_dBdt]
    type = ParsedVectorFunction
    value_x = '0.0'
    value_y = '0.0'
    value_z ='0.1*pi*cos(0.1*pi*t)'
    # value_z ='0'
    # value_z ='-0.1*pi*cos(0.1*pi*t)'
  [../]  
  [./z_sln]
    type = ParsedFunction
    value = 'sin(0.1*pi*t)'
    # value = 'cos(0.1*pi*t)/(0.1*pi)'
  [../]  
[]

[Variables]
  [./T]
    family = LAGRANGE_VEC
    order = FIRST
  [../]
  [./phi]
    family = LAGRANGE
    order = SECOND
  [../]
[]

[Kernels]
  [./Faraday]
    type = GaugedMaxwell
    variable = T
    scalar_potential = phi
  [../]
  [./Gauge]
    type = MaxwellDivergenceFree
    variable = phi
    vector_potential = T
  [../]
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
    penalty = 1e10
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
  [../]
[]

[Executioner]
  type = Transient
  dt = 0.1
  num_steps = 50
  start_time = 0.0
  automatic_scaling = true
  # solve_type = PJFNK
  # petsc_options_iname = '-pc_type -pc_hypre_type'
  # petsc_options_value = 'hypre boomeramg'

  solve_type = 'NEWTON'
  petsc_options_iname = '-pc_type'
  petsc_options_value = 'lu'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  # [TimeStepper]
  #   type = IterationAdaptiveDT
  #   optimal_iterations = 1
  #   linear_iteration_ratio = 1
  #   dt = 0.1
  # []  
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
