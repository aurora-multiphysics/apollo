[Mesh]
  type = ExclusiveMFEMMesh
  file = ./coil.gen
  dim = 3
[]

[Problem]
  type = MFEMProblem
  use_glvis = false
[]

[Formulation]
  type = MagnetostaticFormulation
  magnetic_vector_potential_name = magnetic_vector_potential
  magnetic_reluctivity_name = magnetic_reluctivity
  magnetic_permeability_name = magnetic_permeability
  magnetic_flux_density_name = magnetic_flux_density
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
  [magnetic_vector_potential]
    type = MFEMVariable
    fespace = HCurlFESpace
  []
  [magnetic_flux_density]
    type = MFEMVariable
    fespace = HDivFESpace
  []
  [source_current_density]
    type = MFEMVariable
    fespace = HCurlFESpace
  []
  [electric_potential]
    type = MFEMVariable
    fespace = H1FESpace
  []
[]

[BCs]
  [tangential_E_bdr]
    type = MFEMVectorDirichletBC
    variable = magnetic_vector_potential
    boundary = '1 2 4'
    vector_coefficient = TangentialECoef
  []
  [high_terminal]
    type = MFEMScalarDirichletBC
    variable = electric_potential
    boundary = '1'
    coefficient = HighPotential
  []
  [low_terminal]
    type = MFEMScalarDirichletBC
    variable = electric_potential
    boundary = '2'
    coefficient = LowPotential
  []
[]

[Materials]
  [coil]
    type = MFEMConductor
    electrical_conductivity_coeff = CoilEConductivity
    electric_permittivity_coeff = CoilPermittivity
    magnetic_permeability_coeff = CoilPermeability
    block = 1
  []
  [air]
    type = MFEMConductor
    electrical_conductivity_coeff = AirEConductivity
    electric_permittivity_coeff = AirPermittivity
    magnetic_permeability_coeff = AirPermeability
    block = 2
  []
  [core]
    type = MFEMConductor
    electrical_conductivity_coeff = CoreEConductivity
    electric_permittivity_coeff = CorePermittivity
    magnetic_permeability_coeff = CorePermeability
    block = 3
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
  [CoilEConductivity]
    type = MFEMConstantCoefficient
    value = 62.83185
  []
  [CoilPermeability]
    type = MFEMConstantCoefficient
    value = 1.0
  []
  [CoilPermittivity]
    type = MFEMConstantCoefficient
    value = 0.0
  []

  [AirEConductivity]
    type = MFEMConstantCoefficient
    value = 62.83185e-6
  []
  [AirPermeability]
    type = MFEMConstantCoefficient
    value = 1.0
  []
  [AirPermittivity]
    type = MFEMConstantCoefficient
    value = 0.0
  []

  [CoreEConductivity]
    type = MFEMConstantCoefficient
    value = 62.83185e-6
  []
  [CorePermeability]
    type = MFEMConstantCoefficient
    value = 1.0
  []
  [CorePermittivity]
    type = MFEMConstantCoefficient
    value = 0.0
  []

  [HighPotential]
    type = MFEMConstantCoefficient
    value = 1.0
  []
  [LowPotential]
    type = MFEMConstantCoefficient
    value = -1.0
  []
[]

[Sources]
  [SourcePotential]
    type = MFEMScalarPotentialSource
    potential = electric_potential
    conductivity = electrical_conductivity
    h1_fespace = H1FESpace
    hcurl_fespace = HCurlFESpace
    block = 1
  []
[]

[Executioner]
  type = Steady
  l_tol = 1e-6
  l_max_its = 10
[]

[Outputs]
  [ParaViewDataCollection]
    type = MFEMParaViewDataCollection
    file_base = OutputData/ParaViewMagnetostaticCoil
  []
[]
