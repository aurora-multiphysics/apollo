[Mesh]
  type = GeneratedMesh
  dim = 3
  nz = 50
  nx = 10
  ny =5
  zmin = 0.0
  zmax = 160.0e-3
  xmin = 0.0
  # ymax = 22.86e-3
  xmax = 11.43e-3
  ymin = 0.0
  ymax =10.16e-3
  elem_type = HEX20
[]

[Modules]
  [ComplexMaxwell]
    family = NEDELEC_ONE
    order = FIRST

    pec_boundaries = 'left top bottom'
    pec_penalty = 1.75e30

    wg_input_boundaries = 'back'
    wg_output_boundaries = 'front'
    wg_properties = 'Waveguide'
  []
[]

[UserObjects]
  [./Waveguide]
    type = WaveguideProperties
    port_length_vector = '22.86e-3 0 0'
    port_width_vector = '0 10.16e-3 0'
    frequency = 9.3e9
  [../]
[]

[Materials]
  [./Cavity]
    type = ComplexConductor
    real_electrical_conductivity = 0.0 #rel
    frequency = 9.3e9
  [../]
[]

[AuxVariables]
  [./E_mag]
    family = MONOMIAL
    order = FIRST
  [../]
[]

[AuxKernels]
  [./calc_E_mag]
    type = VectorVariableComplexMagnitudeAux
    variable = E_mag # the auxvariable to compute
    real_vector = E_real # vector variable to compute from
    imag_vector = E_imag # vector variable to compute from
    execute_on = 'initial timestep_end'
  [../]
[]


[Preconditioning]
  [./pre]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Steady
  automatic_scaling = true
  solve_type = 'LINEAR'
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu mumps'
  petsc_options = '-snes_converged_reason -ksp_converged_reason -snes_linesearch_monitor'
[]

[Outputs]
  exodus = true
  csv = true
[]
