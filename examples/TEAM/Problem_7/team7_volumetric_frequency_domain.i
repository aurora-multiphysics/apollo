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
  [electric_potential]
    type = MFEMVariable
    fespace = H1FESpace
  []
[]

[Sources]
  [SourceCoil]
    type = MFEMDivFreeVolumetricSource
    vector_coefficient = SourceCurrentCoef
    hcurl_fespace = HCurlFESpace
    h1_fespace = H1FESpace
    block = '3 4 5 6'
  []
[]

[Functions]
  [RacetrackCoilCurrentFunction]
    type = RacetrackCoilCurrentDensity
    coil_axis_x = 194e-3
    coil_axis_y = 100e-3
    coil_thickness = 50e-3
    coil_current_magnitude = 2742.0
    coil_xsection_area = 2.5e-3
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

[VectorCoefficients]
  [SourceCurrentCoef]
    type = MFEMVectorFunctionCoefficient
    function = RacetrackCoilCurrentFunction
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
[]

[Executioner]
  type = Steady
  l_tol = 1e-16
[]

[Outputs]
  [ParaViewDataCollection]
    type = MFEMParaViewDataCollection
    file_base = OutputData/TEAM7ParaViewFrequencyDomain
    refinements = 1
    high_order_output = true
  []
[]
