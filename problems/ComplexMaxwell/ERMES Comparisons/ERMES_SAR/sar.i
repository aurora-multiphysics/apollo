[Mesh]
  [gen]
    type = FileMeshGenerator
    file = gold/mesh_sar_tet.e
  [../]
[]

[Modules]
  [ComplexMaxwell]
    family = NEDELEC_ONE
    order = FIRST

    pec_boundaries = 'left top bottom'
    pec_penalty = 1.75e30

    wg_input_boundaries = 'back'
    wg_output_boundaries = 'front'
    wg_properties = 'WaveguideProps'
  []
[]

[UserObjects]
  [./WaveguideProps]
    type = WaveguideProperties
    port_length_vector = '24.76e-2 0 0'
    port_width_vector = '0 12.38e-2 0'
    frequency = 900e6
  [../]
[]

[Materials]
  [./Mouse]
    type = ComplexConductor
    real_electrical_conductivity = 0.97
    real_rel_permittivity = 43
    block = 'mouse'
  [../]
  [./Waveguide]
    type = ComplexConductor
    block = 'waveguide'
  [../]
[]

[AuxVariables]
  [./E]
    family = NEDELEC_ONE
    order = FIRST
  [../]
  [./q]
    family = MONOMIAL
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
  [heat_source]
    type = JouleHeating
    electric_field = E
    variable = q
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
