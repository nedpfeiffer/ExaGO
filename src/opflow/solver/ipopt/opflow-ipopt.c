#if defined(SCOPFLOW_HAVE_IPOPT)

#include <private/opflowimpl.h>
#include "opflow-ipopt.h"

static int CCMatrixToMatrixMarketValuesOnly(CCMatrix ccmatrix,PetscInt nz,PetscScalar *values)
{
  PetscErrorCode ierr;

  ierr = PetscMemcpy(values,ccmatrix->values,nz*sizeof(PetscScalar));

  return 0;
}

static int CCMatrixToMatrixMarketLocationsOnly(CCMatrix ccmatrix,PetscInt ncol,PetscInt *iRow,PetscInt *jCol,PetscInt roffset,PetscInt coffset,PetscInt nval)
{
  PetscInt *rowidx;
  PetscInt *colptr;
  PetscInt j,k,ctr=0;
  
  rowidx = ccmatrix->rowidx;
  colptr = ccmatrix->colptr;

  /* Copy from compressed column to (row,col,val) format */
  for(j=0; j < ncol; j++) {
    for(k=colptr[j]; k < colptr[j+1]; k++) {
      iRow[ctr] = rowidx[k] + roffset;
      jCol[ctr] = j + coffset;
      ctr++;
    }
  }
  if(ctr != nval) SETERRQ2(PETSC_COMM_SELF,PETSC_ERR_SUP,"Incorrect number of entries ctr = %d given = %d\n",ctr,nval);

  return 0;
}

/* IPOPT callback functions */
Bool eval_opflow_f(PetscInt n, PetscScalar* x, Bool new_x,
            PetscScalar* obj_value, UserDataPtr user_data)
{
  PetscErrorCode ierr;
  OPFLOW  opflow=(OPFLOW)user_data;

  *obj_value = 0.0;
  ierr = VecPlaceArray(opflow->X,x);CHKERRQ(ierr);
  ierr = (*opflow->formops.computeobjective)(opflow,opflow->X,obj_value);CHKERRQ(ierr);
  ierr = VecResetArray(opflow->X);CHKERRQ(ierr);
				
  return TRUE;
}

Bool eval_opflow_grad_f(PetscInt n, PetscScalar* x, Bool new_x,
                 PetscScalar* grad_f, UserDataPtr user_data)
{
  PetscErrorCode ierr;
  OPFLOW  opflow=(OPFLOW)user_data;

  ierr = VecPlaceArray(opflow->X,x);CHKERRQ(ierr);
  ierr = VecPlaceArray(opflow->gradobj,grad_f);CHKERRQ(ierr);
  ierr = (*opflow->formops.computegradient)(opflow,opflow->X,opflow->gradobj);CHKERRQ(ierr);
  ierr = VecResetArray(opflow->X);CHKERRQ(ierr);
  ierr = VecResetArray(opflow->gradobj);CHKERRQ(ierr);

  return TRUE;
}

Bool eval_opflow_g(PetscInt n, PetscScalar* x, Bool new_x,
            PetscInt m, PetscScalar* g, UserDataPtr user_data)
{
  PetscErrorCode ierr;
  OPFLOW         opflow=(OPFLOW)user_data;

  ierr = VecPlaceArray(opflow->X,x);CHKERRQ(ierr);
  ierr = VecPlaceArray(opflow->G,g);CHKERRQ(ierr);
  ierr = (*opflow->formops.computeconstraints)(opflow,opflow->X,opflow->G);CHKERRQ(ierr);
  ierr = VecResetArray(opflow->X);CHKERRQ(ierr);
  ierr = VecResetArray(opflow->G);CHKERRQ(ierr);

  return TRUE;
}

Bool eval_opflow_jac_g(PetscInt n, PetscScalar *x, Bool new_x,
                PetscInt m, PetscInt nele_jac,
                PetscInt *iRow, PetscInt *jCol, PetscScalar *values,
                UserDataPtr user_data)
{

  PetscErrorCode ierr;
  OPFLOW         opflow=(OPFLOW)user_data;
  OPFLOWSolver_IPOPT ipopt=(OPFLOWSolver_IPOPT)opflow->solver;
  PetscInt       *iRowstart = iRow,*jColstart=jCol;
  PetscInt       roffset,coffset;

  if(values == NULL) {
    /* Set locations only */
    roffset = opflow->nconeq;
    coffset = 0;

    CCMatrixToMatrixMarketLocationsOnly(ipopt->jac_ge,opflow->nx,iRowstart,jColstart,roffset,coffset,ipopt->nnz_jac_ge);

    /* Increment iRow,jCol pointers */
    iRowstart += ipopt->nnz_jac_ge;
    jColstart += ipopt->nnz_jac_ge;

    if(opflow->nconineq) {
      roffset = opflow->nconeq;
      
      CCMatrixToMatrixMarketLocationsOnly(ipopt->jac_gi,opflow->nx,iRowstart,jColstart,roffset,coffset,ipopt->nnz_jac_gi);
    }
  } else {

  }

  return TRUE;
}

