#include <iostream>
#include <cstdio>
#include <string>

#include <private/opflowimpl.h>
#include <exago_config.h>

#include "opflow/OpflowTests.hpp"
#include "TestAcopfUtils.hpp"


#if defined(EXAGO_HAVE_RAJA)
#include <umpire/Allocator.hpp>
#include <umpire/ResourceManager.hpp>
#include <RAJA/RAJA.hpp>

#ifdef EXAGO_HAVE_GPU
  using exago_raja_exec = RAJA::cuda_exec<128>;
  using exago_raja_reduce = RAJA::cuda_reduce;
  using exago_raja_atomic = RAJA::cuda_atomic;
  #define RAJA_LAMBDA [=] __device__
#else
  using exago_raja_exec = RAJA::omp_parallel_for_exec;
  using exago_raja_reduce = RAJA::omp_reduce;
  using exago_raja_atomic = RAJA::omp_atomic;
  #define RAJA_LAMBDA [=]
#endif

#endif

/**
 * @brief Converts an array xin in natural ordering to an array xout in
 * sparse-dense ordering
 */
void naturaltospdense(const double *xin,double *xout,int *idxn2sd_map,int nx)
{
  int i;

  for(i=0; i < nx; i++) {
    xout[idxn2sd_map[i]] = xin[i];
  }
}

/**
 * @brief Converts an array xin in sparse dense ordering to an array xout in
 * natural ordering
 */
void spdensetonatural(const double *xin,double *xout,int *idxn2sd_map,int nx)
{
  int i;

  for(i=0; i < nx; i++) {
    xout[i] = xin[idxn2sd_map[i]];
  }
}

/**
 * @brief Unit test driver for ACOPF models
 * @see opflow/OpflowTests.hpp for kernels tested by this driver
 *
 * You can pass several options to the TestAcopf executatable through the command line (implemented using PETSc options):
 *
 *    ~ -netfile <data_file> : Specifies the input data file to test against. Default value is `/<exago_dir>/datafiles/case9/case9mod.m`.
 *                             See directory datafiles for other potential inputs.
 *
 *    ~ -gen_test_data       : If used, generates an answer key using IPOPT to test against.
 *                             If not used, uses existing answer keys located in `/<exago_dir>/datafiles/test_validation`.
 *
 *    ~ -write_test_data     : If used, generates test data, and then writes out the results to `/<install_dir>/tests/datafiles/test_validation/<data_file>/`.
 *                             In order to save generated results, you should copy them into `/<exago_dir>/datafiles/test_validation/<data_file>/` and add them to git.
 *      
 */
