[Mesh]
  type = ExclusiveMFEMMesh
  file = ./coil.gen
  dim = 3
[]

[Problem]
  type = MFEMProblem
[]

[Formulation]
  type = AFormulation
  magnetic_vector_potential_name = magnetic_vector_potential
  magnetic_reluctivity_name = magnetic_reluctivity
  magnetic_permeability_name = magnetic_permeability
  electric_conductivity_name = electrical_conductivity
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
  [electric_potential]
    type = MFEMVariable
    fespace = H1FESpace
  []
[]

[Functions]
  [potential_high]
    type = ParsedFunction
    value = cos(2.0*pi*freq*t)
    vars = 'freq'
    vals = '0.01666667'
  []
  [potential_low]
    type = ParsedFunction
    value = -cos(2.0*pi*freq*t)
    vars = 'freq'
    vals = '0.01666667'
  []
  [tangential_E]
    type = ParsedVectorFunction
    value_x = 0.0
    value_y = 0.0
    value_z = 0.0
  []
[]

[BCs]
  [tangential_E_bc]
    type = MFEMVectorDirichletBC
    variable = dmagnetic_vector_potential_dt
    vector_coefficient = TangentialECoef
    boundary = '1 2 3 4'
  []
  [high_terminal]
    type = MFEMScalarDirichletBC
    variable = electric_potential
    boundary = '1'
    function = potential_high
  []
  [low_terminal]
    type = MFEMScalarDirichletBC
    variable = electric_potential
    boundary = '2'
    function = potential_low
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
    type = MFEMVectorFunctionCoefficient
    function = tangential_E
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

  [OneCoef]
    type = MFEMConstantCoefficient
    value = 1.0e12
  []
[]

[Sources]
  [SourcePotential]
    type = MFEMScalarPotentialSource
    potential = electric_potential
    conductivity = electrical_conductivity
    hcurl_fespace = HCurlFESpace
    h1_fespace = H1FESpace
    solver_max_its = 1000
    block = 1
  []
[]

[Executioner]
  type = Transient
  dt = 0.5
  start_time = 0.0
  end_time = 1.0

  l_tol = 1e-16
  l_max_its = 1000
[]

[Outputs]
  [ParaViewDataCollection]
    type = MFEMParaViewDataCollection
    file_base = OutputData/ParaViewCoil
  []
[]
