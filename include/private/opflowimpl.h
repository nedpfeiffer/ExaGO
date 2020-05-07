/**
 * @file opflowimpl.h
 * @brief Private header file that defines data and structures for Optimal Power Flow application
 */
#ifndef OPFLOWIMPL_H
#define OPFLOWIMPL_H

#include <ps.h>
#include <private/psimpl.h>
#include <pflow.h>
#include <opflow.h>

#define OPFLOWFORMULATIONSMAX 4
#define OPFLOWSOLVERSMAX      3

typedef enum {
  OPFLOWINIT_MIDPOINT, /* Midpoint */
  OPFLOWINIT_FROMFILE, /* From file */
  OPFLOWINIT_ACPF,      /* From AC power flow solution */
  OPFLOWINIT_FLATSTART  /* Voltage flat start */
}OPFLOWInitializationType; 

extern const char *const OPFLOWInitializationTypes[];


struct _p_OPFLOWFormulationOps {
  PetscErrorCode (*destroy)(OPFLOW);
  PetscErrorCode (*setnumvariables)(OPFLOW,PetscInt*,PetscInt*,PetscInt*); /* Set number of variables for buses and branches, and total number of variables */
  PetscErrorCode (*setnumconstraints)(OPFLOW,PetscInt*,PetscInt*,PetscInt*,PetscInt*); /* Set number of equality and inequality constraints */
  PetscErrorCode (*setvariablebounds)(OPFLOW,Vec,Vec); /* Upper and lower bounds on the vector */
  PetscErrorCode (*setconstraintbounds)(OPFLOW,Vec,Vec); /* Lower and upper bounds on constraints */
  PetscErrorCode (*setvariableandconstraintbounds)(OPFLOW,Vec,Vec,Vec,Vec); /* Lower and upper bounds on variables and constraints */
  PetscErrorCode (*setinitialguess)(OPFLOW,Vec); /* Set the initial guess for the optimization */
  PetscErrorCode (*computeequalityconstraints)(OPFLOW,Vec,Vec); /* Set equality constraints */
  PetscErrorCode (*computeinequalityconstraints)(OPFLOW,Vec,Vec); /* Set inequality constraints */
  PetscErrorCode (*computeconstraints)(OPFLOW,Vec,Vec);
  PetscErrorCode (*computeequalityconstraintjacobian)(OPFLOW,Vec,Mat);
  PetscErrorCode (*computeinequalityconstraintjacobian)(OPFLOW,Vec,Mat);
  PetscErrorCode (*computehessian)(OPFLOW,Vec,Vec,Vec,Mat);
  PetscErrorCode (*computeobjandgradient)(OPFLOW,Vec,PetscScalar*,Vec); /* Objective and gradient routine */
  PetscErrorCode (*computeobjective)(OPFLOW,Vec,PetscScalar*); /* Objective */
  PetscErrorCode (*computegradient)(OPFLOW,Vec,Vec); /* Gradient of the objective function */
  PetscErrorCode (*computejacobian)(OPFLOW,Vec,Mat); /* Jacobian of the constraints */
  PetscErrorCode (*solutiontops)(OPFLOW); /* Update PS struct from OPFLOW solution */
};

struct _p_OPFLOWSolverOps {
  PetscErrorCode (*destroy)(OPFLOW);
  PetscErrorCode (*setup)(OPFLOW);
  PetscErrorCode (*solve)(OPFLOW);
  PetscErrorCode (*getobjective)(OPFLOW,PetscReal*);
  PetscErrorCode (*getsolution)(OPFLOW,Vec*);
  PetscErrorCode (*getconvergencestatus)(OPFLOW,PetscBool*);
  PetscErrorCode (*getconstraints)(OPFLOW,Vec*);
  PetscErrorCode (*getconstraintmultipliers)(OPFLOW,Vec*);
};

struct _p_OPFLOWFormulationList{
  char name[32]; /* Name of the formulation */
  PetscErrorCode (*create)(OPFLOW); /* Formulation creation routine */
};

struct _p_OPFLOWSolverList{
  char name[32]; /* Name of the solver */
  PetscErrorCode (*create)(OPFLOW); /* Solver object creation routine */
};


/**
 * @brief private struct for optimal power flow application
 */
struct _p_OPFLOW{
  /* Common fields */
  COMM comm; /**< Communicator context */
  PS   ps;   /**< Power system context */

  Vec  X,localX;    /* Global and local solution vector */
  Vec  G; /**< Inequality and equality constraint function */
  Vec  Ge,Gelocal; /** < Equality constraint function vector (global and local) */
  Vec  Gi; /** < Inequality constraint function vector */

