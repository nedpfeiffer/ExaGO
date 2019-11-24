#if defined(SCOPFLOW_HAVE_IPOPT)

#include <private/opflowimpl.h>
#include <private/scopflowimpl.h>
#include "scopflow-ipopt.h"

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
Bool eval_scopflow_f(PetscInt n, PetscScalar* x, Bool new_x,
            PetscScalar* obj_value, UserDataPtr user_data)
{
  PetscErrorCode ierr;
  SCOPFLOW  scopflow=(SCOPFLOW)user_data;
  SCOPFLOWSolver_IPOPT scopflowipopt = (SCOPFLOWSolver_IPOPT)scopflow->solver;
  OPFLOW    opflow;
  PetscInt  i;
  PetscScalar *xi;
  PetscScalar opflowobj;

  *obj_value = 0.0;

  for(i=0; i < scopflow->Ns; i++) {
    opflowobj = 0.0;
    xi = x + scopflowipopt->xstarti[i];
    opflow = scopflow->opflows[0];
    ierr = VecPlaceArray(opflow->X,xi);CHKERRQ(ierr);
    ierr = (*opflow->formops.computeobjective)(opflow,opflow->X,&opflowobj);CHKERRQ(ierr);
    *obj_value += opflowobj;
    ierr = VecResetArray(opflow->X);CHKERRQ(ierr);
  }
				
  return TRUE;
}

Bool eval_scopflow_grad_f(PetscInt n, PetscScalar* x, Bool new_x,
                 PetscScalar* grad_f, UserDataPtr user_data)
{
  PetscErrorCode ierr;
  SCOPFLOW  scopflow=(SCOPFLOW)user_data;
  SCOPFLOWSolver_IPOPT scopflowipopt = (SCOPFLOWSolver_IPOPT)scopflow->solver;
  OPFLOW    opflow;
  PetscInt  i;
  PetscScalar *xi,*gradi;

  for(i=0; i < scopflow->Ns; i++) {
    xi = x + scopflowipopt->xstarti[i];
    gradi = grad_f + scopflowipopt->xstarti[i];
    opflow = scopflow->opflows[0];

    ierr = VecPlaceArray(opflow->X,xi);CHKERRQ(ierr);
    ierr = VecPlaceArray(opflow->gradobj,gradi);CHKERRQ(ierr);
    ierr = VecSet(opflow->gradobj,0.0);CHKERRQ(ierr);

    ierr = (*opflow->formops.computegradient)(opflow,opflow->X,opflow->gradobj);CHKERRQ(ierr);
    
    ierr = VecResetArray(opflow->X);CHKERRQ(ierr);
    ierr = VecResetArray(opflow->gradobj);CHKERRQ(ierr);
  }
  return TRUE;
}

Bool eval_scopflow_g(PetscInt n, PetscScalar* x, Bool new_x,
            PetscInt m, PetscScalar* g, UserDataPtr user_data)
{
  PetscErrorCode ierr;
  SCOPFLOW  scopflow=(SCOPFLOW)user_data;
  SCOPFLOWSolver_IPOPT scopflowipopt = (SCOPFLOWSolver_IPOPT)scopflow->solver;
  OPFLOW    opflow;
  PetscInt  i,j,k,loc,ctr;
  PetscScalar *x0,*xi,*geqi,*gineqi,*gineqcoupi;
  PS        ps;
  PSBUS     bus;
  PSGEN     gen;

  x0 = x;
  for(i=0; i < scopflow->Ns; i++) {
    xi   = x + scopflowipopt->xstarti[i];
    geqi = g + scopflowipopt->gstarti[i];

    opflow = scopflow->opflows[0];

    ierr = VecPlaceArray(opflow->X,xi);CHKERRQ(ierr);

    /* Equality constraints */
    ierr = VecPlaceArray(opflow->Ge,geqi);CHKERRQ(ierr);
    ierr = (*opflow->formops.computeequalityconstraints)(opflow,opflow->X,opflow->Ge);CHKERRQ(ierr);
    ierr = VecResetArray(opflow->Ge);CHKERRQ(ierr);

    /* This code needs to be moved to formulation */
    if(scopflow->Nconineq) {
      gineqi = geqi + opflow->nconeq;

      /* Inequality constraints */
      ierr = VecPlaceArray(scopflow->Gi,gineqi);CHKERRQ(ierr);
      ierr = (*opflow->formops.computeinequalityconstraints)(opflow,opflow->X,opflow->Gi);CHKERRQ(ierr);
      ierr = VecResetArray(opflow->Gi);CHKERRQ(ierr);
    }

    if(scopflow->nconineqcoup[i]) {
      gineqcoupi = gineqi + opflow->nconineq;
      ctr = 0;
      ps = opflow->ps;
      for(j=0; j < ps->nbus; j++) {
	bus = &ps->bus[i];
	ierr = PSBUSGetVariableLocation(bus,&loc);CHKERRQ(ierr);
	
	for(k=0; k < bus->ngen; k++) {
	  loc += 2;
	  ierr = PSBUSGetGen(bus,k,&gen);CHKERRQ(ierr);
	  gineqcoupi[ctr++] = xi[loc] - x0[loc]; /* PGi - PG0 */
	}
      }
    }

    ierr = VecResetArray(opflow->X);CHKERRQ(ierr);
  }

  return TRUE;
}

