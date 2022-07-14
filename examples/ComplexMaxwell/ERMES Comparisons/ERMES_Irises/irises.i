[Mesh]
  [gen]
    type = FileMeshGenerator
    file = gold/mesh_irises_hex.e
  [../]
  [./scale]
    type = TransformGenerator
    input = gen
    transform = SCALE
    vector_value ='1e-3 1e-3 1e-3'
  []
  uniform_refine=1
[]

[Modules]
  [ComplexMaxwell]
    family = NEDELEC_ONE
    order = FIRST

    pec_boundaries = '1'
    pec_penalty = 1.75e30

    wg_input_boundaries = '2'
    wg_output_boundaries = '3'
    wg_properties = 'Waveguide'
  []
[]

[UserObjects]
  [./Waveguide]
    type = WaveguideProperties
    port_length_vector = '0 22.86e-3 0'
    port_width_vector = '0 0 10.16e-3'
    frequency = 9.3e9
  [../]
[]

[Materials]
  [./Vacuum]
    type = ComplexConductor
    block = '1'
  [../]
[]

[AuxVariables]
  [./E]
    family = NEDELEC_ONE
    order = FIRST
  [../]
[]

[AuxKernels]
  [E_field]
    type = ComplexMagnitude
    vector_re = E_real
    vector_im = E_imag
    variable = E
    execute_on = 'timestep_end'
  [../]
[]

[Preconditioning]
  [pre]
    type = SMP
    full = true
    petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
    petsc_options_value = 'lu mumps'
  []
[]

[Executioner]
  type = Steady
  automatic_scaling = false
  solve_type = LINEAR
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
[]

[Outputs]
  exodus = true
  csv = true
[]
