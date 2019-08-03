#include <private/psimpl.h>
#include <private/opflowimpl.h>
#include <private/scopflowpipsimpl.h>
#include <math.h>

int str_init_x0(double* x0, CallBackDataPtr cbd) {
	int row = cbd->row_node_id;
	int col = cbd->col_node_id;
	if(row == 0)
	{
		x0[0] = 1.0;
		x0[1] = 1.0;
	}
	else if(row == 1)
	{
		x0[0] = 1.0;
	}
	else if(row == 2)
	{
		x0[0] = 1.0;
	}

	return 1;
}

int str_prob_info(int* n, double* col_lb, double* col_ub, int* m,
		double* row_lb, double* row_ub, CallBackDataPtr cbd) {
	int row = cbd->row_node_id;
	int col = cbd->col_node_id;
        int type = cbd->typeflag;
        if(type == 1){
	    if(row_lb == NULL){
                *m = 0;
	    }
            return 1;
	}
	if(col_lb == NULL)
	{
	  if(row==0)
	    {
	      *n = 2;
	      *m = 1;
	    }
	  else if(row ==1 || row == 2)
	    {
	      *n = 1;
	      *m = 1;
	    }
	}
	else
	{
		if(row==0)
		{
			col_lb[0] = -INFINITY;
			col_lb[1] = -INFINITY;
			col_ub[0] = INFINITY;
			col_ub[1] = INFINITY;
			row_lb[0] = 100;
			row_ub[0] = 100;
		}
		else if(row ==1 || row == 2)
		{
			col_lb[0] = -INFINITY;
			col_ub[0] = INFINITY;
			row_lb[0] = 0;
			row_ub[0] = 600;
		}
	}

	return 1;
}

int str_eval_f(double* x0, double* x1, double* obj, CallBackDataPtr cbd) {
	int row = cbd->row_node_id;
	int col = cbd->col_node_id;
	if(row == 0 )
	{   // (x0 + x1) ^ 2 + x0 + x1
		*obj =  ( x0[0] + x0[1] ) * ( x0[0] + x0[1] ) + x0[0] + x0[1];
	}
	else if(row == 1)
	{   // (x0 + x1)x3 + x3
		*obj = ( x0[0] + x0[1] ) * x1[0] + x1[0];
	}
	else if(row == 2)
	{  // (x0 + x1)x4 + x4
		*obj = ( x0[0] + x0[1] ) * x1[0] + x1[0];
	}
	return 1;
}

int str_eval_g(double* x0, double* x1, double* eq_g, double* inq_g,
		CallBackDataPtr cbd) {
	int row = cbd->row_node_id;
	int col = cbd->col_node_id;
	if(row == 0)
	{	//x1 + x2 + x1x2 = 100
		eq_g[0] = x0[0] + x0[1] + x0[0]*x0[1];
	}
	else if(row == 1)
	{   //0< x2^2 + x3x1 + x1 + x2 + x3^2 + x3<600
		inq_g[0] = x0[1]*x0[1] + x1[0] * x0[0] + x0[0] + x0[1] + x1[0]*x1[0] + x1[0];
	}
	else if(row == 2)
	{  //0< x1^2 + x4x2 + x1 + x2 + x4^2 + x4< 600
		inq_g[0] = x0[0]*x0[0] + x1[0] * x0[1] + x0[0] + x0[1] + x1[0]*x1[0] + x1[0];
	}

	return 1;
}

int str_eval_grad_f(double* x0, double* x1, double* grad, CallBackDataPtr cbd) {
	int row = cbd->row_node_id;
	int col = cbd->col_node_id;
	if(row == 0 && col == 0)
	{
		grad[0] = 2.0 * (x0[0] + x0[1]) + 1.0;
		grad[1] = 2.0 * (x0[0] + x0[1]) + 1.0;
	}
	else if(row == 1 && col == 1)
	{
		grad[0] = (x0[0] + x0[1]) + 1.0;
	}
	else if(row == 2 && col == 2)
	{
		grad[0] = (x0[0] + x0[1]) + 1.0;
	}
	else if(row == 1 && col == 0)
	{
		grad[0] = x1[0];
		grad[1] = x1[0];
	}
	else if(row == 2 && col == 0)
	{
		grad[0] = x1[0];
		grad[1] = x1[0];
	}

	return 1;
}

