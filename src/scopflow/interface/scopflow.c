#include <private/opflowimpl.h>
#include <private/scopflowimpl.h>

/*
  SCOPFLOWCreate - Creates a security constrained optimal power flow application object

  Input Parameters
. mpicomm - The MPI communicator

  Output Parameters
. scopflowout - The security constrained optimal power flow application object
*/
PetscErrorCode SCOPFLOWCreate(MPI_Comm mpicomm, SCOPFLOW *scopflowout)
{
  PetscErrorCode ierr;
  SCOPFLOW         scopflow;

  PetscFunctionBegin;
  ierr = PetscCalloc1(1,&scopflow);CHKERRQ(ierr);

  ierr = COMMCreate(mpicomm,&scopflow->comm);CHKERRQ(ierr);

  scopflow->Nconeq   = scopflow->nconeq   = 0;
  scopflow->Nconineq = scopflow->nconineq = 0;
  scopflow->Ncon     = scopflow->ncon     = 0;
  scopflow->Nx       = scopflow->nx       = 0;
  scopflow->Nc       = scopflow->nc       = 0;
  scopflow->Gi       = NULL;
  scopflow->Lambdai  = NULL;
  
  scopflow->obj_factor = 1.0;
  scopflow->obj = 0.0;

  scopflow->solver   = NULL;

  scopflow->nsolversregistered = 0;
  scopflow->SCOPFLOWSolverRegisterAllCalled = PETSC_FALSE;

  /* Register all solvers */
  ierr = SCOPFLOWSolverRegisterAll(scopflow);

  /* Run-time options */
  scopflow->iscoupling = PETSC_FALSE;
  scopflow->replicate_basecase = PETSC_FALSE;

  scopflow->mode = SCOPFLOWMODE_CORRECTIVE;

  scopflow->solutiontops = PETSC_FALSE;

  scopflow->ctgcfileset = PETSC_FALSE;
  scopflow->setupcalled = PETSC_FALSE;
  *scopflowout = scopflow;

  ierr = PetscPrintf(scopflow->comm->type,"SCOPFLOW: Application created\n");
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
  PetscInt       c;

  PetscFunctionBegin;
  ierr = COMMDestroy(&(*scopflow)->comm);CHKERRQ(ierr);

  /* Solution vector */
  ierr = VecDestroy(&(*scopflow)->X);CHKERRQ(ierr);
  ierr = VecDestroy(&(*scopflow)->localX);CHKERRQ(ierr);
  ierr = VecDestroy(&(*scopflow)->gradobj);CHKERRQ(ierr);

  /* Lower and upper bounds on X */
  ierr = VecDestroy(&(*scopflow)->Xl);CHKERRQ(ierr);
  ierr = VecDestroy(&(*scopflow)->Xu);CHKERRQ(ierr);

  /* Constraints vector */
  ierr = VecDestroy(&(*scopflow)->G);CHKERRQ(ierr);
  ierr = VecDestroy(&(*scopflow)->Ge);CHKERRQ(ierr);
  ierr = VecDestroy(&(*scopflow)->Gelocal);CHKERRQ(ierr);
  ierr = VecDestroy(&(*scopflow)->Lambdae);CHKERRQ(ierr);
  ierr = VecDestroy(&(*scopflow)->Lambdaelocal);CHKERRQ(ierr);
  if((*scopflow)->Nconineq) {
    ierr = VecDestroy(&(*scopflow)->Gi);CHKERRQ(ierr);
    ierr = VecDestroy(&(*scopflow)->Lambdai);CHKERRQ(ierr);
  }
  ierr = VecDestroy(&(*scopflow)->Gl);CHKERRQ(ierr);
  ierr = VecDestroy(&(*scopflow)->Gu);CHKERRQ(ierr);
  ierr = VecDestroy(&(*scopflow)->Lambda);CHKERRQ(ierr);

  /* Jacobian of constraints */
  ierr = MatDestroy(&(*scopflow)->Jac);CHKERRQ(ierr);
  ierr = MatDestroy(&(*scopflow)->Jac_Ge);CHKERRQ(ierr);
  ierr = MatDestroy(&(*scopflow)->Jac_Gi);CHKERRQ(ierr);

  ierr = MatDestroy(&(*scopflow)->Hes);CHKERRQ(ierr);

  if((*scopflow)->solverops.destroy) {
    ierr = ((*scopflow)->solverops.destroy)(*scopflow);
  }

  /* Destroy OPFLOW objects */
  for(c=0; c < (*scopflow)->nc; c++) {
    ierr = OPFLOWDestroy(&(*scopflow)->opflows[c]);CHKERRQ(ierr);
  }

  ierr = PetscFree((*scopflow)->nconineqcoup);CHKERRQ(ierr);
  ierr = PetscFree((*scopflow)->ctgclist.cont);CHKERRQ(ierr);
  ierr = PetscFree((*scopflow)->opflows);CHKERRQ(ierr);
  ierr = PetscFree(*scopflow);CHKERRQ(ierr);
  //  *scopflow = 0;
  PetscFunctionReturn(0);
}