  Mat  Jac; /* Jacobian for equality and inequality constraints */
  Mat  Jac_Ge; /* Equality constraint Jacobian */
  Mat  Jac_Gi; /* Inequality constraint Jacobian */

  Mat  Hes;  /* Lagrangian Hessian */
  
  Vec Xl; /**< Lower bound on solution */
  Vec Xu; /**< Upper bound on solution */

  Vec Gl; /**< Lower bound on G */
  Vec Gu; /**< Upper bound on G */

  PetscScalar obj; /**< Objective function */
  Vec gradobj; /**< Gradient of the objective function */
  
  PetscScalar obj_factor; /* IPOPT scales the objective hessian part with this factor. For all other solvers, unless it is set, obj_factor = 1.0. */

  PetscBool   obj_gencost; /* Allows to conditionally include generator cost in the objective function */

  PetscBool setupcalled; /* OPFLOWSetUp called? */

  OPFLOWInitializationType initializationtype; /* OPFLOW Initialization type */

  PetscInt nconeq, Nconeq;     /* Local and global number of equality constraints, excluding ghosts! */
  PetscInt nconineq, Nconineq; /* Local and global number of inequality constraints */
  PetscInt Ncon,ncon;               /* Total number of constraints (equality + inequality) */
  PetscInt nx,Nx;          /* Total number of local and global variables, excluding ghosts! */

  //  Mat Jac_GeT; /* Transpose of equality constraint Jacobian */
  //  Mat Jac_GiT; /* Transpose of inequality constraint Jacobian */

  /* Lagrange multipliers */
  Vec Lambda,Lambdae,Lambdai; // Lagrange multipliers for constraints Lambda = [Lambdae; Lambdai];
  Vec Lambdaelocal; /* Local Lambdae vector */

  void* solver; /* Solver object */
  struct _p_OPFLOWSolverOps solverops;
  char  solvername[64];

  void* formulation; /* Formulation object */
  struct _p_OPFLOWFormulationOps formops;
  char formulationname[64];

  /* List of formulations and solvers registered */
  struct _p_OPFLOWFormulationList OPFLOWFormulationList[OPFLOWFORMULATIONSMAX];
  struct _p_OPFLOWSolverList OPFLOWSolverList[OPFLOWSOLVERSMAX];
  PetscInt nformulationsregistered;
  PetscInt nsolversregistered;
  PetscBool OPFLOWFormulationRegisterAllCalled;
  PetscBool OPFLOWSolverRegisterAllCalled;

  PetscBool ignore_lineflow_constraints; /* Ignore line flow constraints */

  PetscBool include_loadloss_variables; /* Include variables for loadloss */
  PetscReal loadloss_penalty;

  PetscBool include_powerimbalance_variables; /* Include variables for power imbalance */
  PetscReal powerimbalance_penalty;

  /* Flag to denote if the OPFLOW solution has been transfered to PS struct via OPLOWSolutionToPS call */
  PetscBool solutiontops;

  /* Global indices for the equality constraints. It is used when operating on equality constraints */
  PetscInt *eqconglobloc;

  VecScatter scattereqcon; /* Vector scatter object for scattering from global equality constraints to local equality constraints vector */

  /* Local and global index sets used in VecScatter */
  IS       isconeqlocal;
  IS       isconeqglob;

  /* Used only when initialization from power flow (ACPF) is called */
  PFLOW initpflow; /**< power flow solver context for obtaining initial conditions */
  PetscSection initpflowpsection; /** < PetscSection object to hold dofs at each vertex */
  PetscSection initpflowpglobsection; /** <Global section for initpflow */
  PetscSection defaultsection;      /** < PetscSection used with opflow. This is temporarily stored when running the initial power flow */
  PetscSection defaultglobalsection; /** <Global section used with opflow. This is temporarily stored when running the initial power flow */
  DM           initpflowdm; /* DM used with initial power flow */
  
  PetscInt *busnvararray,*branchnvararray; /* Number of variables at buses and branches */
};

/* Registers all the OPFLOW formulations */
extern PetscErrorCode OPFLOWFormulationRegisterAll(OPFLOW);

/* Register all OPFLOW solvers */
extern PetscErrorCode OPFLOWSolverRegisterAll(OPFLOW);

/* Update PS struct from OPFLOW solution */
extern PetscErrorCode OPFLOWSolutionToPS(OPFLOW);
#endif
