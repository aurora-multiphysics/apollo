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
  elem_type = TET10
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
  [./Metal]
    type = ComplexConductor
    real_conductivity = 0.0 #rel
    frequency = 9.3e9
  [../]
[]

[Variables]
  [./E_real]
    family = NEDELEC_ONE
    order = FIRST
  [../]
  [./E_imag]
    family = NEDELEC_ONE
    order = FIRST
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
[Kernels]
  [./Real]
    type = ComplexMaxwellReal
    variable = E_real
    v = E_imag
    waveguide_properties = Waveguide
  [../]
  [./Imag]
    type = ComplexMaxwellImag
    variable = E_imag
    v = E_real
    waveguide_properties = Waveguide
  [../]
[]


[BCs]
  [./wallReal]
    type = VectorTangentialPenaltyDirichletBC
    boundary = 'left top bottom'
    penalty = 1.75e30
    variable = E_real
    v = E_imag
  [../]
  [./wallImag]
    type = VectorTangentialPenaltyDirichletImagBC
    boundary = 'left top bottom'
    penalty = 1.75e30
    variable = E_imag
    v = E_real
  [../]
  [./inputPortReal]
    type = RobinTE10RealBC
    boundary = 'back'
    variable = E_real
    v = E_imag
    waveguide_properties = Waveguide
    input_port = true
  [../]
  [./inputPortImag]
    type = RobinTE10ImagBC
    boundary = 'back'
    waveguide_properties = Waveguide
    variable = E_imag
    v = E_real
    input_port = true
  [../]
  [./outputPortReal]
    type = RobinTE10RealBC
    boundary = 'front'
    waveguide_properties = Waveguide
    variable = E_real
    v = E_imag
    input_port = false
  [../]
  [./outputPortImag]
    type = RobinTE10ImagBC
    boundary = 'front'
    waveguide_properties = Waveguide
    variable = E_imag
    v = E_real
    input_port = false
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