int str_eval_jac_g(double* x0, double* x1, int* e_nz, double* e_elts,
		int* e_rowidx, int* e_colptr, int* i_nz, double* i_elts, int* i_rowidx,
		int* i_colptr, CallBackDataPtr cbd) {
	int row = cbd->row_node_id;
	int col = cbd->col_node_id;
	if(e_elts==NULL && i_elts == NULL)
	{

		if (row == 0 && col == 0) {
			*e_nz = 2;
			*i_nz = 0;
		} else if (row == 1 && col == 1) {
			*e_nz = 0;
			*i_nz = 1;
		} else if (row == 2 && col == 2) {
			*e_nz = 0;
			*i_nz = 1;
		} else if (row == 1 && col == 0) {
			*e_nz = 0;
			*i_nz = 2;
		} else if (row == 2 && col == 0) {
			*e_nz = 0;
			*i_nz = 2;
		}
	}
	else
	{
		if (row == 0 && col == 0) {
			e_rowidx[0] = 0;
			e_rowidx[1] = 0;
			e_colptr[0] = 0;
			e_colptr[1] = 1;
			e_colptr[2] = 2;
			e_elts[0] = x0[1] + 1.0;
			e_elts[1] = x0[0] + 1.0;
		} else if (row == 1 && col == 1) {
			i_rowidx[0] = 0;
			i_colptr[0] = 0;
			i_colptr[1] = 1;
			i_elts[0] = x0[0] + 2.0*x1[0] + 1.0;
		} else if (row == 2 && col == 2) {
			i_rowidx[0] = 0;
			i_colptr[0] = 0;
			i_colptr[1] = 1;
			i_elts[0] = x0[1] + 2.0*x1[0] + 1.0;
		} else if (row == 1 && col == 0) {
			i_rowidx[0] = 0;
			i_rowidx[1] = 0;
			i_colptr[0] = 0;
			i_colptr[1] = 1;
			i_colptr[2] = 2;
			i_elts[0] = x1[0] + 1.0;
			i_elts[1] = 2.0*x0[1] + 1.0;
		} else if (row == 2 && col == 0) {

			i_rowidx[0] = 0;
			i_rowidx[1] = 0;
			i_colptr[0] = 0;
			i_colptr[1] = 1;
			i_colptr[2] = 2;
			i_elts[0] = 2.0*x0[0] + 1.0;
			i_elts[1] = x1[0] + 1.0;
		}
	}


	return 1;
}

int str_eval_h(double* x0, double* x1, double* lambda, int* nz, double* elts,
		int* rowidx, int* colptr, CallBackDataPtr cbd) {
	int row = cbd->row_node_id;
	int col = cbd->col_node_id;
	if(elts==NULL)
	{

		if (row == 0 && col == 0) {
			*nz = 3;
		} else if (row == 1 && col == 1) {
			*nz = 1;
		} else if (row == 2 && col == 2) {
			*nz = 1;
		} else if (row == 1 && col == 0) {
			*nz = 3;
		} else if (row == 2 && col == 0) {
			*nz = 3;
		} else if (row == 0 && col == 1) {
			*nz = 2;
		} else if (row == 0 && col == 2) {
			*nz = 2;
		}

	}
	else{
		if (row == 0 && col == 0) {
			rowidx[0] = 0;
			rowidx[1] = 1;
			rowidx[2] = 1;
			colptr[0] = 0;
			colptr[1] = 2;
			colptr[2] = 3;
			elts[0] = 2.0;
			elts[1] = 2.0 + 1.0 * lambda[0];
			elts[2] = 2.0;
		} else if (row == 1 && col == 1) {
			rowidx[0] = 0;
			colptr[0] = 0;
			colptr[1] = 1;
			elts[0] = 2.0 * lambda[0];
		} else if (row == 2 && col == 2) {
			rowidx[0] = 0;
			colptr[0] = 0;
			colptr[1] = 1;
			elts[0] = 2.0 * lambda[0];
		} else if (row == 1 && col == 0) { //parent contribution
			rowidx[0] = 0;
			rowidx[1] = 1;
			rowidx[2] = 1;
			colptr[0] = 0;
			colptr[1] = 2;
			colptr[2] = 3;
			elts[0] = 0.0;
			elts[1] = 0.0;
			elts[2] = 2.0 * lambda[0];
		} else if (row == 2 && col == 0) { //parent contribution
			rowidx[0] = 0;
			rowidx[1] = 1;
			rowidx[2] = 1;
			colptr[0] = 0;
			colptr[1] = 2;
			colptr[2] = 3;
			elts[0] = 2.0 * lambda[0];
			elts[1] = 0.0;
			elts[2] = 0.0;
		} else if (row == 0 && col == 1) {
			rowidx[0] = 0;
			rowidx[1] = 0;
			colptr[0] = 0;
			colptr[1] = 1;
			colptr[2] = 2;
			elts[0] = 1.0 + 1.0*lambda[0];
			elts[0] = 1.0;
		} else if (row == 0 && col == 2) {
			rowidx[0] = 0;
			rowidx[1] = 0;
			colptr[0] = 0;
			colptr[1] = 1;
			colptr[2] = 2;
			elts[0] = 1.0;
			elts[0] = 1.0 + 1.0*lambda[0];
		}
	}

	return 1;
}

