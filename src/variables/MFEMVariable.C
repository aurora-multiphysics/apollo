#include "MFEMVariable.h"

registerMooseObject("ApolloApp", MFEMVariable);

InputParameters
MFEMVariable::validParams()
{
  InputParameters params = GeneralUserObject::validParams();

  params.registerBase("MooseVariableBase");

  // Create user-facing 'boundary' input for restricting inheriting object to boundaries

  MooseEnum order(
      "CONSTANT FIRST SECOND THIRD FOURTH FIFTH SIXTH SEVENTH EIGHTH NINTH TENTH ELEVENTH TWELFTH"
      "THIRTEENTH FOURTEENTH FIFTEENTH SIXTEENTH SEVENTEENTH EIGHTTEENTH NINETEENTH TWENTIETH "
      "TWENTYFIRST TWENTYSECOND TWENTYTHIRD TWENTYFOURTH TWENTYFIFTH TWENTYSIXTH TWENTYSEVENTH "
      "TWENTYEIGHTH TWENTYNINTH THIRTIETH THIRTYFIRST THIRTYSECOND THIRTYTHIRD THIRTYFOURTH "
      "THIRTYFIFTH THIRTYSIXTH THIRTYSEVENTH THIRTYEIGHTH THIRTYNINTH FORTIETH FORTYFIRST"
      "FORTYSECOND FORTYTHIRD",
      "FIRST",
      true);
  params.addParam<MooseEnum>("order",
                             order,
                             "Order of the FE shape function to use for this variable (additional"
                             "orders not listed here are allowed,"
                             "depending on the family.");

  params.addRequiredParam<std::string>("fespace_name",
                                       "Specifies name of the FE space to use for this variable.");

  MooseEnum fespace_types("H1 ND RT L2", "H1", true);
  params.addParam<MooseEnum>(
      "fespace_type",
      fespace_types,
      "Specifies the family of FE shape functions (FE space) to use for this variable.");

  params.addRangeCheckedParam<unsigned int>(
      "components", 3, "components>0", "Number of components for an array variable");

  params.addParam<std::vector<Real>>("scaling",
                                     "Specifies a scaling factor to apply to this variable");
  params.addParam<bool>("eigen", false, "True to make this variable an eigen variable");
  params.addParam<bool>("fv", false, "True to make this variable a finite volume variable");
  params.addParam<bool>("array",
                        false,
                        "True to make this variable a array variable regardless of number of "
                        "components. If 'components' > 1, this will automatically be set to"
                        "true.");
  params.addParamNamesToGroup("scaling eigen", "Advanced");

  params.addParam<bool>("use_dual", false, "True to use dual basis for Lagrange multipliers");

  return params;
}

MFEMVariable::MFEMVariable(const InputParameters & parameters)
  : GeneralUserObject(parameters),
    mfem_params(),
    fespace_name(parameters.get<std::string>("fespace_name")),
    fespace_type(parameters.get<MooseEnum>("fespace_type")),
    order(parameters.get<MooseEnum>("order")),
    components(parameters.get<unsigned int>("components"))
{
  mfem_params.SetParam("FESpaceName", fespace_name);
  mfem_params.SetParam("FESpaceType", fespace_type);
  mfem_params.SetParam("order", order);
  mfem_params.SetParam("components", components);
}

MFEMVariable::~MFEMVariable() {}
