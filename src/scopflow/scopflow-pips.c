#include <private/psimpl.h>
#include <private/opflowimpl.h>
#include <private/scopflowpipsimpl.h>
#include <math.h>

extern int str_eval_g(double*,double*,double*,double*,CallBackDataPtr);
extern int str_eval_jac_g(double*,double*,int*,double*,int*,int*,int*,double*,int*,int*,CallBackDataPtr);
extern int str_eval_h(double*, double*, double*, int*, double*,
		      int*, int*, CallBackDataPtr);
extern int str_eval_f(double*, double*, double*, CallBackDataPtr);
extern int str_eval_grad_f(double*,double*,double*,CallBackDataPtr);



/* Bounds on coupling constraints */
PetscErrorCode SCOPFLOWAddCouplingConstraintBounds(SCOPFLOW scopflow,int row,Vec Gl,Vec Gu)
{
  PetscErrorCode ierr;
  PetscScalar    *gl,*gu;
  PS             ps = scopflow->opflows[row]->ps; /* PS for the scenario */
  PetscInt       gloc; /* starting location for coupling constraints in G vector */
  PetscInt       i;
  PSBUS          bus;

  PetscFunctionBegin;

  gloc = scopflow->opflows[row]->Nconeq + scopflow->opflows[row]->Nconineq;

  ierr = VecGetArray(Gl,&gl);CHKERRQ(ierr);
  ierr = VecGetArray(Gu,&gu);CHKERRQ(ierr);

  for(i=0; i < ps->nbus; i++) {
    PetscInt k;

    bus = &ps->bus[i];

    for(k=0; k < bus->ngen; k++) {
      PSGEN gen;

      ierr = PSBUSGetGen(bus,k,&gen);CHKERRQ(ierr);
      /* Assume Generator can do a full ramp up to its max. capacity */
      gl[gloc] = PETSC_NINFINITY;//gen->pt;
      gu[gloc] = PETSC_INFINITY; //gen->pt;
      gloc += 1;
    }
  }

  ierr = VecRestoreArray(Gl,&gl);CHKERRQ(ierr);
  ierr = VecRestoreArray(Gu,&gu);CHKERRQ(ierr);

  PetscFunctionReturn(0);
}


/*
  SCOPFLOWSetVariableandConstraintBounds - Sets the bounds on variables and constraints

  Input Parameters:
+ scopflow - the SCOPFLOW object
- row      - scenario number

  Output Parameters:
+ Xl     - vector of lower bound on variables
. Xu     - vector of upper bound on variables
. Gl     - vector of lower bound on constraints
. Gu     - vector of lower bound on constraints
*/
PetscErrorCode SCOPFLOWSetVariableandConstraintBounds(SCOPFLOW scopflow, PetscInt row, Vec Xl, Vec Xu, Vec Gl, Vec Gu)
{
  PetscErrorCode ierr;
  OPFLOW         opflow=scopflow->opflows[row];
  PS             ps=opflow->ps;
  PetscScalar    *xl,*xu,*gl,*gu;
  PetscInt       i;
  PSLINE         line;
  PSBUS          bus;
  PetscInt       loc=0,gloc=0;


  PetscFunctionBegin;

  /* Get array pointers */
  ierr = VecGetArray(Xl,&xl);CHKERRQ(ierr);
  ierr = VecGetArray(Xu,&xu);CHKERRQ(ierr);
  ierr = VecGetArray(Gl,&gl);CHKERRQ(ierr);
  ierr = VecGetArray(Gu,&gu);CHKERRQ(ierr);
  
  for(i=0; i < ps->nbus; i++) {
    PetscInt k;

    bus = &ps->bus[i];

    ierr = PSBUSGetVariableLocation(bus,&loc);CHKERRQ(ierr);

    /* Bounds on voltage angles and bounds on real power mismatch equality constraints */
    xl[loc] = -1e6; xu[loc] = 1e6;
    gl[gloc] = 0.0;   gu[gloc] = 0.0;

    /* Bounds on voltage magnitudes and bounds on reactive power mismatch equality constraints */
    xl[loc+1] = bus->Vmin; xu[loc+1] = bus->Vmax;
    gl[gloc+1] = 0.0;       gu[gloc+1] = 0.0;

    if(bus->ide == REF_BUS || bus->ide == ISOLATED_BUS) xl[loc] = xu[loc] = bus->va*PETSC_PI/180.0;
    if(bus->ide == ISOLATED_BUS) xl[loc+1] = xu[loc+1] = bus->vm;
    
    for(k=0; k < bus->ngen; k++) {
      PSGEN gen;

      ierr = PSBUSGetGen(bus,k,&gen);CHKERRQ(ierr);
      loc = loc+2;
      if(!gen->status) xl[loc] = xu[loc] = xl[loc+1] = xu[loc+1] = 0.0;
      else {
	xl[loc] = gen->pb; /* PGmin */
	xu[loc] = gen->pt; /* PGmax */
	xl[loc+1] = gen->qb; /* QGmin */
	xu[loc+1] = gen->qt; /* QGmax */
	/* pb, pt, qb, qt are converted in p.u. in ps.c */
      }
    }
    gloc += 2;
  }

  if(!scopflow->ignore_line_flow_constraints) {
    for(i=0; i < ps->Nbranch; i++) {
      
      line = &ps->line[i];
      
      /* Line flow inequality constraints */
      if(!line->status) gl[gloc] = gu[gloc] = gl[gloc+1] = gu[gloc+1] = 0.0;
      else {
	gl[gloc] = gl[gloc+1] = 0.0; 
	gu[gloc] = gu[gloc+1] = (line->rateA/ps->MVAbase)*(line->rateA/ps->MVAbase);
      }    
      gloc += 2;
    }
  }


  ierr = VecRestoreArray(Xl,&xl);CHKERRQ(ierr);
  ierr = VecRestoreArray(Xu,&xu);CHKERRQ(ierr);
  ierr = VecRestoreArray(Gl,&gl);CHKERRQ(ierr);
  ierr = VecRestoreArray(Gu,&gu);CHKERRQ(ierr);

  PetscFunctionReturn(0);
}