int str_write_solution(double* x, double* lam_eq, double* lam_ieq,CallBackDataPtr cbd)
{
	int row = cbd->row_node_id;
	int col = cbd->col_node_id;
	if(row == 0)
	{
	}
	else if(row == 1 || row == 2)
	{
	}
	return 1;
}

/*
  SCOPFLOWGetConstraintJacobianNonzeros - Gets the number of nonzeros in the lagrangian hessian matrix

  Input Parameters:
. scopflow - the security constrained optimal power flow application object

  Output Parameters:
. nnz - number of nonzeros in the lagrangian hessian

  Notes:
  SCOPFLOWSetUp() must be called before calling this routine.
*/
PetscErrorCode SCOPFLOWGetLagrangianHessianNonzeros(SCOPFLOW scopflow,PetscInt *nnz)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;

  *nnz = 0;

  PetscFunctionReturn(0);
}

/*
  SCOPFLOWSetConstraintJacobianLocations - Sets the row and column nonzero locations for the
              lagrangian hessian

  Input Parameters:
+ scopflow - the SCOPFLOW object

  Output Parameters:
+ row - the row locations
- col - the col locations

  Notes:
   The row and column locations should be such that H(row[i],col[i]) = val
*/
PetscErrorCode SCOPFLOWSetLagrangianHessianLocations(SCOPFLOW scopflow, PetscInt *row, PetscInt *col)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;

  PetscFunctionReturn(0);
}

/*
  SCOPFLOWSetHessianValues - Sets the nonzero values for the
              Lagrangian Hessian

  Input Parameters:
+ scopflow - the SCOPFLOW object
- X      - the current iterate


  Output Parameters:
. values - the nonzero values in the Lagrangian Hessian

  Notes:
   The values should be in the same row and col order as set in SCOPFLOWSetLagrangianHessianLocations
*/
PetscErrorCode SCOPFLOWSetLagrangianHessianValues(SCOPFLOW scopflow, PetscScalar obj_factor, Vec X, Vec Lambda, PetscScalar *values)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;

  PetscFunctionReturn(0);
}

/*
  SCOPFLOWCreate - Creates an security constrained optimal power flow application object

  Input Parameters
. mpicomm - The MPI communicator

  Output Parameters
. scopflowout - The security constrained optimal power flow application object
*/
PetscErrorCode SCOPFLOWCreate(MPI_Comm mpicomm, SCOPFLOW *scopflowout)
{
  PetscErrorCode ierr;
  SCOPFLOW       scopflow;

  PetscFunctionBegin;
  ierr = PetscCalloc1(1,&scopflow);CHKERRQ(ierr);

  ierr = COMMCreate(mpicomm,&scopflow->comm);CHKERRQ(ierr);

  scopflow->ns              = -1;
  scopflow->Ns              = -1;
  scopflow->nc              =  0;
  scopflow->Nc              =  0;
  scopflow->nx              =  0;
  scopflow->Nx              =  0;

  scopflow->nlp_pips       = NULL;

  //  gmyid = scopflow->comm->rank;

  scopflow->setupcalled = PETSC_FALSE;
  
  *scopflowout = scopflow;
  PetscFunctionReturn(0);
}