int main(int argc, char** argv)
{
  const bool     isTestOpflowModelPBPOL     = false;
#if defined(EXAGO_HAVE_RAJA)
  const bool     isTestOpflowModelPBPOLRAJAHIOP = true;
  const bool     isTestOpflowModelPBPOLHIOP = false;
#else
  const bool     isTestOpflowModelPBPOLRAJAHIOP = false;
  const bool     isTestOpflowModelPBPOLHIOP = true;
#endif
  PetscErrorCode ierr;
  PetscBool      flg, gen_test_data, write_test_data;
  bool           ineq_present = false;
  Vec            X, Xl, Xu, G, Gl, Gu, grad, Lambda;
  Mat            Jeq, Jineq, Hess;
  int            fail;
  PetscLogStage  stages[2];
  double         obj_value, obj_factor;
  char           file_c_str[PETSC_MAX_PATH_LEN];
  char           gen_test_data_c_str[PETSC_MAX_PATH_LEN];
  char           write_test_data_c_str[PETSC_MAX_PATH_LEN];
  std::string file;
  std::string options_pathname(EXAGO_OPTIONS_DIR);
  std::string filename("/opflowoptions");
  std::cout << options_pathname << "\n";
  std::cout << filename << "\n";
  options_pathname += filename;
  std::cout << options_pathname << "\n";

  char help[] = "Unit tests for ACOPF\n";
  PetscInitialize(&argc, &argv, options_pathname.c_str(), help);

  /* Get network data file from command line */
  ierr = PetscOptionsGetString(NULL,NULL,"-netfile",file_c_str,PETSC_MAX_PATH_LEN,&flg);CHKERRQ(ierr);

  if(!flg){
    file = "../datafiles/case9/case9mod.m";
  } 
  else{
    file = file_c_str;
  }

  std::cout << file << std::endl;

  /* Place to store/read reference solutions */
  std::string file_path = std::string(EXAGO_OPTIONS_DIR) +
                          std::string("/../tests/datafiles/test_validation/") +
                          getFileName(file) + "/";

  ierr = PetscOptionsGetString(NULL, NULL, "-gen_test_data", gen_test_data_c_str,PETSC_MAX_PATH_LEN,&gen_test_data);CHKERRQ(ierr);
  ierr = PetscOptionsGetString(NULL, NULL, "-write_test_data", write_test_data_c_str,PETSC_MAX_PATH_LEN,&write_test_data);CHKERRQ(ierr);

  ierr = PetscLogStageRegister("Solution key",&stages[0]);CHKERRQ(ierr);
  ierr = PetscLogStageRegister("Test stage",&stages[1]);CHKERRQ(ierr);

  ierr = PetscLogStagePush(stages[0]);

  if(gen_test_data || write_test_data)
  {
    std::cout << "Generating test_validation data from existing model." << std::endl;

    OPFLOW opflow;

    /* Create reference opflow */
    ierr = OPFLOWCreate(PETSC_COMM_WORLD,&opflow);CHKERRQ(ierr);

    /* Read Network data */
    ierr = OPFLOWReadMatPowerData(opflow,file.c_str());CHKERRQ(ierr);

    /* Set opflow model type to power balance polar (no component assembly) */
    ierr = OPFLOWSetModel(opflow,OPFLOWMODEL_PBPOL);CHKERRQ(ierr);

    /* Set opflow solver type to IPOPT */
    ierr = OPFLOWSetSolver(opflow,OPFLOWSOLVER_IPOPT);CHKERRQ(ierr);

    /* Solve OPFLOW to get the reference solution */
    ierr = OPFLOWSolve(opflow);

    /* Get the reference solution */
    ierr = OPFLOWGetSolution(opflow,&X);CHKERRQ(ierr);

    /* Get reference variable bounds */
    ierr = OPFLOWGetVariableBounds(opflow,&Xl,&Xu);CHKERRQ(ierr);

    /* Get reference objective value */
    ierr = OPFLOWComputeObjective(opflow,X,&obj_value);CHKERRQ(ierr);


    /* Get reference value for the objective radient */
    ierr = VecDuplicate(X,&grad);CHKERRQ(ierr);
    ierr = OPFLOWComputeGradient(opflow,X,grad);CHKERRQ(ierr);


    /* Get reference constraint residuals */
    ierr = OPFLOWGetConstraints(opflow,&G);CHKERRQ(ierr);
    ierr = OPFLOWComputeConstraints(opflow,X,G);CHKERRQ(ierr);


    /* Get reference constraint bounds */
    ierr = OPFLOWGetConstraintBounds(opflow,&Gl,&Gu);CHKERRQ(ierr);


    /* Get reference equality constraint and inequality constraint Jacobian */
    ierr = OPFLOWGetConstraintJacobian(opflow,&Jeq,&Jineq);CHKERRQ(ierr);
    ierr = OPFLOWComputeConstraintJacobian(opflow,X,Jeq,Jineq);CHKERRQ(ierr);
    ierr = OPFLOWGetConstraintMultipliers(opflow,&Lambda);CHKERRQ(ierr);
    ierr = OPFLOWGetHessian(opflow,&Hess,&obj_factor);CHKERRQ(ierr);
    ierr = OPFLOWComputeHessian(opflow,X,Lambda,obj_factor,Hess);CHKERRQ(ierr);

    if(write_test_data)
    {
      std::cout << "Writing answer keys to " << file_path << std::endl;

      validate_directory(file_path);
      saveToFile(X, file_path + "X_valid.txt");
      saveToFile(Xl, file_path + "Xl_valid.txt");
      saveToFile(Xu, file_path + "Xu_valid.txt");
      saveToFile(obj_value, file_path + "obj_value_valid.txt");
      saveToFile(grad, file_path + "grad_valid.txt");
      saveToFile(G, file_path + "G_valid.txt");
      saveToFile(Gl, file_path + "Gl_valid.txt");
      saveToFile(Gu, file_path + "Gu_valid.txt");
      saveToFile(Jeq, file_path + "Jeq_valid.bin");
      if (opflow->nconineq)
      {
        ineq_present = true;
        saveToFile(Jineq, file_path + "Jineq_valid.bin");
      }
      saveToFile(Lambda, file_path + "Lambda_valid.txt");
      saveToFile(Hess, file_path + "Hess_valid.bin");
      saveToFile(obj_factor, file_path + "obj_factor_valid.txt");
    }
  }
  else
  {
    readFromFile(&Xl, file_path + "Xl_valid.txt");
    readFromFile(&Xu, file_path + "Xu_valid.txt");
    readFromFile(&X, file_path + "X_valid.txt");
    readFromFile(&Gl, file_path + "Gl_valid.txt");
    readFromFile(&Gu, file_path + "Gu_valid.txt");
    readFromFile(&G, file_path + "G_valid.txt");
    readFromFile(&grad, file_path + "grad_valid.txt");
    readFromFile(&obj_value, file_path + "obj_value_valid.txt");
    readFromFile(&Lambda, file_path + "Lambda_valid.txt");
    readFromFile(&Jeq, file_path + "Jeq_valid.bin");
    ineq_present = readFromFile(&Jineq, file_path + "Jineq_valid.bin");
    readFromFile(&Hess, file_path + "Hess_valid.bin");
    readFromFile(&obj_factor, file_path + "obj_factor_valid.txt");
  }

  ierr = PetscLogStagePop();CHKERRQ(ierr);

  ierr = PetscLogStagePush(stages[1]);CHKERRQ(ierr);

  if(isTestOpflowModelPBPOLHIOP)
  {
    OPFLOW opflowtest;
    exago::tests::TestOpflow test;

    std::cout << "\nTesting custom power balance model in polar coordinates for HIOP"
              << "(componentwise assembly) ... \n";

    // Create optimal power flow model
    ierr = OPFLOWCreate(PETSC_COMM_WORLD,&opflowtest);CHKERRQ(ierr);

    /* Read Network data */
    ierr = OPFLOWReadMatPowerData(opflowtest,file.c_str());CHKERRQ(ierr);

    /* Set opflow model type to power balance polar2 (component assembly) */
    ierr = OPFLOWSetModel(opflowtest,OPFLOWMODEL_PBPOLHIOP);CHKERRQ(ierr);

    /* Set solver to HIOP */
    ierr = OPFLOWSetSolver(opflowtest,OPFLOWSOLVER_HIOP);CHKERRQ(ierr);

    /* Set up */
    ierr = OPFLOWSetUp(opflowtest);CHKERRQ(ierr);

    int nx,nconeq,nconineq,*idxn2sd_map;
    ierr = OPFLOWGetSizes(opflowtest,&nx,&nconeq,&nconineq);CHKERRQ(ierr);
    ierr = OPFLOWGetVariableOrdering(opflowtest,&idxn2sd_map);CHKERRQ(ierr);

    double *x_vec, *xl_vec, *xu_vec, *grad_vec, \
           *x_ref, *xl_ref, *xu_ref, *grad_ref, *g_ref, *gl_ref, *gu_ref, *lambda_ref;

    ierr = PetscMalloc1(nx,&x_ref);CHKERRQ(ierr);
    ierr = PetscMalloc1(nx,&xl_ref);CHKERRQ(ierr);
    ierr = PetscMalloc1(nx,&xu_ref);CHKERRQ(ierr);
    ierr = PetscMalloc1(nx,&grad_ref);CHKERRQ(ierr);

    ierr = VecGetArray(X,&x_vec);CHKERRQ(ierr);
    ierr = VecGetArray(Xl,&xl_vec);CHKERRQ(ierr);
    ierr = VecGetArray(Xu,&xu_vec);CHKERRQ(ierr);
    ierr = VecGetArray(grad,&grad_vec);CHKERRQ(ierr);


    /**
     * @note IPOPT passes the scaled Lagrangian multipliers for Hessian 
     * calculation. The scaling factor is the Hessian objective factor it 
     * uses in the Hessian calculation, so we need to scale Lambda. Note 
     * that obj_factor = 1.0 except when the solver is IPOPT which sets 
     * obj_factor
     */
    ierr = VecScale(Lambda,obj_factor);CHKERRQ(ierr);

    ierr = VecGetArray(Lambda, &lambda_ref);CHKERRQ(ierr);

    ierr = VecGetArray(G,&g_ref);CHKERRQ(ierr);
    ierr = VecGetArray(Gl,&gl_ref);CHKERRQ(ierr);
    ierr = VecGetArray(Gu,&gu_ref);CHKERRQ(ierr);

    /* Convert from natural to sparse dense ordering */
    naturaltospdense(x_vec,x_ref,idxn2sd_map,nx);CHKERRQ(ierr);
    naturaltospdense(xl_vec,xl_ref,idxn2sd_map,nx);CHKERRQ(ierr);
    naturaltospdense(xu_vec,xu_ref,idxn2sd_map,nx);CHKERRQ(ierr);
    naturaltospdense(grad_vec,grad_ref,idxn2sd_map,nx);CHKERRQ(ierr);
    /* _ref pointers are now in sparse-dense ordering */
    fail = 0;
    fail += test.computeVariableBounds(opflowtest,xl_ref,xu_ref);
    fail += test.computeObjective(opflowtest,x_ref,obj_value);
    fail += test.computeGradient(opflowtest,x_ref,grad_ref);
    fail += test.computeConstraints(opflowtest,x_ref,g_ref);
    fail += test.computeConstraintBounds(opflowtest,gl_ref,gu_ref);
    fail += test.computeConstraintJacobian(opflowtest, x_ref, Jeq, Jineq);
    fail += test.computeHessian(opflowtest, x_ref, lambda_ref, obj_factor, Hess);

    ierr = PetscFree(x_ref);CHKERRQ(ierr);
    ierr = PetscFree(xl_ref);CHKERRQ(ierr);
    ierr = PetscFree(xu_ref);CHKERRQ(ierr);
    ierr = PetscFree(grad_ref);CHKERRQ(ierr);

    ierr = VecRestoreArray(X,&x_vec);CHKERRQ(ierr);
    ierr = VecRestoreArray(Xl,&xl_vec);CHKERRQ(ierr);
    ierr = VecRestoreArray(Xu,&xu_vec);CHKERRQ(ierr);
    ierr = VecRestoreArray(grad,&grad_vec);CHKERRQ(ierr);
    ierr = VecRestoreArray(G,&g_ref);CHKERRQ(ierr);
    ierr = VecRestoreArray(Gl,&gl_ref);CHKERRQ(ierr);
    ierr = VecRestoreArray(Gu,&gu_ref);CHKERRQ(ierr);
    ierr = VecRestoreArray(Lambda,&lambda_ref);CHKERRQ(ierr);

    ierr = VecScale(Lambda,1/obj_factor);CHKERRQ(ierr);
    ierr = OPFLOWDestroy(&opflowtest);CHKERRQ(ierr);
  }

#if defined(EXAGO_HAVE_RAJA)
  if(isTestOpflowModelPBPOLRAJAHIOP)
  {
    OPFLOW opflowtest;
    exago::tests::TestOpflow test;

    std::cout << "\nTesting custom power balance model in polar coordinates for HIOP using RAJA"
              << "(PBPOLHIOPRAJA) ... \n";

    // Create optimal power flow model
    ierr = OPFLOWCreate(PETSC_COMM_WORLD,&opflowtest);CHKERRQ(ierr);

    /* Read Network data */
    ierr = OPFLOWReadMatPowerData(opflowtest,file.c_str());CHKERRQ(ierr);

    /* Set opflow model type to custom model for hiop using RAJA */
    ierr = OPFLOWSetModel(opflowtest,OPFLOWMODEL_PBPOLRAJAHIOP);CHKERRQ(ierr);

    /* Set solver to HIOP */
    ierr = OPFLOWSetSolver(opflowtest,OPFLOWSOLVER_HIOP);CHKERRQ(ierr);

    /* Set up */
    ierr = OPFLOWSetUp(opflowtest);CHKERRQ(ierr);

    int nx,nconeq,nconineq,*idxn2sd_map;
    ierr = OPFLOWGetSizes(opflowtest,&nx,&nconeq,&nconineq);CHKERRQ(ierr);
    ierr = OPFLOWGetVariableOrdering(opflowtest,&idxn2sd_map);CHKERRQ(ierr);

    std::cout << "nx = " << nx << std::endl  << "nconeq = " << nconeq << std::endl
              << "nconineq = " << nconineq << std::endl;

    double *x_vec, *xl_vec, *xu_vec, *grad_vec, \
      *x_ref, *xl_ref, *xu_ref, *grad_ref, *g_ref, *gl_ref, *gu_ref,*lambda_ref;

    ierr = PetscMalloc1(nx,&x_ref);CHKERRQ(ierr);
    ierr = PetscMalloc1(nx,&xl_ref);CHKERRQ(ierr);
    ierr = PetscMalloc1(nx,&xu_ref);CHKERRQ(ierr);
    ierr = PetscMalloc1(nx,&grad_ref);CHKERRQ(ierr);

    // Petsc Documentation mentions "You MUST call VecRestoreArray() when you no longer need access to the array."...
    ierr = VecGetArray(X,&x_vec);CHKERRQ(ierr);
    ierr = VecGetArray(Xl,&xl_vec);CHKERRQ(ierr);
    ierr = VecGetArray(Xu,&xu_vec);CHKERRQ(ierr);
    ierr = VecGetArray(grad,&grad_vec);CHKERRQ(ierr);

    /**
     * @note IPOPT passes the scaled Lagrangian multipliers for Hessian 
     * calculation. The scaling factor is the Hessian objective factor it 
     * uses in the Hessian calculation, so we need to scale Lambda. Note 
     * that obj_factor = 1.0 except when the solver is IPOPT which sets 
     * obj_factor
     */
    ierr = VecScale(Lambda,obj_factor);CHKERRQ(ierr);

    ierr = VecGetArray(Lambda,&lambda_ref);CHKERRQ(ierr);

    ierr = VecGetArray(G,&g_ref);CHKERRQ(ierr);
    ierr = VecGetArray(Gl,&gl_ref);CHKERRQ(ierr);
    ierr = VecGetArray(Gu,&gu_ref);CHKERRQ(ierr);

    /* Convert from natural to sparse dense ordering */
    naturaltospdense(x_vec,x_ref,idxn2sd_map,nx);CHKERRQ(ierr);
    naturaltospdense(xl_vec,xl_ref,idxn2sd_map,nx);CHKERRQ(ierr);
    naturaltospdense(xu_vec,xu_ref,idxn2sd_map,nx);CHKERRQ(ierr);
    naturaltospdense(grad_vec,grad_ref,idxn2sd_map,nx);CHKERRQ(ierr);
    /* _ref pointers are now in sparse-dense ordering */
    
    // Get resource manager instance
    auto& resmgr = umpire::ResourceManager::getInstance();

    // Get Allocators
    umpire::Allocator h_allocator = resmgr.getAllocator("HOST");
    umpire::Allocator d_allocator = resmgr.getAllocator("DEVICE");

    // Register array xref and lambdaref with umpire
    umpire::util::AllocationRecord record_x{x_ref,sizeof(double)*nx,h_allocator.getAllocationStrategy()};
    resmgr.registerAllocation(x_ref,record_x);

    umpire::util::AllocationRecord record_lam{lambda_ref,sizeof(double)*(nconeq+nconineq),h_allocator.getAllocationStrategy()};
    resmgr.registerAllocation(lambda_ref,record_lam);

    // Allocate and copy xref to device
    double *x_ref_dev = static_cast<double*>(d_allocator.allocate(nx*sizeof(double)));
    resmgr.copy(x_ref_dev,x_ref);

    // Allocate and copy xref to device
    double *lambda_ref_dev = static_cast<double*>(d_allocator.allocate((nconeq+nconineq)*sizeof(double)));
    resmgr.copy(lambda_ref_dev,lambda_ref);
					    
    // Tests
    fail += test.computeVariableBounds(opflowtest,xl_ref,xu_ref,resmgr);
    fail += test.computeObjective(opflowtest,x_ref_dev,obj_value);
    fail += test.computeGradient(opflowtest,x_ref_dev,grad_ref,resmgr);
    fail += test.computeConstraints(opflowtest,x_ref_dev,g_ref,resmgr);
    fail += test.computeConstraintBounds(opflowtest,gl_ref,gu_ref,resmgr);
    fail += test.computeConstraintJacobian(opflowtest,x_ref_dev,Jeq,Jineq,resmgr);

    int    nxdense = 2*opflowtest->ps->nbus;
    double *data_hess_host,*data_hess_dev;
    double **hess_dense,**hess_dense_dev;

    data_hess_host = static_cast<double*>(h_allocator.allocate(nxdense*nxdense*sizeof(double)));
    data_hess_dev  = static_cast<double*>(d_allocator.allocate(nxdense*nxdense*sizeof(double)));

    hess_dense     = static_cast<double**>(h_allocator.allocate(nxdense*sizeof(double*)));
    hess_dense_dev = static_cast<double**>(d_allocator.allocate(nxdense*sizeof(double*)));

    for(int i=0; i < nxdense; i++) {
      hess_dense[i] = data_hess_host + i*nxdense;
    }

    RAJA::forall<exago_raja_exec>(RAJA::RangeSegment(0, nxdense),
      RAJA_LAMBDA (RAJA::Index_type i)
      {
        hess_dense_dev[i] = data_hess_dev + i*nxdense;
      }
    );

    fail += test.computeHessian(opflowtest,x_ref_dev,lambda_ref_dev,obj_factor,Hess,resmgr,hess_dense,hess_dense_dev,data_hess_host,data_hess_dev);

    d_allocator.deallocate(x_ref_dev);
    d_allocator.deallocate(lambda_ref_dev);

    h_allocator.deallocate(hess_dense);
    d_allocator.deallocate(hess_dense_dev);

    h_allocator.deallocate(data_hess_host);
    d_allocator.deallocate(data_hess_dev);


    ierr = PetscFree(x_ref);CHKERRQ(ierr);
    ierr = PetscFree(xl_ref);CHKERRQ(ierr);
    ierr = PetscFree(xu_ref);CHKERRQ(ierr);
    ierr = PetscFree(grad_ref);CHKERRQ(ierr);

    ierr = VecRestoreArray(X,&x_vec);CHKERRQ(ierr);
    ierr = VecRestoreArray(Xl,&xl_vec);CHKERRQ(ierr);
    ierr = VecRestoreArray(Xu,&xu_vec);CHKERRQ(ierr);
    ierr = VecRestoreArray(grad,&grad_vec);CHKERRQ(ierr);
    ierr = VecRestoreArray(Lambda,&lambda_ref);CHKERRQ(ierr);

    ierr = VecRestoreArray(G,&g_ref);CHKERRQ(ierr);
    ierr = VecRestoreArray(Gl,&gl_ref);CHKERRQ(ierr);
    ierr = VecRestoreArray(Gu,&gu_ref);CHKERRQ(ierr);


    ierr = VecScale(Lambda,1/obj_factor);CHKERRQ(ierr);
    ierr = OPFLOWDestroy(&opflowtest);CHKERRQ(ierr);
  }
#endif

  if (isTestOpflowModelPBPOL)
  {
    OPFLOW                   opflowtest;
    exago::tests::TestOpflow test;

    std::cout << "\nTesting power balance model in polar coordinates "
              << " ... \n";


    /* Set up test opflow */
    ierr = OPFLOWCreate(PETSC_COMM_WORLD,&opflowtest);CHKERRQ(ierr);
    ierr = OPFLOWReadMatPowerData(opflowtest,file.c_str());CHKERRQ(ierr);
    ierr = OPFLOWSetSolver(opflowtest,OPFLOWSOLVER_IPOPT);CHKERRQ(ierr);
    ierr = OPFLOWSetModel(opflowtest,OPFLOWMODEL_PBPOL);CHKERRQ(ierr);
    ierr = OPFLOWSetUp(opflowtest);CHKERRQ(ierr);

    fail += test.computeVariableBounds(opflowtest,Xl,Xu);
    fail += test.computeObjective(opflowtest,X,obj_value);
    fail += test.computeGradient(opflowtest,X,grad);
    fail += test.computeConstraints(opflowtest,X,G);
    fail += test.computeConstraintBounds(opflowtest,Gl,Gu);
    fail += test.computeConstraintJacobian(opflowtest,X,Jeq,Jineq);
    fail += test.computeHessian(opflowtest,X,Lambda,obj_factor,Hess);

    ierr = OPFLOWDestroy(&opflowtest);CHKERRQ(ierr);
  }

  /* Destroy OPFLOW objects */
  ierr = VecDestroy(&G);CHKERRQ(ierr);
  ierr = VecDestroy(&Gl);CHKERRQ(ierr);
  ierr = VecDestroy(&Gu);CHKERRQ(ierr);
  ierr = VecDestroy(&X);CHKERRQ(ierr);
  ierr = VecDestroy(&Xl);CHKERRQ(ierr);
  ierr = VecDestroy(&Xu);CHKERRQ(ierr);
  ierr = VecDestroy(&grad);CHKERRQ(ierr);
  ierr = VecDestroy(&Lambda);CHKERRQ(ierr);
  ierr = MatDestroy(&Jeq);CHKERRQ(ierr);
  if (ineq_present)
    ierr = MatDestroy(&Jineq);CHKERRQ(ierr);
  ierr = MatDestroy(&Hess);CHKERRQ(ierr);
  PetscFinalize();
}