//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include <string>

namespace Maxwell
{
const std::string e_field = "E_field";
const std::string e_field_re = "E_real";
const std::string e_field_im = "E_imag";

const std::string d_field = "D_field";
const std::string d_field_re = "D_real";
const std::string d_field_im = "D_imag";

const std::string h_field = "H_field";
const std::string h_field_re = "H_real";
const std::string h_field_im = "H_imag";

const std::string b_field = "B_field";
const std::string b_field_re = "B_real";
const std::string b_field_im = "B_imag";


static const Real mu0 = 1.25663706e-6;
static const Real epsilon0 = 8.85418782e-12;
}