/*
  SCOPFLOWSetSolver - Sets the solver for SCOPFLOW

  Input Parameters:
+ scopflow - scopflow application object
- solvername - name of the solver
*/
PetscErrorCode SCOPFLOWSetSolver(SCOPFLOW scopflow,const char* solvername)
{
  PetscErrorCode ierr,(*r)(SCOPFLOW)=NULL;
  PetscInt       i;
  PetscFunctionBegin;
  PetscBool match;
  for(i=0;i < scopflow->nsolversregistered;i++) {
    ierr = PetscStrcmp(scopflow->SCOPFLOWSolverList[i].name,solvername,&match);CHKERRQ(ierr);
    if(match) {
      r = scopflow->SCOPFLOWSolverList[i].create;
      break;
    }
  }

  if(!r) SETERRQ1(PETSC_COMM_SELF,PETSC_ERR_ARG_UNKNOWN_TYPE,"Unknown type for SCOPFLOW Solver %s",solvername);

  /* Initialize (Null) the function pointers */
  scopflow->solverops.destroy = 0;
  scopflow->solverops.solve   = 0;
  scopflow->solverops.setup   = 0;

  ierr = PetscStrcpy(scopflow->solvername,solvername);CHKERRQ(ierr);
  /* Call the underlying implementation constructor */
  ierr = (*r)(scopflow);CHKERRQ(ierr);

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

  PetscFunctionBegin;

  ierr = PetscMemcpy(scopflow->netfile,netfile,100*sizeof(char));CHKERRQ(ierr);

  PetscFunctionReturn(0);
}


/*
  SCOPFLOWSetMode - Sets the operation mode (preventive or corrective) for SCOPFLOW

  Input Parameters:
+ scopflow - the SCOPFLOW object
- mode     - the operation mode (SCOPFLOWMODE_PREVENTIVE or SCOPFLOWMODE_CORRECTIVE)

  Notes: 
    In the preventive mode, Pgi-Pg0 = 0,i \forall i \in Nc, i.e., the base case real
    power generation is same as the real power generation for contingency cases.
    In the corrective mode, the real power generation is allowed to deviate from the
    base case limited by 30-min ramp. rate, i.e., -ramp_30 \le Pg,i - Pg,0 \le ramp_30
*/
PetscErrorCode SCOPFLOWSetMode(SCOPFLOW scopflow,PetscInt mode)
{
  PetscFunctionBegin;
  if(mode != 0 || mode != 0) {
    SETERRQ(PETSC_COMM_SELF,PETSC_ERR_SUP,"Incorrect mode in SCOPFLOWSetMode, Use 0 for preventive OR 1 for corrective mode\n");
  }
  scopflow->mode = mode;
  PetscFunctionReturn(0);
}

