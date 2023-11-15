[Mesh]
  type = ExclusiveMFEMMesh
  file = ./ermes_mouse_coarse.g
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

  current_density_re_name = current_density_re
  current_density_im_name = current_density_im
  magnetic_flux_density_re_name = magnetic_flux_density_re
  magnetic_flux_density_im_name = magnetic_flux_density_im
  joule_heating_density_name = joule_heating_density
[]

[FESpaces]
  [HCurlFESpace]
    type = MFEMFESpace
    fespace_type = ND
    order = FIRST
  []
  [HDivFESpace]
    type = MFEMFESpace
    fespace_type = RT
    order = CONSTANT
  []
  [L2FESpace]
    type = MFEMFESpace
    fespace_type = L2
    order = CONSTANT
  []
[]

[AuxVariables]
  [electric_field_re]
    type = MFEMVariable
    fespace = HCurlFESpace
  []
  [electric_field_im]
    type = MFEMVariable
    fespace = HCurlFESpace
  []
  [current_density_re]
    type = MFEMVariable
    fespace = HDivFESpace
  []
  [current_density_im]
    type = MFEMVariable
    fespace = HDivFESpace
  []
  [magnetic_flux_density_re]
    type = MFEMVariable
    fespace = HDivFESpace
  []
  [magnetic_flux_density_im]
    type = MFEMVariable
    fespace = HDivFESpace
  []
  [joule_heating_density]
    type = MFEMVariable
    fespace = L2FESpace
  []
[]

[BCs]
  [tangential_E_bdr]
    type = MFEMComplexVectorDirichletBC
    variable = electric_field
    boundary = '2 3 4'
    real_vector_coefficient = TangentialECoef
    imag_vector_coefficient = TangentialECoef
  []
  [waveguide_port_in]
    type = MFEMRWTE10PortRBC
    variable = electric_field
    boundary = '5'
    port_length_vector = '24.76e-2 0 0'
    port_width_vector = '0 12.38e-2 0'
    frequency = 900e6
    input_port = true
  []
  [waveguide_port_out]
    type = MFEMRWTE10PortRBC
    variable = electric_field
    boundary = '6'
    port_length_vector = '24.76e-2 0 0'
    port_width_vector = '0 12.38e-2 0'
    frequency = 900e6
    input_port = false
  []
[]

[Materials]
  [mouse]
    type = MFEMConductor
    electrical_conductivity_coeff = MouseEConductivity
    electric_permittivity_coeff = MousePermittivity
    magnetic_permeability_coeff = MousePermeability
    block = 1
  []
  [air]
    type = MFEMConductor
    electrical_conductivity_coeff = AirEConductivity
    electric_permittivity_coeff = AirPermittivity
    magnetic_permeability_coeff = AirPermeability
    block = 2
  []
[]

[VectorCoefficients]
  [TangentialECoef]
    type = MFEMVectorConstantCoefficient
    value_x = 0.0
    value_y = 0.0
    value_z = 0.0
  []
[]

[Coefficients]
  [frequency]
    type = MFEMConstantCoefficient
    value = 900e6
  []
  [MouseEConductivity]
    type = MFEMConstantCoefficient
    value = 0.97
  []
  [MousePermeability]
    type = MFEMConstantCoefficient
    value = 1.25663706e-6
  []
  [MousePermittivity]
    type = MFEMConstantCoefficient
    value = 3.807300762e-10
  []
  [AirEConductivity]
    type = MFEMConstantCoefficient
    value = 0.0
  []
  [AirPermeability]
    type = MFEMConstantCoefficient
    value = 1.25663706e-6
  []
  [AirPermittivity]
    type = MFEMConstantCoefficient
    value = 8.8541878176e-12
  []
[]

[Executioner]
  type = Steady
[]

[Outputs]
  [ParaViewDataCollection]
    type = MFEMParaViewDataCollection
    file_base = OutputData/ERMESMouseParaView
    refinements = 1
    high_order_output = true
  []
[]
