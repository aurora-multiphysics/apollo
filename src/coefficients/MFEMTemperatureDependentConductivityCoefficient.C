#include "MFEMTemperatureDependentConductivityCoefficient.h"

registerMooseObject("ApolloApp", MFEMTemperatureDependentConductivityCoefficient);

InputParameters
MFEMTemperatureDependentConductivityCoefficient::validParams()
{
  InputParameters params = MFEMCoefficient::validParams();
  params.addParam<FunctionName>(
      "resistivity_function", 1.0, "The resistivity of the material as a function of temperature");
  params.addParam<std::string>("temperature_variable",
                               "The MFEMVariable describing the temperature of the material.");
  return params;
}

MFEMTemperatureDependentConductivityCoefficient::MFEMTemperatureDependentConductivityCoefficient(
    const InputParameters & parameters)
  : MFEMCoefficient(parameters),
    hephaestus::CoupledCoefficient(hephaestus::InputParameters(
        {{"CoupledVariableName", getParam<std::string>("temperature_variable")}})),
    _func(getFunction("resistivity_function"))
{
}

double
MFEMTemperatureDependentConductivityCoefficient::Eval(mfem::ElementTransformation & trans,
                                                      const mfem::IntegrationPoint & ip)
{
  auto gf_value{hephaestus::CoupledCoefficient::Eval(trans, ip)};
  return 1.0 / _func.value(gf_value);
};

MFEMTemperatureDependentConductivityCoefficient::~MFEMTemperatureDependentConductivityCoefficient()
{
}
