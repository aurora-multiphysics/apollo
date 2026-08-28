[Mesh]
  type = ExclusiveMFEMMesh
  file = ./team7.g
[]

[Problem]
  type = MFEMProblem
  use_glvis = false
[]

[Formulation]
  type = ComplexAFormulation
  magnetic_vector_potential_name = magnetic_vector_potential
  magnetic_vector_potential_re_name = magnetic_vector_potential_real
  magnetic_vector_potential_im_name = magnetic_vector_potential_imag
  frequency_name = frequency
  magnetic_reluctivity_name = magnetic_reluctivity
  magnetic_permeability_name = magnetic_permeability
  electric_conductivity_name = electrical_conductivity
  dielectric_permittivity_name = dielectric_permittivity
  magnetic_flux_density_re_name = magnetic_flux_density_real
  magnetic_flux_density_im_name = magnetic_flux_density_imag
  current_density_re_name = current_density_real
  current_density_im_name = current_density_imag
[]

[FESpaces]
  [H1FESpace]
    type = MFEMFESpace
    fespace_type = H1
    order = FIRST
  []
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
[]

[AuxVariables]
  [magnetic_vector_potential_real]
    type = MFEMVariable
    fespace = HCurlFESpace
  []
  [magnetic_vector_potential_imag]
    type = MFEMVariable
    fespace = HCurlFESpace
  []
  [magnetic_flux_density_real]
    type = MFEMVariable
    fespace = HDivFESpace
  []
  [magnetic_flux_density_imag]
    type = MFEMVariable
    fespace = HDivFESpace
  []
  [current_density_real]
    type = MFEMVariable
    fespace = HDivFESpace
  []
  [current_density_imag]
    type = MFEMVariable
    fespace = HDivFESpace
  []
  [source_electric_field]
    type = MFEMVariable
    fespace = HCurlFESpace
  []
  [source_current_density]
    type = MFEMVariable
    fespace = HDivFESpace
  []
[]

[AuxKernels]
  [LineSampler]
    type = MFEMLineSamplerAux
    filename = 'bfield.csv'
    variable = magnetic_flux_density_real
    num_points = 100
    start_point = '0.09 0.072 0.034'
    end_point = '0.1 0.072 0.034'
    header = 't (s), x (m), y (m), z (m), B_x (T), B_y (T), B_z (T)'
  []
[]

[Sources]
  [SourceCoil]
    type = MFEMClosedCoilSource
    total_current_coefficient = CurrentCoef
    source_electric_field_gridfunction = source_electric_field
    source_current_density_gridfunction = source_current_density
    hcurl_fespace = HCurlFESpace
    h1_fespace = H1FESpace
    coil_xsection_boundary = 7
    block = '3 4 5 6'
  []
[]

[Materials]
  [air]
    type = MFEMConductor
    electrical_conductivity_coeff = AirEConductivity
    electric_permittivity_coeff = AirPermittivity
    magnetic_permeability_coeff = AirPermeability
    block = '1 3 4 5 6'
  []
  [plate]
    type = MFEMConductor
    electrical_conductivity_coeff = PlateEConductivity
    electric_permittivity_coeff = PlatePermittivity
    magnetic_permeability_coeff = PlatePermeability
    block = 2
  []
[]

[Coefficients]
  [frequency]
    type = MFEMConstantCoefficient
    value = 200
  []
  [AirEConductivity]
    type = MFEMConstantCoefficient
    value = 1.0
  []
  [AirPermeability]
    type = MFEMConstantCoefficient
    value = 1.25663706e-6
  []
  [AirPermittivity]
    type = MFEMConstantCoefficient
    value = 0.0
  []
  [PlateEConductivity]
    type = MFEMConstantCoefficient
    value = 3.526e7
  []
  [PlatePermeability]
    type = MFEMConstantCoefficient
    value = 1.25663706e-6
  []
  [PlatePermittivity]
    type = MFEMConstantCoefficient
    value = 0.0
  []
  [CurrentCoef]
    type = MFEMConstantCoefficient
    value = 2742 # Ampere-turns
  []
[]

[Executioner]
  type = Steady
  l_tol = 1e-16
[]

[Outputs]
  [ParaViewDataCollection]
    type = MFEMParaViewDataCollection
    file_base = OutputData/TEAM7ParaViewFrequencyDomain
  []
[]