Bool eval_opflow_h(PetscInt n, PetscScalar *x, Bool new_x, PetscScalar obj_factor,
            PetscInt m, PetscScalar *lambda, Bool new_lambda,
            PetscInt nele_hess, PetscInt *iRow, PetscInt *jCol,
            PetscScalar *values, UserDataPtr user_data)
{
  return 1;
}

PetscErrorCode OPFLOWSolverSolve_IPOPT(OPFLOW opflow)
{
  PetscErrorCode     ierr;
  OPFLOWSolver_IPOPT ipopt=opflow->solver;
  Mat_SeqAIJ         *aij;
  PetscScalar        *x,*xl,*xu,*gl,*gu;

  PetscFunctionBegin;

  /* Compute nonzeros for the Jacobian */
  /* Equality constraint Jacobian */
  ierr = (*opflow->formops.computeequalityconstraintjacobian)(opflow,opflow->X,opflow->Jac_Ge);CHKERRQ(ierr);
  /* Transpose the matrix to convert it to column compressed sparse format */
  ierr = MatTranspose(opflow->Jac_Ge,MAT_INITIAL_MATRIX,&ipopt->Jac_GeT);CHKERRQ(ierr);
  ierr = MatSetOption(ipopt->Jac_GeT,MAT_NEW_NONZERO_LOCATION_ERR,PETSC_TRUE);CHKERRQ(ierr);
  aij = (Mat_SeqAIJ*)ipopt->Jac_GeT->data;
  ipopt->nnz_jac_ge = aij->nz;

  /* Create ccmatrix object for equality constrained Jacobian */
  ierr = PetscCalloc1(1,&ipopt->jac_ge);CHKERRQ(ierr);
  ierr = PetscMalloc1(opflow->nx+1,&ipopt->jac_ge->colptr);CHKERRQ(ierr);
  ierr = PetscMalloc1(ipopt->nnz_jac_ge,&ipopt->jac_ge->rowidx);CHKERRQ(ierr);
  ierr = PetscMalloc1(ipopt->nnz_jac_ge,&ipopt->jac_ge->values);CHKERRQ(ierr);

  if(opflow->nconineq) {
    ierr = (*opflow->formops.computeinequalityconstraintjacobian)(opflow,opflow->X,opflow->Jac_Gi);CHKERRQ(ierr);
    /* Transpose the matrix to convert it to column compressed sparse format */
    ierr = MatTranspose(opflow->Jac_Gi,MAT_INITIAL_MATRIX,&ipopt->Jac_GiT);CHKERRQ(ierr);
    ierr = MatSetOption(ipopt->Jac_GiT,MAT_NEW_NONZERO_LOCATION_ERR,PETSC_TRUE);CHKERRQ(ierr);
    aij = (Mat_SeqAIJ*)ipopt->Jac_GiT->data;
    ipopt->nnz_jac_gi = aij->nz;

  /* Create ccmatrix object for inequality constrained Jacobian */
  ierr = PetscCalloc1(1,&ipopt->jac_gi);CHKERRQ(ierr);
  ierr = PetscMalloc1(opflow->nx+1,&ipopt->jac_gi->colptr);CHKERRQ(ierr);
  ierr = PetscMalloc1(ipopt->nnz_jac_ge,&ipopt->jac_gi->rowidx);CHKERRQ(ierr);
  ierr = PetscMalloc1(ipopt->nnz_jac_gi,&ipopt->jac_gi->values);CHKERRQ(ierr);

  }   
  ipopt->nnz_jac_g = ipopt->nnz_jac_ge + ipopt->nnz_jac_gi;

  /* Compute non-zeros for Hessian */
  ierr = (*opflow->formops.computehessian)(opflow,opflow->X,opflow->Lambda,opflow->Hes);CHKERRQ(ierr);
  /* Since the Hessian is symmetric, we don't need to convert it to column compressed sparse format */
  aij = (Mat_SeqAIJ*)opflow->Hes;
  ipopt->nnz_hes = aij->nz;

  /* Create ccmatrix object for hessian */
  ierr = PetscCalloc1(1,&ipopt->hes);CHKERRQ(ierr);
  ierr = PetscMalloc1(opflow->nx+1,&ipopt->hes->colptr);CHKERRQ(ierr);
  ierr = PetscMalloc1(ipopt->nnz_hes,&ipopt->hes->rowidx);CHKERRQ(ierr);
  ierr = PetscMalloc1(ipopt->nnz_hes,&ipopt->hes->values);CHKERRQ(ierr);

  /* Create IPOPT solver instance */
  ierr = VecGetArray(opflow->Xl,&xl);CHKERRQ(ierr);
  ierr = VecGetArray(opflow->Xu,&xu);CHKERRQ(ierr);
  ierr = VecGetArray(opflow->Gl,&gl);CHKERRQ(ierr);
  ierr = VecGetArray(opflow->Gu,&gu);CHKERRQ(ierr);

  /* Create IPOPT problem */
  ipopt->nlp = CreateIpoptProblem(opflow->nx,xl,xu,opflow->ncon,gl,gu,ipopt->nnz_jac_g,ipopt->nnz_hes,0,&eval_opflow_f,
				   &eval_opflow_g,&eval_opflow_grad_f,
				   &eval_opflow_jac_g,&eval_opflow_h);

  ierr = VecRestoreArray(opflow->Xl,&xl);CHKERRQ(ierr);
  ierr = VecRestoreArray(opflow->Xu,&xu);CHKERRQ(ierr);
  ierr = VecRestoreArray(opflow->Gl,&gl);CHKERRQ(ierr);
  ierr = VecRestoreArray(opflow->Gu,&gu);CHKERRQ(ierr);

  ierr = VecGetArray(opflow->X,&x);CHKERRQ(ierr);
  /* Solve */
  ipopt->solve_status = IpoptSolve(ipopt->nlp,x,NULL,&opflow->obj,NULL,NULL,NULL,opflow);

  ierr = VecRestoreArray(opflow->X,&x);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

PetscErrorCode OPFLOWSolverDestroy_IPOPT(OPFLOW opflow)
{
  PetscErrorCode     ierr;
  OPFLOWSolver_IPOPT ipopt=opflow->solver;

  PetscFunctionBegin;

  if(ipopt->nlp) {
    FreeIpoptProblem(ipopt->nlp);
  }
  ierr = MatDestroy(&ipopt->Jac_GeT);CHKERRQ(ierr);
  ierr = PetscFree(ipopt->jac_ge->colptr);CHKERRQ(ierr);
  ierr = PetscFree(ipopt->jac_ge->rowidx);CHKERRQ(ierr);
  ierr = PetscFree(ipopt->jac_ge->values);CHKERRQ(ierr);
  ierr = PetscFree(ipopt->jac_ge);CHKERRQ(ierr);

  if(opflow->nconineq) {
    ierr = MatDestroy(&ipopt->Jac_GiT);CHKERRQ(ierr);
    ierr = PetscFree(ipopt->jac_gi->colptr);CHKERRQ(ierr);
    ierr = PetscFree(ipopt->jac_gi->rowidx);CHKERRQ(ierr);
    ierr = PetscFree(ipopt->jac_gi->values);CHKERRQ(ierr);
    ierr = PetscFree(ipopt->jac_gi);CHKERRQ(ierr);
  }

  ierr = PetscFree(ipopt->hes->colptr);CHKERRQ(ierr);
  ierr = PetscFree(ipopt->hes->rowidx);CHKERRQ(ierr);
  ierr = PetscFree(ipopt->hes->values);CHKERRQ(ierr);
  ierr = PetscFree(ipopt->hes);CHKERRQ(ierr);

  ierr = PetscFree(ipopt);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

PetscErrorCode OPFLOWSolverSetUp_IPOPT(OPFLOW opflow)
{
  PetscFunctionBegin;

  PetscFunctionReturn(0);
}

PetscErrorCode OPFLOWSolverCreate_IPOPT(OPFLOW opflow)
{
  PetscErrorCode ierr;
  OPFLOWSolver_IPOPT ipopt;
  
  PetscFunctionBegin;
  ierr = PetscCalloc1(1,&ipopt);CHKERRQ(ierr);

  ipopt->nlp = NULL;
  ipopt->nnz_jac_g = 0;
  ipopt->nnz_hes = 0;
  opflow->solver = ipopt;

  opflow->solverops.setup = OPFLOWSolverSetUp_IPOPT;
  opflow->solverops.solve = OPFLOWSolverSolve_IPOPT;
  opflow->solverops.destroy = OPFLOWSolverDestroy_IPOPT;

  PetscFunctionReturn(0);
}

#endif
