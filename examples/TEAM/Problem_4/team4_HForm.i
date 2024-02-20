[Mesh]
  type = ExclusiveMFEMMesh
  file = ./team4_symmetrized.e
  dim = 3
[]

[Problem]
  type = MFEMProblem
[]

[Formulation]
  type = HFormulation
  magnetic_field_name = magnetic_field
  magnetic_permeability_name = magnetic_permeability
  electric_conductivity_name = electrical_conductivity
  electric_resistivity_name = electrical_resistivity

  electric_field_name = total_electric_field
  current_density_name = total_current_density
  joule_heating_density_name = joule_heating_density
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
  [L2FESpace]
    type = MFEMFESpace
    fespace_type = L2
    order = CONSTANT
  []
[]

[AuxVariables]
  [magnetic_field]
    type = MFEMVariable
    fespace = HCurlFESpace
  []
  [source_magnetic_potential]
    type = MFEMVariable
    fespace = H1FESpace
  []
  [source_grad_potential]
    type = MFEMVariable
    fespace = HCurlFESpace
  []
  [total_current_density]
    type = MFEMVariable
    fespace = HDivFESpace
  []
  [total_electric_field]
    type = MFEMVariable
    fespace = HCurlFESpace
  []
  [joule_heating_density]
    type = MFEMVariable
    fespace = L2FESpace
  []
[]

[Functions]
  # Here, externally applied B field = grad psi
  [dpsi_dt]
    type = ParsedFunction
    expression = -z*(B0/tau)*exp(-t/tau)
    symbol_names = 'B0 tau'
    symbol_values = '0.1 0.0119'
  []
[]

[BCs]
  [tangential_H_bc]
    type = MFEMVectorDirichletBC
    variable = dmagnetic_field_dt
    vector_coefficient = TangentialHCoef
    boundary = '1 2 5 6'
  []
  [normal_B_bc]
    type = MFEMScalarDirichletBC
    variable = source_magnetic_potential
    boundary = '1 2'
    coefficient = MagneticPotentialTimeDerivative
  []
[]

[Materials]
  [brick]
    type = MFEMConductor
    electrical_conductivity_coeff = BrickEConductivity
    electric_permittivity_coeff = BrickPermittivity
    magnetic_permeability_coeff = BrickPermeability
    block = 1
  []
  [vacuum]
    type = MFEMConductor
    electrical_conductivity_coeff = VacuumEConductivity
    electric_permittivity_coeff = VacuumPermittivity
    magnetic_permeability_coeff = VacuumPermeability
    block = 2
  []
[]

[VectorCoefficients]
  [TangentialHCoef]
    type = MFEMVectorConstantCoefficient
    value_x = 0.0
    value_y = 0.0
    value_z = 0.0
  []
[]

[Coefficients]
  [BrickEConductivity]
    type = MFEMConstantCoefficient
    value = 2.538e7
  []
  [BrickPermeability]
    type = MFEMConstantCoefficient
    value = 1.25663706e-6 # T m/A
  []
  [BrickPermittivity]
    type = MFEMConstantCoefficient
    value = 0.0
  []

  [VacuumEConductivity]
    type = MFEMConstantCoefficient
    value = 1.0
  []
  [VacuumPermeability]
    type = MFEMConstantCoefficient
    value = 1.25663706e-6 # T m/A
  []
  [VacuumPermittivity]
    type = MFEMConstantCoefficient
    value = 0.0
  []
  [MagneticPotentialTimeDerivative]
    type = MFEMFunctionCoefficient
    function = dpsi_dt
  []
  [OneCoef]
    type = MFEMConstantCoefficient
    value = 1.0
  []
[]

[Sources]
  [SourceField]
    type = MFEMScalarPotentialSource
    source_potential_gridfunction = source_magnetic_potential
    source_grad_potential_gridfunction = source_grad_potential
    conductivity_coef = OneCoef
    h1_fespace = H1FESpace
    hcurl_fespace = HCurlFESpace
    block = '1 2'
  []
[]

[Executioner]
  type = Transient
  dt = 0.001
  start_time = 0.0
  end_time = 0.02

  l_tol = 1e-16
  l_max_its = 500
[]

[Outputs]
  [ParaViewDataCollection]
    type = MFEMParaViewDataCollection
    file_base = OutputData/TEAM4
  []
[]
