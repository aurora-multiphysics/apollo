[Mesh]
  type = CoupledMFEMMesh
  file = ./gold/cylinder-hex-q2.e
  dim = 3
[]

[Problem]
  type = MFEMProblem
  use_glvis = false
[]

[Formulation]
  type = EBFormulation
  e_field_name = electric_field
  b_field_name = magnetic_flux_density
  magnetic_reluctivity_name = magnetic_reluctivity
  magnetic_permeability_name = magnetic_permeability
  electric_conductivity_name = electrical_conductivity
[]

[FESpaces]
  [H1FESpace]
    type = MFEMFESpace
    fespace_type = H1
    order = SECOND
  []
  [HCurlFESpace]
    type = MFEMFESpace
    fespace_type = ND
    order = SECOND
  []
  [HDivFESpace]
    type = MFEMFESpace
    fespace_type = RT
    order = FIRST
  []
[]

[AuxVariables]
  [electric_field]
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
  [grad_electric_potential]
    type = MFEMVariable 
    fespace = HCurlFESpace
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
  [cu-ohfc-rx]
    type = ADPiecewiseLinear
    x = '4 5 6 7 8 9 10 12 14 16 18 20 22 24 26 28 30 40 60 80 100 120 140 160 180 200 220 240 260 280 300'
    y = '1.550E-10		1.550E-10		1.551E-10		1.551E-10		1.552E-10		1.553E-10	
         1.555E-10		1.562E-10		1.574E-10		1.593E-10		1.623E-10		1.666E-10	
         1.727E-10		1.809E-10		1.916E-10		2.053E-10		2.224E-10		3.754E-10	
         1.083E-09		2.256E-09		3.686E-09		5.159E-09		6.596E-09		7.992E-09	
         9.356E-09		1.070E-08		1.204E-08		1.337E-08		1.471E-08		1.605E-08	
         1.739E-08'
  []
  # [hts-rx]
  #   type = ADPiecewiseLinear
  #   x = '4 5 6 7 8 9 10 12 14 16 18 20 22 24 26 28 30 40 60 80 100 120 140 160 180 200 220 240 260 280 300'
  #   y = '1.000E-16	1.000E-16	1.000E-16	1.000E-16	1.000E-16	1.000E-16
  #   1.000E-16	1.000E-16	1.000E-16	1.000E-16	1.000E-16	1.000E-16
  #   1.000E-16	1.000E-16	1.000E-16	1.000E-16	1.000E-16	1.000E-16
  #   1.000E-08	1.000E-08	1.000E-08	1.000E-08	1.000E-08	1.000E-08
  #   1.000E-08	1.000E-08	1.000E-08	1.000E-08	1.000E-08	1.000E-08
  #   1.000E-08'
  # []
  # [stainless-steel-rx]
  #   type = ADPiecewiseLinear
  #   x = '4 5 6 7 8 9 10 12 14 16 18 20 22 24 26 28 30 40 60 80 100 120 140 160 180 200 220 240 260 280 300'
  #   y = '5.300E-07	5.300E-07	5.300E-07		5.296E-07		5.290E-07		5.299E-07	
  #        5.300E-07	5.310E-07	5.313E-07		5.326E-07		5.338E-07		5.348E-07	
  # 	     5.358E-07	5.367E-07	5.375E-07		5.382E-07		5.389E-07		5.418E-07	
  # 	     5.503E-07	5.675E-07	5.900E-07		6.158E-07		6.438E-07		6.717E-07	
  # 	     6.973E-07	7.197E-07	7.380E-07		7.492E-07		7.512E-07		7.458E-07	
  # 	     7.347E-07'
  # []
[]

[BCs]
  [tangential_E_bdr]
    type = MFEMVectorDirichletBC
    variable = electric_field
    boundary = '1 2 3'
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

[Sources]
  [SourcePotential]
    type = MFEMScalarPotentialSource
    potential = electric_potential
    grad_potential = grad_electric_potential
    conductivity = electrical_conductivity
    h1_fespace = H1FESpace
    hcurl_fespace = HCurlFESpace
    block = '1 2'
  []
[]

[AuxVariables]
  [temperature]
    family = LAGRANGE
    order = FIRST
    initial_condition = 150.0
  []
  [joule_heating]
    family = MONOMIAL
    order = CONSTANT
    initial_condition = 0.0
  []
[]

[AuxKernels]
  [joule_heating_aux]
    type = MFEMJouleHeatingAux
  []
[]

[Materials]
  [copper]
    type = MFEMConductor
    electrical_conductivity_coeff = CopperEConductivity
    electric_permittivity_coeff = CopperPermittivity
    magnetic_permeability_coeff = CopperPermeability
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
  [CopperEConductivity]
    type = MFEMTemperatureDependentConductivityCoefficient
    temperature_variable = 'temperature'
    resistivity_function = cu-ohfc-rx
  []
  [CopperPermeability]
    type = MFEMConstantCoefficient
    value = 1.25663706e-6
  []
  [CopperPermittivity]
    type = MFEMConstantCoefficient
    value = 0.0
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
  [HighPotential]
    type = MFEMFunctionCoefficient
    function = potential_high
  []
  [LowPotential]
    type = MFEMFunctionCoefficient
    function = potential_low
  []
[]

[Executioner]
  type = Transient
  dt = 0.5
  start_time = 0.0
  end_time = 1.0

  l_tol = 1e-16
  l_max_its = 100
[]

[Outputs]
  [ParaViewDataCollection]
    type = MFEMParaViewDataCollection
    file_base = OutputData/NodalCoupledMFEMVarTest
  []
[]
