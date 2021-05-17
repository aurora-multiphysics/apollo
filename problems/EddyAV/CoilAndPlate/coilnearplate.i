[Mesh]
  [gen]
    type = FileMeshGenerator
    file = gold/coil_plate_2d.e
  [../]
  [./scale]
    type = TransformGenerator
    input = gen
    transform = SCALE
    vector_value ='1e-2 1e-2 1e-2'
    []
  [extrude]
    type = MeshExtruderGenerator
    input = scale
    num_layers = 1
    extrusion_vector = '0 0 0.01'
    bottom_sideset = 'new_back'
    top_sideset = 'new_front'
  []
[]

[Modules]
  [AVFormulation]
    vector_family = LAGRANGE_VEC
    vector_order = FIRST

    scalar_family = LAGRANGE
    scalar_order = FIRST

    zero_flux_boundaries = 'top_edge bottom_edge left_edge right_edge new_front new_back'
    zero_flux_penalty = 1e30

    electric_current_boundaries = 'top_edge bottom_edge left_edge right_edge new_front new_back'
    surface_electric_currents = 'zv zv zv zv zv zv'
  []
[]

[Materials]
  [./vacuum]
    type = Conductor
    electrical_conductivity = 1 # [S/m]
    block = 'vacuum coil'
  [../]
  [./aluminium]
    type = Conductor
    electrical_conductivity = 1e6 #[S/m]
    block = 'plate'
  [../]
[]

[Functions]
  [./dts]
    type = PiecewiseConstant
    x = '10.0 10.02 10.04 10.1'
    y = '0.5 2.5e-4 5.0e-4 1e-3'
    direction = right
  [../]
  [./coil_current_density]
    type = ParsedVectorFunction
    value_x = '0.0'
    value_y = '0.0'
    value_z ='J0*if(t<Deltat, t/Deltat,
                 if(t>t2, 1-(t-t2)/Deltat,
                    1)) '
    vars = 'J0 Deltat t2'
    vals = '1e6 10e-3 10'
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
    function = coil_current_density
    block = 'coil'
  [../]
[]

[AuxVariables]
  # Cannot use Lagrange, as components may be discontinuous
  # at material boundaries.
  [E]
    family = MONOMIAL_VEC
    order = FIRST
  []
  [J]
    order = FIRST
    family = MONOMIAL_VEC
  []
  [F]
    order = FIRST
    family = MONOMIAL_VEC
  []
  [J_eddy]
    order = FIRST
    family = MONOMIAL_VEC
  []
  [B]
    family = MONOMIAL_VEC
    order = FIRST
  []
[]

[AuxKernels]
  [J_source]
    type = VectorFunctionAux
    function = coil_current_density
    variable = J
  [../]
  [J_eddy]
   type = AVCurrentDensity
    magnetic_vector_potential = magnetic_vector_potential
    electric_scalar_potential = electric_scalar_potential
    variable = J_eddy
    execute_on = timestep_end
  [../]
  [Lorentz_Force_Density]
   type = LorentzForce
    current_density = J_eddy
    magnetic_flux_density = B
    variable = F
    execute_on = timestep_end
  [../]
  [E_field]
    type = ElectricField
    magnetic_vector_potential = magnetic_vector_potential
    electric_scalar_potential = electric_scalar_potential
    variable = E
    execute_on = timestep_end
  []
  [B_field]
    type = VectorCurl
    vector_variable = magnetic_vector_potential
    variable = B
    execute_on = timestep_end
  [../]
[]
[Preconditioning]
  [./pre]
    type = SMP
    full = true
    petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
    petsc_options_value = 'lu mumps'
    petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
  [../]
[]

[Executioner]
  type = Transient
  automatic_scaling = true
  solve_type = 'LINEAR'
  nl_abs_tol = 1e-8
  nl_rel_tol = 1e-8
  [./TimeStepper]
    type = FunctionDT
    function = dts
  [../]
[]

[Outputs]
  exodus = true
  csv = true
  # perf_graph = true
[]