Bool eval_scopflow_jac_g(PetscInt n, PetscScalar *x, Bool new_x,
                PetscInt m, PetscInt nele_jac,
                PetscInt *iRow, PetscInt *jCol, PetscScalar *values,
                UserDataPtr user_data)
{

  PetscErrorCode ierr;
  SCOPFLOW         scopflow=(SCOPFLOW)user_data;
  SCOPFLOWSolver_IPOPT scopflowipopt=(SCOPFLOWSolver_IPOPT)scopflow->solver;
  OPFLOW         opflow;
  OPFLOWSolver_IPOPT opflowipopt;
  PetscInt       *iRowstart = iRow,*jColstart=jCol;
  PetscInt       roffset,coffset;
  Mat_SeqAIJ     *aij;
  PetscInt       nrow,ncol;
  PetscScalar    *xi,*valuesi;
  PetscInt       i;

  if(values == NULL) {
    /* Set locations only */

    for(i=0; i < scopflow->Ns; i++) {
      opflow = scopflow->opflows[i];
      opflowipopt = opflow->solver;

      roffset = scopflowipopt->gstarti[i];
      coffset = scopflowipopt->xstarti[i];

      /* Jac_GeT already calculated during set up stage */
      ierr = MatGetSize(opflowipopt->Jac_GeT,&nrow,&ncol);CHKERRQ(ierr);
      aij = (Mat_SeqAIJ*)opflowipopt->Jac_GeT->data;
      ierr = PetscMemcpy(opflowipopt->jac_ge->rowidx,aij->j,aij->nz*sizeof(PetscInt));CHKERRQ(ierr);
      ierr = PetscMemcpy(opflowipopt->jac_ge->colptr,aij->i,(nrow+1)*sizeof(PetscInt));CHKERRQ(ierr);
      ierr = PetscMemcpy(opflowipopt->jac_ge->values,aij->a,aij->nz*sizeof(PetscScalar));CHKERRQ(ierr);
      CCMatrixToMatrixMarketLocationsOnly(opflowipopt->jac_ge,opflow->nx,iRowstart,jColstart,roffset,coffset,opflowipopt->nnz_jac_ge);

      /* Increment iRow,jCol pointers */
      iRowstart += opflowipopt->nnz_jac_ge;
      jColstart += opflowipopt->nnz_jac_ge;

      if(opflow->Nconineq) {
	/* Inequality constrained Jacobian */
	roffset += opflow->nconeq;

	/* Jac_GiT already computed during set up stage */
	ierr = MatGetSize(opflowipopt->Jac_GiT,&nrow,&ncol);CHKERRQ(ierr);
	aij = (Mat_SeqAIJ*)opflowipopt->Jac_GiT->data;
	ierr = PetscMemcpy(opflowipopt->jac_gi->rowidx,aij->j,aij->nz*sizeof(PetscInt));CHKERRQ(ierr);
	ierr = PetscMemcpy(opflowipopt->jac_gi->colptr,aij->i,(nrow+1)*sizeof(PetscInt));CHKERRQ(ierr);
	ierr = PetscMemcpy(opflowipopt->jac_gi->values,aij->a,aij->nz*sizeof(PetscScalar));CHKERRQ(ierr);
      
	CCMatrixToMatrixMarketLocationsOnly(opflowipopt->jac_gi,opflow->nx,iRowstart,jColstart,roffset,coffset,opflowipopt->nnz_jac_gi);
      }

      if(scopflow->iscoupling) {
	/***** Coupling constraint Jacobian to be added here ********/
      }

      ierr = VecResetArray(opflow->X);CHKERRQ(ierr);
    }
  } else {

    valuesi = values;
    for(i=0; i < scopflow->Ns; i++) {
      opflow = scopflow->opflows[i];
      opflowipopt = opflow->solver;

      xi = x + scopflowipopt->xstarti[i];
      ierr = VecPlaceArray(opflow->X,xi);CHKERRQ(ierr);
      /* Compute equality constraint jacobian */
      ierr = (*opflow->formops.computeequalityconstraintjacobian)(opflow,opflow->X,opflow->Jac_Ge);CHKERRQ(ierr);
      ierr = MatTranspose(opflow->Jac_Ge,MAT_REUSE_MATRIX,&opflowipopt->Jac_GeT);CHKERRQ(ierr);
      ierr = MatGetSize(opflowipopt->Jac_GeT,&nrow,&ncol);CHKERRQ(ierr);
      aij = (Mat_SeqAIJ*)opflowipopt->Jac_GeT->data;

      ierr = PetscMemcpy(opflowipopt->jac_ge->values,aij->a,aij->nz*sizeof(PetscScalar));CHKERRQ(ierr);

      CCMatrixToMatrixMarketValuesOnly(opflowipopt->jac_ge,opflowipopt->nnz_jac_ge,valuesi);
      valuesi += opflowipopt->nnz_jac_ge;

      if(opflow->Nconineq) {
	/* Compute inequality constraint jacobian */
	ierr = (*opflow->formops.computeinequalityconstraintjacobian)(opflow,opflow->X,opflow->Jac_Gi);CHKERRQ(ierr);

	ierr = MatTranspose(opflow->Jac_Gi,MAT_REUSE_MATRIX,&opflowipopt->Jac_GiT);CHKERRQ(ierr);
	aij = (Mat_SeqAIJ*)opflowipopt->Jac_GiT->data;

	ierr = PetscMemcpy(opflowipopt->jac_gi->values,aij->a,aij->nz*sizeof(PetscScalar));CHKERRQ(ierr);
      

	CCMatrixToMatrixMarketValuesOnly(opflowipopt->jac_gi,opflowipopt->nnz_jac_gi,valuesi);
	valuesi += opflowipopt->nnz_jac_gi;
      }

      if(scopflow->iscoupling) {
	/*** Coupling constraint Jacobian part to be added here */
      }
      ierr = VecResetArray(opflow->X);CHKERRQ(ierr);
    }
  }

  return TRUE;
}

