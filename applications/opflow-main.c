static char help[] = "User example calling OPFLOW.\n\n";

#include <opflow.h>
#include <scopflow_config.h>

int main(int argc,char **argv)
{
  PetscErrorCode ierr;
  OPFLOW             opflow;
  OutputFormat       fmt=CSV;
  char              file[PETSC_MAX_PATH_LEN];
  PetscBool         flg;
  PetscReal         obj;
  Vec               X,G,Lambda;
  PetscBool         conv_status;
  #if defined(PETSC_USE_LOG)
    PetscLogStage stages[2];
  #endif
  char options_pathname[200] = SCOPFLOW_OPTIONS_DIR;
  char* filename = "/opflowoptions";
  printf("%s\n", options_pathname);
  printf("%s\n", filename);
  strcat(options_pathname, filename);
  printf("%s\n", options_pathname);

  PetscInitialize(&argc,&argv,options_pathname,help);

  /* PetscLogStageRegister */
  ierr = PetscLogStageRegister("Reading Data",&stages[0]);CHKERRQ(ierr);
  ierr = PetscLogStageRegister("Allocation & Setting",&stages[1]);CHKERRQ(ierr);

  /* Petsc Stage Push 1 */
  ierr = PetscLogStagePush(stages[0]);CHKERRQ(ierr);
  /* Create OPFLOW object */
  ierr = OPFLOWCreate(PETSC_COMM_WORLD,&opflow);CHKERRQ(ierr);

  /* Get network data file from command line */
  ierr = PetscOptionsGetString(NULL,NULL,"-netfile",file,PETSC_MAX_PATH_LEN,&flg);CHKERRQ(ierr);
  /* Read Network Data file */
  if(flg) {
    ierr = OPFLOWReadMatPowerData(opflow,file);CHKERRQ(ierr);
  } else {
    ierr = OPFLOWReadMatPowerData(opflow,"datafiles/case9mod.m");CHKERRQ(ierr);
  }
  /* End of First Stage and Start of Second */
  ierr = PetscLogStagePop();CHKERRQ(ierr);
  ierr = PetscLogStagePush(stages[1]);CHKERRQ(ierr);
  /* Solve */
  ierr = OPFLOWSolve(opflow);CHKERRQ(ierr);

  ierr = OPFLOWPrintSolution(opflow);CHKERRQ(ierr);

  ierr = OPFLOWSaveSolution(opflow,fmt,"opflow.csv");CHKERRQ(ierr);

  /*End of Final Stage */
  ierr = PetscLogStagePop();CHKERRQ(ierr);

  /* Destroy OPFLOW object */
  ierr = OPFLOWDestroy(&opflow);CHKERRQ(ierr);

  PetscFinalize();
  return 0;
}