/*
  SCOPFLOWDestroy - Destroys the security constrained optimal power flow application object

  Input Parameter
. scopflow - The SCOPFLOW object to destroy
*/
PetscErrorCode SCOPFLOWDestroy(SCOPFLOW *scopflow)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  ierr = COMMDestroy(&(*scopflow)->comm);CHKERRQ(ierr);

  //  FreeIpoptProblem((*scopflow)->nlp_ipopt);CHKERRQ(ierr);
  ierr = PetscFree(*scopflow);CHKERRQ(ierr);
  *scopflow = 0;

  PetscFunctionReturn(0);
}

/*
  SCOPFLOWSetNetworkData - Sets and reads the network data

  Input Parameter
+  scopflow - The SCOPFLOW object
-  netfile - The name of the network file

  Notes: The input data must be in MATPOWER data format
*/
PetscErrorCode SCOPFLOWSetNetworkData(SCOPFLOW scopflow,const char netfile[])
{
  PetscErrorCode ierr;
  OPFLOW         *opflows;
  PetscInt       i;

  PetscFunctionBegin;
  if(scopflow->ns == -1) {
    SETERRQ(scopflow->comm->type,0,"Must call SCOPFLOWSetNumScenarios or SCOPFLOWSetScenariosFile before calling this function\n");
  }
  ierr = PetscMalloc1(scopflow->ns,&opflows);CHKERRQ(ierr);
  scopflow->opflow = opflows;
  for(i=0; i < scopflow->ns; i++) {
    /* Create OPFLOW object for each scenario */
    ierr = OPFLOWCreate(PETSC_COMM_SELF,&opflows[i]);CHKERRQ(ierr);
    /* Have the OPFLOW object read MatPower data file */
    ierr = OPFLOWReadMatPowerData(opflows[i],netfile);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

/*
  SCOPFLOWCreateGlobalVector - Returns a global vector of the appropriate size
  and distribution conforming to the distribution of the PS object.

  Input Paramereters:
. scopflow - the security constrained optimal power flow application object

  Output Parameters:
. vec - the global vector

  Notes:
  SCOPFLOWSetUp() must be called before calling this routine.
*/
PetscErrorCode SCOPFLOWCreateGlobalVector(SCOPFLOW scopflow,Vec *vec)
{
  PetscErrorCode ierr;
  PetscInt       nx;
  OPFLOW         *opflows=scopflow->opflow;

  PetscFunctionBegin;
  if(!scopflow->setupcalled) SETERRQ(scopflow->comm->type,0,"SCOPFLOWSetUp() must be called before calling SCOPFLOWCreateGlobalVector");
  
  nx = opflows[0]->nvar*scopflow->ns;
  ierr = VecCreate(scopflow->comm->type,&scopflow->X);CHKERRQ(ierr);
  ierr = VecSetSizes(scopflow->X,nx,PETSC_DECIDE);CHKERRQ(ierr);
  ierr = VecSetFromOptions(scopflow->X);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

/*
  SCOPFLOWCreateConstraintJacobian - Returns a distributed matrix of appropriate size that can
                                   be used as the Jacobian


  Input Paramereters:
. scopflow - the security constrained optimal power flow application object

  Output Parameters:
. mat - the jacobian of the constraints

  Notes:
  SCOPFLOWSetUp() must be called before calling this routine.
*/
PetscErrorCode SCOPFLOWCreateConstraintJacobian(SCOPFLOW scopflow,Mat *mat)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;

  PetscFunctionReturn(0);
}

/*
  SCOPFLOWGetConstraintJacobianNonzeros - Gets the number of nonzeros in the constraint jacobian matrix

  Input Paramereters:
. scopflow - the security constrained optimal power flow application object

  Output Parameters:
. nnz - number of nonzeros in the constraint Jacobian

  Notes:
  SCOPFLOWSetUp() must be called before calling this routine.
*/
PetscErrorCode SCOPFLOWGetConstraintJacobianNonzeros(SCOPFLOW scopflow,PetscInt *nnz)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;

  *nnz = 0;

  PetscFunctionReturn(0);
}

/*
  SCOPFLOWSetVariableandConstraintBounds - Sets the bounds on variables and constraints

  Input Parameters:
. scopflow - the SCOPFLOW object

  Output Parameters:
+ Xl     - vector of lower bound on variables
. Xu     - vector of upper bound on variables
. Gl     - vector of lower bound on constraints
. Gu     - vector of lower bound on constraints
*/
PetscErrorCode SCOPFLOWSetVariableandConstraintBounds(SCOPFLOW scopflow, Vec Xl, Vec Xu, Vec Gl, Vec Gu)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;

  PetscFunctionReturn(0);
}