Bool eval_scopflow_h(PetscInt n, PetscScalar *x, Bool new_x, PetscScalar obj_factor,
            PetscInt m, PetscScalar *lambda, Bool new_lambda,
            PetscInt nele_hess, PetscInt *iRow, PetscInt *jCol,
            PetscScalar *values, UserDataPtr user_data)
{
  PetscErrorCode ierr;
  PetscInt       nrow;
  Mat_SeqSBAIJ   *sbaij;
  SCOPFLOW         scopflow=(SCOPFLOW)user_data;
  SCOPFLOWSolver_IPOPT scopflowipopt=(SCOPFLOWSolver_IPOPT)scopflow->solver;
  OPFLOW          opflow;
  OPFLOWSolver_IPOPT opflowipopt;
  PetscScalar     *xi,*valuesi=values,*lameqi,*lamineqi;
  PetscInt         i;
  PetscInt        *iRowStart=iRow,*jColStart=jCol;
  PetscInt        roffset;

  scopflow->obj_factor = obj_factor;

  if(values == NULL) {

    for(i=0; i < scopflow->Ns; i++) {
      opflow = scopflow->opflows[i];
      opflowipopt = opflow->solver;

      roffset = scopflowipopt->xstarti[i];

      ierr = MatGetSize(opflowipopt->Hes_sbaij,&nrow,NULL);CHKERRQ(ierr);
      sbaij = (Mat_SeqSBAIJ*)opflowipopt->Hes_sbaij->data;
      ierr = PetscMemcpy(opflowipopt->hes->rowidx,sbaij->j,sbaij->nz*sizeof(PetscInt));CHKERRQ(ierr);
      ierr = PetscMemcpy(opflowipopt->hes->colptr,sbaij->i,(nrow+1)*sizeof(PetscInt));CHKERRQ(ierr);
      ierr = PetscMemcpy(opflowipopt->hes->values,sbaij->a,sbaij->nz*sizeof(PetscScalar));CHKERRQ(ierr);
      CCMatrixToMatrixMarketLocationsOnly(opflowipopt->hes,opflow->nx,iRowStart,jColStart,roffset,roffset,opflowipopt->nnz_hes);

      iRowStart += opflowipopt->nnz_hes;
      jColStart += opflowipopt->nnz_hes;
    }
  } else {

    for(i=0; i < scopflow->Ns; i++) {
      opflow = scopflow->opflows[i];
      opflowipopt = opflow->solver;

      roffset = scopflowipopt->xstarti[i];

      xi = x + roffset;
      lameqi = lambda + scopflowipopt->gstarti[i];

      ierr = VecPlaceArray(opflow->X,xi);CHKERRQ(ierr);
      ierr = VecPlaceArray(opflow->Lambdae,lameqi);CHKERRQ(ierr);
      if(opflow->Nconineq) {
	lamineqi = lameqi + opflow->nconeq;
	ierr = VecPlaceArray(opflow->Lambdai,lamineqi);CHKERRQ(ierr);
      }

      /* Compute non-zeros for Hessian */
      ierr = (*opflow->formops.computehessian)(opflow,opflow->X,opflow->Lambdae,opflow->Lambdai,opflow->Hes);CHKERRQ(ierr);
      ierr = MatConvert(opflow->Hes,MATSEQSBAIJ,MAT_REUSE_MATRIX,&opflowipopt->Hes_sbaij);CHKERRQ(ierr);
      /* Since the Hessian is symmetric, we don't need to convert it to column compressed sparse format */
      sbaij = (Mat_SeqSBAIJ*)opflowipopt->Hes_sbaij->data;
      opflowipopt->nnz_hes = sbaij->nz;
      ierr = PetscMemcpy(opflowipopt->hes->values,sbaij->a,sbaij->nz*sizeof(PetscScalar));CHKERRQ(ierr);
      CCMatrixToMatrixMarketValuesOnly(opflowipopt->hes,opflowipopt->nnz_hes,valuesi);
      valuesi += opflowipopt->nnz_hes;

      ierr = VecResetArray(opflow->X);CHKERRQ(ierr);
      ierr = VecResetArray(opflow->Lambdae);CHKERRQ(ierr);
      if(opflow->Nconineq) {
	ierr = VecResetArray(opflow->Lambdai);CHKERRQ(ierr);
      }
    }
  }

  return 1;
}