/* Sets the underlying OPFLOW object for each SCOPFLOW scenario
   and first-stage 
*/
PetscErrorCode SCOPFLOWSetUp_OPFLOW(SCOPFLOW scopflow,PetscInt row)
{
  PetscErrorCode ierr;
  PetscInt       ngen;
  PetscInt       ncoup;
  OPFLOW         opflow=scopflow->opflows[row];
  PetscInt       i;

  PetscFunctionBegin;
  /* Set up PS object */
  ierr = PSSetUp(opflow->ps);CHKERRQ(ierr);

  /* Set contingencies */
  if(scopflow->ctgcfileset && !scopflow->replicate_basecase) {
    Contingency ctgc=scopflow->ctgclist.cont[row];
    for(i=0; i < ctgc.noutages; i++) {
      if(ctgc.outagelist[i].type == GEN_OUTAGE) {
	PetscInt gbus=ctgc.outagelist[i].bus;
	char     *gid = ctgc.outagelist[i].id;
	PetscInt status = ctgc.outagelist[i].status;
	ierr = PSSetGenStatus(scopflow->opflows[row]->ps,gbus,gid,status);CHKERRQ(ierr);
      }
      if(ctgc.outagelist[i].type == BR_OUTAGE) {
	PetscInt fbus=ctgc.outagelist[i].fbus;
	PetscInt tbus=ctgc.outagelist[i].tbus;
	char     *brid = ctgc.outagelist[i].id;
	PetscInt status = ctgc.outagelist[i].status;
	ierr = PSSetLineStatus(scopflow->opflows[row]->ps,fbus,tbus,brid,status);CHKERRQ(ierr);
      }
    }
  }
  
  ierr = PSGetNumGenerators(opflow->ps,&ngen,NULL);CHKERRQ(ierr);  

  if(scopflow->iscoupling) ncoup = (row == 0) ? 0:ngen;
  else ncoup = 0;

  opflow->nconeq   = 2*opflow->ps->Nbus;
  opflow->Nconeq   = 2*opflow->ps->Nbus;
  if(scopflow->ignore_line_flow_constraints) {
    opflow->nconineq = 0;
    opflow->Nconineq = 0;
  } else {
    opflow->nconineq = 2*opflow->ps->nbranch;
    opflow->Nconineq = 2*opflow->ps->Nbranch; /* 0 <= Sf^2 <= Smax^2, 0 <= St^2 <= Smax^2 */
  }

  /* Vector to hold solution */
  ierr = PSCreateGlobalVector(opflow->ps,&opflow->X);CHKERRQ(ierr);
  ierr = VecDuplicate(opflow->X,&opflow->localX);CHKERRQ(ierr);
  ierr = VecGetSize(opflow->X,&opflow->Nvar);CHKERRQ(ierr);

  /* Vector to hold gradient */
  ierr = VecDuplicate(opflow->X,&opflow->gradobj);CHKERRQ(ierr);

  /* Create the vector for upper and lower bounds on X */
  ierr = VecDuplicate(opflow->X,&opflow->Xl);CHKERRQ(ierr);
  ierr = VecDuplicate(opflow->X,&opflow->Xu);CHKERRQ(ierr);

  /* Create the equality constraint vector */
  ierr = VecCreate(opflow->comm->type,&opflow->Ge);CHKERRQ(ierr);
  ierr = VecSetSizes(opflow->Ge,opflow->nconeq,PETSC_DETERMINE);CHKERRQ(ierr);
  ierr = VecSetFromOptions(opflow->Ge);CHKERRQ(ierr);


  if(opflow->nconineq+ncoup) {
    /* Create the inequality constraint vector */
    ierr = VecCreate(opflow->comm->type,&opflow->Gi);CHKERRQ(ierr);
    ierr = VecSetSizes(opflow->Gi,opflow->nconineq+ncoup,PETSC_DETERMINE);CHKERRQ(ierr);
    ierr = VecSetFromOptions(opflow->Gi);CHKERRQ(ierr);
  }

  /* Create lower and upper bounds on constraint vector */
  /* Size Nconeq + Nconineq (+Ncoupling for row != 0 only) */
  ierr = VecCreate(opflow->comm->type,&opflow->Gl);CHKERRQ(ierr);
  ierr = VecSetSizes(opflow->Gl,opflow->nconineq+opflow->nconeq+ncoup,PETSC_DETERMINE);CHKERRQ(ierr);
  ierr = VecSetFromOptions(opflow->Gl);CHKERRQ(ierr);
  
  ierr = VecDuplicate(opflow->Gl,&opflow->Gu);CHKERRQ(ierr);

  /* Create Lagrangian multiplier vector */
  ierr = VecDuplicate(opflow->Gl,&opflow->Lambda);CHKERRQ(ierr);

  ierr = DMNetworkSetVertexLocalToGlobalOrdering(opflow->ps->networkdm);CHKERRQ(ierr);

  /* Create Equality Constraint Jacobian */
  ierr = MatCreate(opflow->comm->type,&opflow->Jac_Ge);CHKERRQ(ierr);
  ierr = MatSetSizes(opflow->Jac_Ge,opflow->nconeq,opflow->Nvar,PETSC_DECIDE,PETSC_DECIDE);CHKERRQ(ierr);
  ierr = MatSetType(opflow->Jac_Ge,MATSEQAIJ);CHKERRQ(ierr);
  ierr = MatSetUp(opflow->Jac_Ge);CHKERRQ(ierr);
  ierr = MatSetFromOptions(opflow->Jac_Ge);CHKERRQ(ierr);

  if(opflow->nconineq+ncoup) {
    /* Create Inequality Constraint Jacobian */
    ierr = MatCreate(opflow->comm->type,&opflow->Jac_Gi);CHKERRQ(ierr);
    ierr = MatSetSizes(opflow->Jac_Gi,opflow->nconineq+ncoup,opflow->Nvar,PETSC_DECIDE,PETSC_DECIDE);CHKERRQ(ierr);
    ierr = MatSetType(opflow->Jac_Gi,MATSEQAIJ);CHKERRQ(ierr);
    ierr = MatSetUp(opflow->Jac_Gi);CHKERRQ(ierr);
    ierr = MatSetFromOptions(opflow->Jac_Gi);CHKERRQ(ierr);
  }

  /* Create Hessian */
  /* Use PSCreateMatrix so that we get a symmetric non-zero structure */
  ierr = PSCreateMatrix(opflow->ps,&opflow->Hes);CHKERRQ(ierr);

  PetscFunctionReturn(0);
}

