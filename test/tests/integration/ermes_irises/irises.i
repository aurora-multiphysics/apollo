[Mesh]
  type = ExclusiveMFEMMesh
  file = ./irises.g
[]

[Problem]
  type = MFEMProblem
  use_glvis = true
[]

[Formulation]
  type = ComplexEFormulation
  e_field_name = electric_field
  e_field_re_name = electric_field_re
  e_field_im_name = electric_field_im
  frequency_name = frequency
  magnetic_reluctivity_name = magnetic_reluctivity
  magnetic_permeability_name = magnetic_permeability
  electric_conductivity_name = electrical_conductivity
  dielectric_permittivity_name = dielectric_permittivity
[]

[AuxVariables]
  [./electric_potential]
    type = MFEMVariable
    fespace_name = _H1FESpace
    fespace_type = H1
    order = first
  [../]
[]

[Functions]
  [./tangential_E]
    type = ParsedVectorFunction
    value_x = 0.0
    value_y = 0.0
    value_z = 0.0
  [../]
[]

[BCs]
  [./tangential_E_bdr]
    type = MFEMComplexVectorFunctionDirichletBC
    variable = electric_field
    boundary = '1'
    real_function = tangential_E
    imag_function = tangential_E
  [../]
  [./waveguide_port_in]
    type = MFEMRWTE10PortRBC
    variable = electric_field
    boundary = '2'
    port_length_vector = '0 22.86e-3 0'
    port_width_vector = '0 0 10.16e-3'
    frequency = 9.3e9
    input_port = true
  [../]
  [./waveguide_port_out]
    type = MFEMRWTE10PortRBC
    variable = electric_field
    boundary = '3'
    port_length_vector = '0 22.86e-3 0'
    port_width_vector = '0 0 10.16e-3'
    frequency = 9.3e9
    input_port = false
  [../]
[]

[Materials]
  [./air]
    type = MFEMConductor
    electrical_conductivity_coeff = AirEConductivity
    electric_permittivity_coeff = AirPermittivity
    magnetic_permeability_coeff = AirPermeability
    block = 1
  [../]
[]

[Coefficients]
  [./AirEConductivity]
    type = MFEMConstantCoefficient
    value = 0.0
  [../]
  [./AirPermeability]
    type = MFEMConstantCoefficient
    value = 1.25663706e-6
  [../]
  [./AirPermittivity]
    type = MFEMConstantCoefficient
    value = 8.8541878176e-12
  [../]
  [./frequency]
    type = MFEMConstantCoefficient
    value = 9.3e9
  [../]
[]

[Executioner]
  type = Steady
[]

[Outputs]
  [ParaViewDataCollection]
    type = MFEMParaViewDataCollection
    file_base = OutputData/ERMESIrisesParaView
    refinements = 1
    high_order_output = true
  []
[]