/*
  SCOPFLOWSetInitialGuess - Sets the initial guess for the optimization

  Input Parameters:
. scopflow - the SCOPFLOW object

  Output Parameters:
+ X     - initial guess

  Notes:
   Sets X[i] = (Xl[i] + Xu[i])/2
*/
PetscErrorCode SCOPFLOWSetInitialGuess(SCOPFLOW scopflow, Vec X)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;

  PetscFunctionReturn(0);
}

PetscErrorCode SCOPFLOWConstraintFunction(SCOPFLOW,Vec,Vec);

/*
  SCOPFLOWSolve - Solves the AC security constrained optimal power flow

  Input Parameters:
. scopflow - the security constrained optimal power flow application object
*/
PetscErrorCode SCOPFLOWSolve(SCOPFLOW scopflow)
{
  PetscErrorCode ierr;
  PetscScalar    *xl,*xu,*cl,*cu;
  char           hessiantype[100];
  PetscBool      flg;

  PetscFunctionBegin;
  if(!scopflow->setupcalled) {
    ierr = SCOPFLOWSetUp(scopflow);CHKERRQ(ierr);
  }

  /* Set bounds on variables and constraints */
  ierr = SCOPFLOWSetVariableandConstraintBounds(scopflow,scopflow->Xl,scopflow->Xu,scopflow->Cl,scopflow->Cu);CHKERRQ(ierr);

  ierr = VecGetArray(scopflow->Xl,&xl);CHKERRQ(ierr);
  ierr = VecGetArray(scopflow->Xu,&xu);CHKERRQ(ierr);
  ierr = VecGetArray(scopflow->Cl,&cl);CHKERRQ(ierr);
  ierr = VecGetArray(scopflow->Cu,&cu);CHKERRQ(ierr);

  /* Create PIPS problem */

  ierr = VecRestoreArray(scopflow->Xl,&xl);CHKERRQ(ierr);
  ierr = VecRestoreArray(scopflow->Xu,&xu);CHKERRQ(ierr);
  ierr = VecRestoreArray(scopflow->Cl,&cl);CHKERRQ(ierr);
  ierr = VecRestoreArray(scopflow->Cu,&cu);CHKERRQ(ierr);

  /* Set Initial Guess */
  ierr = SCOPFLOWSetInitialGuess(scopflow,scopflow->X);CHKERRQ(ierr);

  Vec C;
  ierr = VecDuplicate(scopflow->C,&C);CHKERRQ(ierr);
  ierr = SCOPFLOWConstraintFunction(scopflow,scopflow->X,C);CHKERRQ(ierr);

  PetscScalar *x;
  ierr = VecGetArray(scopflow->X,&x);CHKERRQ(ierr);

  /* Solve */
  //  scopflow->solve_status = IpoptSolve(scopflow->nlp_ipopt,x,NULL,&scopflow->obj,NULL,NULL,NULL,scopflow);

  ierr = VecRestoreArray(scopflow->X,&x);CHKERRQ(ierr);

  if(!scopflow->comm->rank) {
    ierr = PetscPrintf(PETSC_COMM_SELF,"Objective function value = %lf\n",scopflow->obj);CHKERRQ(ierr);
  }
  ierr = VecView(scopflow->X,0);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

/*
  SCOPFLOWSetUp - Sets up an security constrained optimal power flow application object

  Input Parameters:
. scopflow - the SCOPFLOW object

  Notes:
  This routine sets up the SCOPFLOW object and the underlying PS object. It
  also distributes the PS object when used in parallel.
*/
PetscErrorCode SCOPFLOWSetUp(SCOPFLOW scopflow)
{
  PetscErrorCode ierr;
  OPFLOW         *opflows=scopflow->opflow;
  PetscInt       ngen; /* Number of generators */
  PetscInt       i,cloc_size;

  PetscFunctionBegin;
  scopflow->nx = 0;
  /* Set up OPFLOW objects for scenarios */
  for(i=0; i < scopflow->ns; i++) {
    ierr = OPFLOWSetUp(opflows[i]);CHKERRQ(ierr);
  }

  str_init_x0_cb init_x0 = &str_init_x0;
  str_prob_info_cb prob_info = &str_prob_info;
  str_eval_f_cb eval_f = &str_eval_f;
  str_eval_g_cb eval_g = &str_eval_g;
  str_eval_grad_f_cb eval_grad_f = &str_eval_grad_f;
  str_eval_jac_g_cb eval_jac_g = &str_eval_jac_g;
  str_eval_h_cb eval_h = &str_eval_h;
  str_write_solution_cb write_solution = &str_write_solution;

  if(scopflow->ns) {
    scopflow->nx = opflows[0]->n;
    ierr = PSGetNumGenerators(opflows[0]->ps,&ngen,NULL);CHKERRQ(ierr);
    scopflow->nd = ngen;
  }

  /* Create vectors X,Xloc, and X0 */
  ierr = VecCreate(PETSC_COMM_SELF,&scopflow->Xloc);CHKERRQ(ierr);
  ierr = VecSetSizes(scopflow->Xloc,scopflow->ns*scopflow->nx,scopflow->ns*scopflow->nx);CHKERRQ(ierr);
  ierr = VecSetFromOptions(scopflow->Xloc);CHKERRQ(ierr);

  ierr = VecDuplicate(scopflow->opflow[0]->X,&scopflow->X0);CHKERRQ(ierr);

  ierr = VecCreate(scopflow->comm->type,&scopflow->X);CHKERRQ(ierr);
  ierr = VecSetSizes(scopflow->X,scopflow->ns*scopflow->nx,PETSC_DECIDE);CHKERRQ(ierr);
  ierr = VecSetFromOptions(scopflow->X);CHKERRQ(ierr);

  ierr = VecDuplicate(scopflow->X,&scopflow->Xl);CHKERRQ(ierr);
  ierr = VecDuplicate(scopflow->X,&scopflow->Xu);CHKERRQ(ierr);

  /* Create vectors for constraints */
  /* Vector for coupling constraints */
  ierr = VecCreate(PETSC_COMM_SELF,&scopflow->Di);CHKERRQ(ierr);
  ierr = VecSetSizes(scopflow->Di,scopflow->nd,PETSC_DECIDE);CHKERRQ(ierr);
  ierr = VecSetFromOptions(scopflow->Di);CHKERRQ(ierr);

  /* Vector for opflow + coupling constraints for one scenario */
  ierr = VecCreate(PETSC_COMM_SELF,&scopflow->Ci);CHKERRQ(ierr);
  ierr = VecSetSizes(scopflow->Ci,scopflow->nd+scopflow->opflow[0]->m,PETSC_DECIDE);CHKERRQ(ierr);
  ierr = VecSetFromOptions(scopflow->Ci);CHKERRQ(ierr);

  /* Vector for opflow + coupling constraints for ns scenarios */
  if(!scopflow->comm->rank) cloc_size = scopflow->ns*scopflow->opflow[0]->m + (scopflow->ns-1)*scopflow->nd;
  else cloc_size =  scopflow->ns*(scopflow->opflow[0]->m + scopflow->nd);
  ierr = VecCreate(PETSC_COMM_SELF,&scopflow->Cloc);CHKERRQ(ierr);
  ierr = VecSetSizes(scopflow->Cloc,cloc_size,cloc_size);CHKERRQ(ierr);
  ierr = VecSetFromOptions(scopflow->Cloc);CHKERRQ(ierr);

  /* Global parallel vector for constraints for Ns scenarios */
  ierr = VecCreate(scopflow->comm->type,&scopflow->C);CHKERRQ(ierr);
  ierr = VecSetSizes(scopflow->C,cloc_size,PETSC_DECIDE);CHKERRQ(ierr);
  ierr = VecSetFromOptions(scopflow->C);CHKERRQ(ierr);

  /* Constraint bounds vectors  */
  ierr = VecDuplicate(scopflow->C,&scopflow->Cl);CHKERRQ(ierr);
  ierr = VecDuplicate(scopflow->C,&scopflow->Cu);CHKERRQ(ierr);
  
  scopflow->setupcalled = PETSC_TRUE;
  PetscFunctionReturn(0);
}

/*
  SCOPFLOWObjectiveFunction - The objective function for the security constrained optimal power flow

  Input Parameters:
+ scopflow - the SCOPFLOW object
. X      - the current iterate

  Output Parameters:
. obj - the objective function value (scalar)
*/
PetscErrorCode SCOPFLOWObjectiveFunction(SCOPFLOW scopflow,Vec X, PetscScalar* obj)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;

  PetscFunctionReturn(0);
}