extern PetscErrorCode OPFLOWSetInitialGuess(OPFLOW,Vec);

int str_init_x0(double* x0, CallBackDataPtr cbd) {
  int row = cbd->row_node_id;
  SCOPFLOW scopflow=cbd->prob;
  OPFLOW   opflow = scopflow->opflows[row];
  PetscErrorCode ierr;
  
  ierr = VecPlaceArray(opflow->X,x0);CHKERRQ(ierr);
  ierr = OPFLOWSetInitialGuess(opflow,opflow->X);CHKERRQ(ierr);
  ierr = VecResetArray(opflow->X);CHKERRQ(ierr);
	
  return 1;
}

int str_prob_info(int* n, double* col_lb, double* col_ub, int* m,
		  double* row_lb, double* row_ub, CallBackDataPtr cbd) {
  int row = cbd->row_node_id;
  int col = cbd->col_node_id;
  SCOPFLOW scopflow = (SCOPFLOW)cbd->prob;
  PetscInt rank=scopflow->comm->rank;
  int type = cbd->typeflag;
  PetscErrorCode ierr;
  Vec            Xl,Xu,Gl,Gu;

  if(type == 1) {
    if(row_lb == NULL){
      *m = 0;
    }
    return 1;
  }
  
  /* Set sizes of variables and constraints */
  if(col_lb == NULL){
    /* Create OPFLOW */
    ierr = OPFLOWCreate(PETSC_COMM_SELF,&scopflow->opflows[row]);CHKERRQ(ierr);
    ierr = OPFLOWReadMatPowerData(scopflow->opflows[row],scopflow->netfile);CHKERRQ(ierr);
    /* SCOPFLOWSetUp_OPFLOW handles creating the vectors and the sizes of the constraints */
    ierr = SCOPFLOWSetUp_OPFLOW(scopflow,row);CHKERRQ(ierr);
    if(scopflow->iscoupling && row > 0 && !scopflow->Jcoup) {
      /* Create the constraint Jacobian for coupling */
      ierr = MatDuplicate(scopflow->opflows[row]->Jac_Gi,MAT_DO_NOT_COPY_VALUES,&scopflow->Jcoup);CHKERRQ(ierr);
    
      PS    ps=scopflow->opflows[row]->ps;
      PSBUS bus;
      PetscInt locglob,genctr,k,i;
      PetscInt rowid[2],colid[2];
      PetscScalar val[2];
      PetscInt    gloc=scopflow->opflows[row]->nconineq;
      for(i=0; i < ps->nbus; i++) {
	bus = &ps->bus[i];
	
	ierr = PSBUSGetVariableGlobalLocation(bus,&locglob);CHKERRQ(ierr);
	genctr = 0;
	for(k=0; k < bus->ngen; k++) {
	  val[0] = -1;
	  rowid[0] = gloc;
	  colid[0] = locglob + 2 + genctr;
	  ierr = MatSetValues(scopflow->Jcoup,1,rowid,1,colid,val,ADD_VALUES);CHKERRQ(ierr);
	  genctr += 2;
	  gloc += 1;
	}
      }
      
      ierr = MatAssemblyBegin(scopflow->Jcoup,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
      ierr = MatAssemblyEnd(scopflow->Jcoup,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
      ierr = MatTranspose(scopflow->Jcoup,MAT_INITIAL_MATRIX,&scopflow->JcoupT);CHKERRQ(ierr);
    }

    ierr = VecGetSize(scopflow->opflows[row]->X,n);CHKERRQ(ierr);
    ierr = VecGetSize(scopflow->opflows[row]->Gl,m);CHKERRQ(ierr);
    scopflow->ns++;

    ierr = PetscPrintf(PETSC_COMM_SELF,"Rank %d row %d col %d type %d Nvar=%d Ncon = %d\n",rank,row,col,type,*n,*m);CHKERRQ(ierr);
  } else {
    /* Bounds on variables and constraints */
    Xl = scopflow->opflows[row]->Xl;
    Xu = scopflow->opflows[row]->Xu;
    Gl = scopflow->opflows[row]->Gl;
    Gu = scopflow->opflows[row]->Gu;

    ierr = VecPlaceArray(Xl,col_lb);CHKERRQ(ierr);
    ierr = VecPlaceArray(Xu,col_ub);CHKERRQ(ierr);
    ierr = VecPlaceArray(Gl,row_lb);CHKERRQ(ierr);
    ierr = VecPlaceArray(Gu,row_ub);CHKERRQ(ierr);
    
    ierr = SCOPFLOWSetVariableandConstraintBounds(scopflow,row,Xl,Xu,Gl,Gu);CHKERRQ(ierr);

    
    if(scopflow->iscoupling && row != 0) {
      /* Adding bounds on coupling constraints between first-stage and scenarios */
      ierr = SCOPFLOWAddCouplingConstraintBounds(scopflow,row,Gl,Gu);CHKERRQ(ierr);
    }
    
    ierr = VecResetArray(Xl);CHKERRQ(ierr);
    ierr = VecResetArray(Xu);CHKERRQ(ierr);
    ierr = VecResetArray(Gl);CHKERRQ(ierr);
    ierr = VecResetArray(Gu);CHKERRQ(ierr);
  
    /* Copy over col_lb and col_ub to vectors Xl and Xu so
       that initialization of X can work correctly as
       Xinital = (Xl + Xu)/2
    */
    PetscScalar *xl,*xu;
    ierr = VecGetArray(Xl,&xl);CHKERRQ(ierr);
    ierr = VecGetArray(Xu,&xu);CHKERRQ(ierr);
    ierr = PetscMemcpy(xl,col_lb,scopflow->opflows[row]->Nvar*sizeof(PetscScalar));CHKERRQ(ierr);
    ierr = PetscMemcpy(xu,col_ub,scopflow->opflows[row]->Nvar*sizeof(PetscScalar));CHKERRQ(ierr);
    ierr = VecRestoreArray(Xl,&xl);CHKERRQ(ierr);
    ierr = VecRestoreArray(Xu,&xu);CHKERRQ(ierr);

    ierr = PetscPrintf(PETSC_COMM_SELF,"Rank %d: row = %d, col = %d set up problem info\n",rank,row,col);CHKERRQ(ierr);
  }
  return 1;
}

int str_write_solution(double* x, double* lam_eq, double* lam_ieq,CallBackDataPtr cbd)
{
  int row = cbd->row_node_id;
  if(row == 0) {
  } else if(row == 1 || row == 2) {
  }
  return 1;
}

/*
  SCOPFLOWReadContingencyData - Reads the contingency list data file

  Input Parameters
+ scopflow - the scopflow object
- ctgcfile - the contingency file name

*/
PetscErrorCode SCOPFLOWReadContingencyData(SCOPFLOW scopflow,const char ctgcfile[])
{
  PetscErrorCode ierr;
  FILE           *fp;
  ContingencyList *ctgclist=&scopflow->ctgclist;
  Contingency    *cont;
  Outage         *outage;
  char           line[MAXLINE];
  char           *out;
  PetscInt       bus,fbus,tbus,type,num;
  char           equipid[3];
  PetscInt       status;
  PetscScalar    prob;

  PetscFunctionBegin;

  fp = fopen(ctgcfile,"r");
  if (fp == NULL) {
    SETERRQ1(PETSC_COMM_SELF,PETSC_ERR_FILE_OPEN,"Cannot open file %s",ctgcfile);CHKERRQ(ierr);
  }

  ctgclist->Ncont = -1;
  while((out = fgets(line,MAXLINE,fp)) != NULL) {
    if(strcmp(line,"\r\n") == 0 || strcmp(line,"\n") == 0) {
      continue; /* Skip blank lines */
    }
    sscanf(line,"%d,%d,%d,%d,%d,'%[^\t\']',%d,%lf",&num,&type,&bus,&fbus,&tbus,equipid,&status,&prob);

    if(num == scopflow->Ns+1) break;

    if(num > MAX_CONTINGENCIES) SETERRQ2(PETSC_COMM_SELF,PETSC_ERR_SUP,"Contingency number %d exceeds max. allowed = %d\n",num,MAX_CONTINGENCIES);

    cont   = &ctgclist->cont[num];
    outage = &cont->outagelist[cont->noutages];
    outage->num  = num;
    outage->type = type;
    outage->bus  = bus;
    outage->fbus = fbus;
    outage->tbus = tbus;
    ierr = PetscMemcpy(outage->id,equipid,3*sizeof(char));CHKERRQ(ierr);
    outage->status = status;
    outage->prob   = prob;
    cont->noutages++;
    if(num > ctgclist->Ncont) ctgclist->Ncont = num;
  }
  fclose(fp);

  PetscFunctionReturn(0);
}

/*
  SCOPFLOWSetContingencyData - Sets the contingency data

  Input Parameter
+  scopflow - The SCOPFLOW object
-  ctgcfile - The name of the contingency list file

*/
PetscErrorCode SCOPFLOWSetContingencyData(SCOPFLOW scopflow,const char ctgcfile[])
{
  PetscErrorCode ierr;

  PetscFunctionBegin;

  ierr = PetscMemcpy(scopflow->ctgcfile,ctgcfile,100*sizeof(char));CHKERRQ(ierr);

  scopflow->ctgcfileset = PETSC_TRUE;
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

  scopflow->ns              =  0;
  scopflow->Ns              = -1;
  scopflow->Jcoup           = NULL;
  scopflow->JcoupT          = NULL;
  scopflow->iscoupling      = PETSC_FALSE;
  scopflow->first_stage_gen_cost_only = PETSC_TRUE;
  scopflow->ignore_line_flow_constraints = PETSC_TRUE;
  scopflow->replicate_basecase = PETSC_FALSE;

  scopflow->nlp_pips       = NULL;

  scopflow->ctgcfileset    = PETSC_FALSE;

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

  PetscFunctionBegin;

  ierr = PetscMemcpy(scopflow->netfile,netfile,100*sizeof(char));CHKERRQ(ierr);
  
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
    ierr = SCOPFLOWSetUp(scopflow);CHKERRQ(ierr);
  }

  /* Solve with PIPS */
  PipsNlpSolveStruct(scopflow->nlp_pips);


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
  PetscInt       i;

  PetscFunctionBegin;

  str_init_x0_cb init_x0 = &str_init_x0;
  str_prob_info_cb prob_info = &str_prob_info;
  str_eval_f_cb eval_f = &str_eval_f;
  str_eval_g_cb eval_g = &str_eval_g;
  str_eval_grad_f_cb eval_grad_f = &str_eval_grad_f;
  str_eval_jac_g_cb eval_jac_g = &str_eval_jac_g;
  str_eval_h_cb eval_h = &str_eval_h;
  str_write_solution_cb write_solution = &str_write_solution;

  ierr = PetscOptionsGetBool(NULL,NULL,"-scopflow_iscoupling",&scopflow->iscoupling,NULL);CHKERRQ(ierr);
  ierr = PetscOptionsGetBool(NULL,NULL,"-scopflow_first_stage_gen_cost_only",&scopflow->first_stage_gen_cost_only,NULL);CHKERRQ(ierr);
  ierr = PetscOptionsGetBool(NULL,NULL,"-scopflow_ignore_line_flow_constraints",&scopflow->ignore_line_flow_constraints,NULL);CHKERRQ(ierr);
  ierr = PetscOptionsGetInt(NULL,NULL,"-scopflow_Ns",&scopflow->Ns,NULL);CHKERRQ(ierr);

  ierr = PetscOptionsGetBool(NULL,NULL,"-scopflow_replicate_basecase",&scopflow->replicate_basecase,NULL);CHKERRQ(ierr);


  if(scopflow->ctgcfileset && !scopflow->replicate_basecase) {
    if(scopflow->Ns < 0) scopflow->Ns = MAX_CONTINGENCIES;
    if(scopflow->Ns) {
      ierr = PetscMalloc1(scopflow->Ns+1,&scopflow->ctgclist.cont);CHKERRQ(ierr);
      for(i=0; i < scopflow->Ns+1; i++) scopflow->ctgclist.cont->noutages = 0;
      ierr = SCOPFLOWReadContingencyData(scopflow,scopflow->ctgcfile);CHKERRQ(ierr);
      scopflow->Ns = scopflow->ctgclist.Ncont;
    }
  } else {
    if(scopflow->Ns == -1) scopflow->Ns = 0;
  }

  ierr = PetscPrintf(PETSC_COMM_WORLD,"SCOPFLOW running with %d scenarios\n",scopflow->Ns);CHKERRQ(ierr);

  ierr = PetscMalloc1(scopflow->Ns+1,&scopflow->opflows);CHKERRQ(ierr);

  scopflow->nlp_pips = CreatePipsNlpProblemStruct(MPI_COMM_WORLD, scopflow->Ns,
							    init_x0, prob_info, eval_f, eval_g, eval_grad_f, eval_jac_g,
						  eval_h, write_solution, (UserDataPtr)scopflow);

  scopflow->setupcalled = PETSC_TRUE;
  PetscFunctionReturn(0);
}

/*
  SCOPFLOWSetNumScenarios - Sets the total number of scenarios in the SCOPF problem

  Input Parameters:
+ scopflow - the security constrained optimal power flow application object
- Ns       - the number of scenarios

  Notes: The total number of scenarios set by SCOPFLOW is actually Ns+1,
  i.e., Ns scenarios + 1 base-case
  This routine only selects the top Ns scenarios given in the contingency file
  Can be also set via command line option -scopflow_Ns <Ns>
*/
PetscErrorCode SCOPFLOWSetNumScenarios(SCOPFLOW scopflow,PetscInt Ns)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  scopflow->Ns = Ns;
  
  PetscFunctionReturn(0);
}