/*
  SCOPFLOWGetMode - Gets the operation mode (preventive or corrective) for SCOPFLOW

  Input Parameters:
. scopflow - the SCOPFLOW object

  Output Parameters:
. mode     - the operation mode (SCOPFLOWMODE_PREVENTIVE (0) or SCOPFLOWMODE_CORRECTIVE (1))

*/
PetscErrorCode SCOPFLOWGetMode(SCOPFLOW scopflow,PetscInt *mode)
{
  PetscFunctionBegin;
  *mode = scopflow->mode;
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
  PetscBool      opflowsolverset,scopflowsolverset;
  char           opflowmodelname[32]="POWER_BALANCE_POLAR";
  char           opflowsolvername[32]="IPOPT";
  char           scopflowsolvername[32]="IPOPT";
  PetscInt       c,j;
  PS             ps;
  PetscBool      flg;

  PetscFunctionBegin;

  ierr = PetscOptionsBegin(scopflow->comm->type,NULL,"SCOPFLOW options",NULL);CHKERRQ(ierr);
  ierr = PetscOptionsString("-scopflow_solver","SCOPFLOW solver type","",scopflowsolvername,scopflowsolvername,32,&scopflowsolverset);CHKERRQ(ierr);

  ierr = PetscOptionsBool("-scopflow_iscoupling","Include coupling between first stage and second stage","",scopflow->iscoupling,&scopflow->iscoupling,NULL);CHKERRQ(ierr);
  ierr = PetscOptionsInt("-scopflow_Nc","Number of second stage scenarios","",scopflow->Nc,&scopflow->Nc,NULL);CHKERRQ(ierr);
  ierr = PetscOptionsBool("-scopflow_replicate_basecase","Only for debugging: Replicate first stage for all second stage scenarios","",scopflow->replicate_basecase,&scopflow->replicate_basecase,NULL);CHKERRQ(ierr);
  ierr = PetscOptionsInt("-scopflow_mode","Operation mode:Preventive (0) or Corrective (1)","",scopflow->mode,&scopflow->mode,NULL);CHKERRQ(ierr);
  
  PetscOptionsEnd();

  if(scopflow->ctgcfileset && !scopflow->replicate_basecase) {
    if(scopflow->Nc < 0) scopflow->Nc = MAX_CONTINGENCIES;
    else scopflow->Nc += 1; 

    ierr = PetscCalloc1(scopflow->Nc,&scopflow->ctgclist.cont);CHKERRQ(ierr);
    for(c=0; c < scopflow->Nc; c++) scopflow->ctgclist.cont->noutages = 0;
    ierr = SCOPFLOWReadContingencyData(scopflow,scopflow->ctgcfileformat,scopflow->ctgcfile);CHKERRQ(ierr);
    scopflow->Nc = scopflow->ctgclist.Ncont+1;
  } else {
    if(scopflow->Nc == -1) scopflow->Nc = 1;
  }

  int q = scopflow->Nc/scopflow->comm->size;
  int d = scopflow->Nc%scopflow->comm->size;
  if(d) {
    scopflow->nc = q + ((scopflow->comm->rank < d)?1:0); 
  } else {
    scopflow->nc = q;
  }
  int cstart,cend;
  ierr = MPI_Scan(&scopflow->nc,&scopflow->cend,1,MPIU_INT,MPI_SUM,scopflow->comm->type);CHKERRQ(ierr);
  scopflow->cstart = scopflow->cend - scopflow->nc;

  ierr = PetscPrintf(scopflow->comm->type,"SCOPFLOW running with %d contingencies (base case + %d scenarios)\n",scopflow->Nc,scopflow->Nc-1);CHKERRQ(ierr);
  ierr = PetscPrintf(PETSC_COMM_SELF,"Rank %d has %d contingencies, range [%d -- %d]\n",scopflow->comm->rank,scopflow->nc,scopflow->cstart,scopflow->cend);CHKERRQ(ierr);

  /* Set solver */
  if(scopflowsolverset) {
    if(scopflow->solver) ierr = (*scopflow->solverops.destroy)(scopflow);
    ierr = SCOPFLOWSetSolver(scopflow,scopflowsolvername);CHKERRQ(ierr);
    ierr = PetscPrintf(scopflow->comm->type,"SCOPFLOW: Using %s solver\n",scopflowsolvername);CHKERRQ(ierr);
  } else {
    if(!scopflow->solver) {
      ierr = SCOPFLOWSetSolver(scopflow,SCOPFLOWSOLVER_IPOPT);CHKERRQ(ierr);
      ierr = PetscPrintf(scopflow->comm->type,"SCOPFLOW: Using %s solver\n",SCOPFLOWSOLVER_IPOPT);CHKERRQ(ierr); 
    }
  }

  /* Create OPFLOW objects */
  ierr = PetscCalloc1(scopflow->nc,&scopflow->opflows);CHKERRQ(ierr);
  for(c=0; c < scopflow->nc; c++) {
    ierr = OPFLOWCreate(PETSC_COMM_SELF,&scopflow->opflows[c]);CHKERRQ(ierr);
    ierr = OPFLOWSetModel(scopflow->opflows[c],opflowmodelname);CHKERRQ(ierr);
    ierr = OPFLOWSetSolver(scopflow->opflows[c],opflowsolvername);CHKERRQ(ierr);

    ierr = OPFLOWReadMatPowerData(scopflow->opflows[c],scopflow->netfile);CHKERRQ(ierr);
    /* Set up the PS object for opflow */
    ps = scopflow->opflows[c]->ps;
    ierr = PSSetUp(ps);CHKERRQ(ierr);

    /* Set contingencies */
    if(scopflow->ctgcfileset && !scopflow->replicate_basecase) {
      Contingency ctgc=scopflow->ctgclist.cont[scopflow->cstart+c];
      for(j=0; j < ctgc.noutages; j++) {
	if(ctgc.outagelist[j].type == GEN_OUTAGE) {
	  PetscInt gbus=ctgc.outagelist[j].bus;
	  char     *gid = ctgc.outagelist[j].id;
	  PetscInt status = ctgc.outagelist[j].status;
	  ierr = PSSetGenStatus(ps,gbus,gid,status);CHKERRQ(ierr);
	}
	if(ctgc.outagelist[j].type == BR_OUTAGE) {
	  PetscInt fbus=ctgc.outagelist[j].fbus;
	  PetscInt tbus=ctgc.outagelist[j].tbus;
	  char     *brid = ctgc.outagelist[j].id;
	  PetscInt status = ctgc.outagelist[j].status;
	  ierr = PSSetLineStatus(ps,fbus,tbus,brid,status);CHKERRQ(ierr);
	}
      }
    }

    /* Set up OPFLOW object */
    ierr = OPFLOWSetUp(scopflow->opflows[c]);CHKERRQ(ierr);
    //    if(scopflow->cstart+c > 0) scopflow->opflows[c]->obj_gencost = PETSC_FALSE; /* No gen. cost minimization for second stage */
  }
  
  ierr = PetscCalloc1(scopflow->nc,&scopflow->nconineqcoup);CHKERRQ(ierr);
  ierr = (*scopflow->solverops.setup)(scopflow);CHKERRQ(ierr);
  ierr = PetscPrintf(scopflow->comm->type,"SCOPFLOW: Setup completed\n");CHKERRQ(ierr);
  
  scopflow->setupcalled = PETSC_TRUE;
  PetscFunctionReturn(0);
}