PetscErrorCode SCOPFLOWSolverSolve_IPOPT(SCOPFLOW scopflow)
{
  PetscErrorCode     ierr;
  SCOPFLOWSolver_IPOPT scopflowipopt=scopflow->solver;
  OPFLOW             opflow;
  OPFLOWSolver_IPOPT opflowipopt;
  Mat_SeqAIJ         *aij;
  Mat_SeqSBAIJ       *sbaij;
  PetscScalar        *x,*xl,*xu,*gl,*gu,*xi,*lameqi,*lamineqi,*lam;
  PetscInt           i;

  PetscFunctionBegin;

  scopflowipopt->nnz_jac_ge = scopflowipopt->nnz_jac_gi = 0;

  ierr = VecGetArray(scopflow->X,&x);CHKERRQ(ierr);
  ierr = VecGetArray(scopflow->Lambda,&lam);CHKERRQ(ierr);
  for(i=0; i < scopflow->Ns; i++) {
    opflow = scopflow->opflows[i];
    opflowipopt = opflow->solver;
    xi = x + scopflowipopt->xstarti[i];

    ierr = VecPlaceArray(opflow->X,xi);CHKERRQ(ierr);

    /* Compute nonzeros for the Jacobian */
    /* Equality constraint Jacobian */
    ierr = (*opflow->formops.computeequalityconstraintjacobian)(opflow,opflow->X,opflow->Jac_Ge);CHKERRQ(ierr);
    /* Transpose the matrix to convert it to column compressed sparse format */
    ierr = MatTranspose(opflow->Jac_Ge,MAT_INITIAL_MATRIX,&opflowipopt->Jac_GeT);CHKERRQ(ierr);
    ierr = MatSetOption(opflowipopt->Jac_GeT,MAT_NEW_NONZERO_LOCATION_ERR,PETSC_TRUE);CHKERRQ(ierr);
    aij = (Mat_SeqAIJ*)opflowipopt->Jac_GeT->data;

    opflowipopt->nnz_jac_ge = aij->nz;
    scopflowipopt->nnz_jac_ge += opflowipopt->nnz_jac_ge;

    /* Create ccmatrix object for equality constrained Jacobian */
    ierr = PetscCalloc1(1,&opflowipopt->jac_ge);CHKERRQ(ierr);
    ierr = PetscMalloc1(opflow->nx+1,&opflowipopt->jac_ge->colptr);CHKERRQ(ierr);
    ierr = PetscMalloc1(opflowipopt->nnz_jac_ge,&opflowipopt->jac_ge->rowidx);CHKERRQ(ierr);
    ierr = PetscMalloc1(opflowipopt->nnz_jac_ge,&opflowipopt->jac_ge->values);CHKERRQ(ierr);

    opflowipopt->nnz_jac_gi = 0;
    if(opflow->Nconineq) {
      ierr = (*opflow->formops.computeinequalityconstraintjacobian)(opflow,opflow->X,opflow->Jac_Gi);CHKERRQ(ierr);
      /* Transpose the matrix to convert it to column compressed sparse format */
      ierr = MatTranspose(opflow->Jac_Gi,MAT_INITIAL_MATRIX,&opflowipopt->Jac_GiT);CHKERRQ(ierr);
      ierr = MatSetOption(opflowipopt->Jac_GiT,MAT_NEW_NONZERO_LOCATION_ERR,PETSC_TRUE);CHKERRQ(ierr);
      aij = (Mat_SeqAIJ*)opflowipopt->Jac_GiT->data;
      opflowipopt->nnz_jac_gi = aij->nz;
      scopflowipopt->nnz_jac_gi += opflowipopt->nnz_jac_gi;

      /* Create ccmatrix object for inequality constrained Jacobian */
      ierr = PetscCalloc1(1,&opflowipopt->jac_gi);CHKERRQ(ierr);
      ierr = PetscMalloc1(opflow->nx+1,&opflowipopt->jac_gi->colptr);CHKERRQ(ierr);
      ierr = PetscMalloc1(opflowipopt->nnz_jac_gi,&opflowipopt->jac_gi->rowidx);CHKERRQ(ierr);
      ierr = PetscMalloc1(opflowipopt->nnz_jac_gi,&opflowipopt->jac_gi->values);CHKERRQ(ierr);
    }   
    opflowipopt->nnz_jac_g = opflowipopt->nnz_jac_ge + opflowipopt->nnz_jac_gi;

    /* Add non-zeros for Jacobian of coupling constraints */
    if(scopflow->iscoupling) scopflowipopt->nnz_jac_gi += 2*scopflow->nconineqcoup[i];


    /* Compute non-zeros for Hessian */

    lameqi = lam + scopflowipopt->gstarti[i];
    lamineqi = lam + scopflowipopt->gstarti[i]+opflow->nconeq;

    ierr = VecPlaceArray(opflow->Lambdae,lameqi);CHKERRQ(ierr);
    ierr = VecPlaceArray(opflow->Lambdai,lamineqi);CHKERRQ(ierr);

    ierr = (*opflow->formops.computehessian)(opflow,opflow->X,opflow->Lambdae,opflow->Lambdai,opflow->Hes);CHKERRQ(ierr);
    /* Convert matrix to symmetric sbaij format needed for the IPOPT solver */
    ierr = MatSetOption(opflow->Hes,MAT_SYMMETRIC,PETSC_TRUE);CHKERRQ(ierr);
    ierr = MatConvert(opflow->Hes,MATSEQSBAIJ,MAT_INITIAL_MATRIX,&opflowipopt->Hes_sbaij);CHKERRQ(ierr);
    /* Since the Hessian is symmetric, we don't need to convert it to column compressed sparse format */
    sbaij = (Mat_SeqSBAIJ*)opflowipopt->Hes_sbaij->data;
    opflowipopt->nnz_hes = sbaij->nz;
    scopflowipopt->nnz_hes += opflowipopt->nnz_hes;

    /* Create ccmatrix object for hessian */
    ierr = PetscCalloc1(1,&opflowipopt->hes);CHKERRQ(ierr);
    ierr = PetscMalloc1(opflow->nx+1,&opflowipopt->hes->colptr);CHKERRQ(ierr);
    ierr = PetscMalloc1(opflowipopt->nnz_hes,&opflowipopt->hes->rowidx);CHKERRQ(ierr);
    ierr = PetscMalloc1(opflowipopt->nnz_hes,&opflowipopt->hes->values);CHKERRQ(ierr);

    ierr = VecResetArray(opflow->Lambdae);CHKERRQ(ierr);
    ierr = VecResetArray(opflow->Lambdai);CHKERRQ(ierr);
    ierr = VecResetArray(opflow->X);CHKERRQ(ierr);
  }
  scopflowipopt->nnz_jac_g = scopflowipopt->nnz_jac_ge + scopflowipopt->nnz_jac_gi;

  /* Create IPOPT solver instance */
  ierr = VecGetArray(scopflow->Xl,&xl);CHKERRQ(ierr);
  ierr = VecGetArray(scopflow->Xu,&xu);CHKERRQ(ierr);
  ierr = VecGetArray(scopflow->Gl,&gl);CHKERRQ(ierr);
  ierr = VecGetArray(scopflow->Gu,&gu);CHKERRQ(ierr);

  /* Create IPOPT problem */
  scopflowipopt->nlp = CreateIpoptProblem(scopflow->Nx,xl,xu,scopflow->Ncon,gl,gu,scopflowipopt->nnz_jac_g,scopflowipopt->nnz_hes,0,&eval_scopflow_f,
				   &eval_scopflow_g,&eval_scopflow_grad_f,
				   &eval_scopflow_jac_g,&eval_scopflow_h);

  ierr = VecRestoreArray(scopflow->Xl,&xl);CHKERRQ(ierr);
  ierr = VecRestoreArray(scopflow->Xu,&xu);CHKERRQ(ierr);
  ierr = VecRestoreArray(scopflow->Gl,&gl);CHKERRQ(ierr);
  ierr = VecRestoreArray(scopflow->Gu,&gu);CHKERRQ(ierr);

  ierr = VecGetArray(scopflow->X,&x);CHKERRQ(ierr);

  /* Solve */
  //  scopflowipopt->solve_status = IpoptSolve(scopflowipopt->nlp,x,NULL,&scopflow->obj,NULL,NULL,NULL,scopflow);

  ierr = VecRestoreArray(scopflow->X,&x);CHKERRQ(ierr);

  PetscFunctionReturn(0);
}