/*
  SCOPFLOWObjGradientFunction - The gradient of the objective function for the security constrained optimal power flow

  Input Parameters:
+ scopflow - the SCOPFLOW object
. X      - the current iterate

  Output Parameters:
. grad - the objective function gradient
*/
PetscErrorCode SCOPFLOWObjGradientFunction(SCOPFLOW scopflow,Vec X, Vec grad)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;

  PetscFunctionReturn(0);
}

/*
  SCOPFLOWConstraintFunction - Evalulates the constraints for the security constrained optimal power flow

  Input Parameters:
+ scopflow - the SCOPFLOW object
. X      - the current iterate

  Output Parameters:
. G  - vector of constraints
*/
PetscErrorCode SCOPFLOWConstraintFunction(SCOPFLOW scopflow,Vec X,Vec C)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;

  PetscFunctionReturn(0);
}

/*
  SCOPFLOWSetConstraintJacobianLocations - Sets the row and column nonzero locations for the
              constraint Jacobian

  Input Parameters:
+ scopflow - the SCOPFLOW object

  Output Parameters:
+ row - the row locations
- col - the col locations

  Notes:
   The row and column locations should be such that J(row[i],col[i]) = val
*/
PetscErrorCode SCOPFLOWSetConstraintJacobianLocations(SCOPFLOW scopflow, PetscInt *row, PetscInt *col)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;

  PetscFunctionReturn(0);
}

