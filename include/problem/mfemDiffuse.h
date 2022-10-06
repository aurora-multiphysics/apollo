#pragma once
#include "mfem.hpp"
#include <fstream>
#include <iostream>



int mfemDiffuse(mfem::Mesh& mesh, mfem::GridFunction& x)
{
    std::cout << "lmao" << std::endl;
    int order = 1;

//    3. Define a finite element space on the mesh. Here we use H1 continuous
//       high-order Lagrange finite elements of the given order.
   mfem::H1_FECollection fec(order, mesh.Dimension());
   mfem::FiniteElementSpace fespace(&mesh, &fec);

   // // 4. Extract the list of all the boundary DOFs. These will be marked as
   // //    Dirichlet in order to enforce zero boundary conditions.
   mfem::Array<int> boundary_dofs;
   fespace.GetBoundaryTrueDofs(boundary_dofs);

   // // 5. Define the solution x as a finite element grid function in fespace. Set
   // //    the initial guess to zero, which also sets the boundary conditions.
   x = 0.0;

   // // 6. Set up the linear form b(.) corresponding to the right-hand side.
   mfem::ConstantCoefficient one(1.0);
   mfem::LinearForm b(&fespace);
   b.AddDomainIntegrator(new mfem::DomainLFIntegrator(one));
   b.Assemble();

   // // 7. Set up the bilinear form a(.,.) corresponding to the -Delta operator.
   mfem::BilinearForm a(&fespace);
   a.AddDomainIntegrator(new mfem::DiffusionIntegrator);
   a.Assemble();

   // // 8. Form the linear system A X = B. This includes eliminating boundary
   // //    conditions, applying AMR constraints, and other transformations.
   mfem::SparseMatrix A;
   mfem::Vector B, X;
   a.FormLinearSystem(boundary_dofs, x, b, A, X, B);

   // // 9. Solve the system using PCG with symmetric Gauss-Seidel preconditioner.
   mfem::GSSmoother M(A);
   mfem::PCG(A, M, B, X, 1, 200, 1e-12, 0.0);

   // // 10. Recover the solution x as a grid function and save to file. The output
   // //     can be viewed using GLVis as follows: "glvis -m mesh.mesh -g sol.gf"
   a.RecoverFEMSolution(X, b, x);
   x.Save("sol.gf");
   mesh.Save("meshOut.mesh");

   return 0;
}