/*
  SCOPFLOWSolve - Solves the AC security constrained optimal power flow

  Input Parameters:
. scopflow - the security constrained optimal power flow application object
*/
PetscErrorCode SCOPFLOWSolve(SCOPFLOW scopflow)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;

  if(!scopflow->setupcalled) {
    ierr = SCOPFLOWSetUp(scopflow);
  }

  /* Solve */
  ierr = (*scopflow->solverops.solve)(scopflow);CHKERRQ(ierr);

  PetscFunctionReturn(0);
}

/*
  SCOPFLOWGetObjective - Returns the objective function value

  Input Parameters:
+ SCOPFLOW - the SCOPFLOW object
- obj    - the objective function value

  Notes: Should be called after the optimization finishes
*/
PetscErrorCode SCOPFLOWGetObjective(SCOPFLOW scopflow,PetscReal *obj)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  ierr = (*scopflow->solverops.getobjective)(scopflow,obj);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

/*
  SCOPFLOWGetBaseSolution - Returns the SCOPFLOW solution for a given contingency

  Input Parameters:
+ SCOPFLOW - the SCOPFLOW object
. contnum  - Contingency number (0 for base/no-contingency)
- X        - the scopflow solution for the given contingency

  Notes: Should be called after the optimization finishes
*/
PetscErrorCode SCOPFLOWGetSolution(SCOPFLOW scopflow,PetscInt contnum,Vec *X)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  ierr = (*scopflow->solverops.getsolution)(scopflow,contnum,X);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

/*
  SCOPFLOWGetConstraints - Returns the SCOPFLOW constraints for a given contingency

  Input Parameters:
+ SCOPFLOW - the SCOPFLOW object
. contnum  - Contingency number (0 for base/no-contingency)
- G    - the scopflow constraints

  Notes: Should be called after the optimization finishes.
         Equality constraints first followed by inequality constraints
*/
PetscErrorCode SCOPFLOWGetConstraints(SCOPFLOW scopflow,PetscInt contnum,Vec *G)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  ierr = (*scopflow->solverops.getconstraints)(scopflow,contnum,G);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

/*
  SCOPFLOWGetConstraintMultipliers - Returns the SCOPFLOW constraint multipliers for a given contingency

  Input Parameters:
+ SCOPFLOW - the SCOPFLOW object
. contnum  - Contingency number (0 for base/no-contingency)
- G    - the scopflow constraint lagrange multipliers

  Notes: Should be called after the optimization finishes.
    Equality constraint multipliers first followed by inequality constraint multipliers
*/
PetscErrorCode SCOPFLOWGetConstraintMultipliers(SCOPFLOW scopflow,PetscInt contnum,Vec *Lambda)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  ierr = (*scopflow->solverops.getconstraintmultipliers)(scopflow,contnum,Lambda);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

/*
  SCOPFLOWGetConvergenceStatus - Did SCOPFLOW converge?

  Input Parameters:
+ SCOPFLOW - the SCOPFLOW object
- status - PETSC_TRUE if converged, PETSC_FALSE otherwise

  Notes: Should be called after the optimization finishes
*/
PetscErrorCode SCOPFLOWGetConvergenceStatus(SCOPFLOW scopflow,PetscBool *status)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  ierr = (*scopflow->solverops.getconvergencestatus)(scopflow,status);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}


/*
  SCOPFLOWSolutionToPS - Updates the PS struct for base case and contingencies
                       from SCOPFLOW solution

  Input Parameters:
. scopflow - the SCOPFLOW object

  Notes: Updates the different fields in the PS struct from the SCOPFLOW solution
*/
PetscErrorCode SCOPFLOWSolutionToPS(SCOPFLOW scopflow)
{
  PetscErrorCode     ierr;
  PetscInt           i;
  OPFLOW             opflow;

  PetscFunctionBegin;

  ierr = (*opflow->modelops.solutiontops)(opflow);

  scopflow->solutiontops = PETSC_TRUE;
  PetscFunctionReturn(0);
}