/*
  SCOPFLOWSetConstraintJacobianValues - Sets the nonzero values for the
              constraint Jacobian

  Input Parameters:
+ scopflow - the SCOPFLOW object
- X      - the current iterate


  Output Parameters:
. values - the nonzero values in the constraint jacobian

  Notes:
   The values should be in the same row and col order as set in SCOPFLOWSetConstraintJacobianLocations
*/
PetscErrorCode SCOPFLOWSetConstraintJacobianValues(SCOPFLOW scopflow, Vec X,PetscScalar *values)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;

  PetscFunctionReturn(0);
}

/*
  SCOPFLOWSetNumScenarios - Sets the total number of scenarios in the SCOPF problem

  Input Parameters:
+ scopflow - the security constrained optimal power flow application object
- Ns       - the number of scenarios

  Notes: The total number of scenarios set by SCOPFLOW is actually Ns+1,
  i.e., Ns scenarios + 1 base-case
*/
PetscErrorCode SCOPFLOWSetNumScenarios(SCOPFLOW scopflow,PetscInt Ns)
{
  PetscErrorCode ierr;
  COMM           comm=scopflow->comm;

  PetscFunctionBegin;
  scopflow->Ns = Ns;
  ierr = PetscOptionsGetInt(NULL,NULL,"-scopflow_Ns",&scopflow->Ns,NULL);CHKERRQ(ierr);
  
  ierr = PetscPrintf(PETSC_COMM_WORLD,"SCOPFLOW running with %d scenarios\n",scopflow->Ns);CHKERRQ(ierr);

  PetscFunctionReturn(0);
}


  