PetscErrorCode SCOPFLOWSolverDestroy_IPOPT(SCOPFLOW scopflow)
{
  PetscErrorCode     ierr;
  SCOPFLOWSolver_IPOPT ipopt=scopflow->solver;

  PetscFunctionBegin;

  if(ipopt->nlp) {
    FreeIpoptProblem(ipopt->nlp);
    ipopt->nlp = NULL;
  }

  ierr = PetscFree(ipopt->xstarti);CHKERRQ(ierr);
  ierr = PetscFree(ipopt->gstarti);CHKERRQ(ierr);
  ierr = PetscFree(ipopt->nxi);CHKERRQ(ierr);
  ierr = PetscFree(ipopt->ngi);CHKERRQ(ierr);

  ierr = PetscFree(ipopt);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

PetscErrorCode SCOPFLOWSolverSetUp_IPOPT(SCOPFLOW scopflow)
{
  PetscErrorCode ierr;
  SCOPFLOWSolver_IPOPT ipopt = (SCOPFLOWSolver_IPOPT)scopflow->solver;
  PetscInt       i,j,k,ctr;
  OPFLOW         opflow;
  PetscInt       ngen;
  PetscScalar    *x,*xi,*xl,*xu,*xli,*xui,*gl,*gu,*gli,*gui;
  PS             ps;
  PSBUS          bus;
  PSGEN          gen;

  PetscFunctionBegin;

  ierr = PetscCalloc1(scopflow->Ns,&ipopt->xstarti);CHKERRQ(ierr);
  ierr = PetscCalloc1(scopflow->Ns,&ipopt->gstarti);CHKERRQ(ierr);
  ierr = PetscCalloc1(scopflow->Ns,&ipopt->nxi);CHKERRQ(ierr);
  ierr = PetscCalloc1(scopflow->Ns,&ipopt->ngi);CHKERRQ(ierr);

  scopflow->Nx = 0;
  scopflow->Ncon = 0;
  ipopt->xstarti[0] = 0;
  ipopt->gstarti[0] = 0;

  ierr = PSGetNumGenerators(scopflow->opflows[0]->ps,&ngen,NULL);CHKERRQ(ierr);
  for(i=0; i < scopflow->Ns; i++) {
    opflow = scopflow->opflows[i];
    ipopt->nxi[i] = opflow->nx;
    if(scopflow->iscoupling) scopflow->nconineqcoup[i] = (i == 0)?0:ngen;
    else scopflow->nconineqcoup[i] = 0;

    ipopt->ngi[i] = opflow->ncon + scopflow->nconineqcoup[i];
    if(i < scopflow->Ns - 1) {
      ipopt->xstarti[i+1] = ipopt->xstarti[i] + ipopt->nxi[i];
      ipopt->gstarti[i+1] = ipopt->gstarti[i] + ipopt->ngi[i];
    }
    scopflow->Nx += ipopt->nxi[i];
    scopflow->Ncon += ipopt->ngi[i];
  }

  /* Create vector X */
  ierr = VecCreate(scopflow->comm->type,&scopflow->X);CHKERRQ(ierr);
  ierr = VecSetSizes(scopflow->X,scopflow->Nx,PETSC_DECIDE);CHKERRQ(ierr);
  ierr = VecSetFromOptions(scopflow->X);CHKERRQ(ierr);

  ierr = VecDuplicate(scopflow->X,&scopflow->Xl);CHKERRQ(ierr);
  ierr = VecDuplicate(scopflow->X,&scopflow->Xu);CHKERRQ(ierr);

  /* vector for constraints */
  ierr = VecCreate(scopflow->comm->type,&scopflow->G);CHKERRQ(ierr);
  ierr = VecSetSizes(scopflow->G,scopflow->Ncon,PETSC_DECIDE);CHKERRQ(ierr);
  ierr = VecSetFromOptions(scopflow->G);CHKERRQ(ierr);

  /* Constraint bounds vectors  */
  ierr = VecDuplicate(scopflow->G,&scopflow->Gl);CHKERRQ(ierr);
  ierr = VecDuplicate(scopflow->G,&scopflow->Gu);CHKERRQ(ierr);

  /* Lagrangian multipliers */
  ierr = VecDuplicate(scopflow->G,&scopflow->Lambda);CHKERRQ(ierr);

  /* Set Initial guess and Bounds on variables and constraints */
  ierr = VecGetArray(scopflow->X,&x);CHKERRQ(ierr);
  ierr = VecGetArray(scopflow->Xl,&xl);CHKERRQ(ierr);
  ierr = VecGetArray(scopflow->Xu,&xu);CHKERRQ(ierr);
  ierr = VecGetArray(scopflow->Gl,&gl);CHKERRQ(ierr);
  ierr = VecGetArray(scopflow->Gu,&gu);CHKERRQ(ierr);

  for(i=0; i < scopflow->Ns; i++) {
    opflow = scopflow->opflows[i];
    /* Set initial guess and bounds on variables */
    xi  = x  + ipopt->xstarti[i];
    xli = xl + ipopt->xstarti[i];
    xui = xu + ipopt->xstarti[i];

    ierr = VecPlaceArray(opflow->X,xi);CHKERRQ(ierr);
    ierr = VecPlaceArray(opflow->Xl,xli);CHKERRQ(ierr);
    ierr = VecPlaceArray(opflow->Xu,xui);CHKERRQ(ierr);

    /* Set bounds */
    ierr = (*opflow->formops.setvariablebounds)(opflow,opflow->Xl,opflow->Xu);CHKERRQ(ierr);

    /* Set initial guess */
    ierr = (*opflow->formops.setinitialguess)(opflow,opflow->X);CHKERRQ(ierr);

    ierr = VecResetArray(opflow->X);CHKERRQ(ierr);
    ierr = VecResetArray(opflow->Xl);CHKERRQ(ierr);
    ierr = VecResetArray(opflow->Xu);CHKERRQ(ierr);

    /* Set bounds on constraints */
    gli = gl + ipopt->gstarti[i];
    gui = gu + ipopt->gstarti[i];
    
    ierr = VecPlaceArray(opflow->Gl,gli);CHKERRQ(ierr);
    ierr = VecPlaceArray(opflow->Gu,gui);CHKERRQ(ierr);

    ierr = (*opflow->formops.setconstraintbounds)(opflow,opflow->Gl,opflow->Gu);CHKERRQ(ierr);

    ierr = VecResetArray(opflow->Gl);CHKERRQ(ierr);
    ierr = VecResetArray(opflow->Gu);CHKERRQ(ierr);
    
    if(scopflow->nconineqcoup[i]) {
      ctr = 0;
      ps = opflow->ps;
      /* Bounds on coupling constraints */
      for(j=0; j < ps->nbus; j++) {
	bus = &ps->bus[j];

	for(k=0; k < bus->ngen; k++) {
	  ierr = PSBUSGetGen(bus,k,&gen);CHKERRQ(ierr);
	  /* Generator can do a full ramp up to its max. capacity */
	  gli[opflow->ncon + ctr] = -gen->pt;
	  gui[opflow->ncon + ctr] =  gen->pt;
	  ctr++;
	}
      }
    }
  }
  
  ierr = VecRestoreArray(scopflow->Xl,&xl);CHKERRQ(ierr);
  ierr = VecRestoreArray(scopflow->Xu,&xu);CHKERRQ(ierr);
  ierr = VecRestoreArray(scopflow->Gl,&gl);CHKERRQ(ierr);
  ierr = VecRestoreArray(scopflow->Gu,&gu);CHKERRQ(ierr);

  /* Initialize Lagrange multiplier */
  ierr = VecSet(scopflow->Lambda,1.0);CHKERRQ(ierr);
  
  PetscFunctionReturn(0);
}

PetscErrorCode SCOPFLOWSolverCreate_IPOPT(SCOPFLOW scopflow)
{
  PetscErrorCode ierr;
  SCOPFLOWSolver_IPOPT ipopt;
  
  PetscFunctionBegin;

  if(scopflow->comm->size > 1) SETERRQ1(PETSC_ERR_SUP,PETSC_COMM_SELF,"nrank = %d, IPOPT solver does not support execution in parallel\n",scopflow->comm->size); 
  ierr = PetscCalloc1(1,&ipopt);CHKERRQ(ierr);

  ipopt->nlp = NULL;
  ipopt->nnz_jac_g = 0;
  ipopt->nnz_hes = 0;
  scopflow->solver = ipopt;

  scopflow->solverops.setup = SCOPFLOWSolverSetUp_IPOPT;
  scopflow->solverops.solve = SCOPFLOWSolverSolve_IPOPT;
  scopflow->solverops.destroy = SCOPFLOWSolverDestroy_IPOPT;

  PetscFunctionReturn(0);
}

#endif
