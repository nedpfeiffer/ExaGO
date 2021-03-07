#include <common.h>
#include <utils.h>
#include <petscsys.h>
#include <private/utilsimpl.h>
#include <exago_config.h>
#include <version.h>

const char* ExaGOVerbosityNames[] = {"INFO","WARNING","ERROR"};

static char ExaGOCurrentAppName[128];

PetscErrorCode ExaGOInitialize(MPI_Comm comm, int *argc, char ***argv,
    char* appname, char *help)
{
  PetscErrorCode ierr;

  strcpy(ExaGOCurrentAppName,appname);

  int initialized;
  MPI_Initialized(&initialized);
  if(!initialized)
    MPI_Init(argc,argv);

  ExaGOLogSetComm(comm);
  ierr = ExaGOLogInitialize();
  if(ierr)
  {
    fprintf(stderr, "Could not initialize ExaGO logger.\n");
    return ierr;
  }

  FILE *fp;
  ExaGOLogGetLoggingFilePointer(&fp);
  if(fp==NULL)
    ExaGOLogSetLoggingFilePointer(stderr);

  int i;
  char options_pathname[200] = EXAGO_OPTIONS_DIR;
  char filename[64];
  ierr=sprintf(filename,"%soptions",ExaGOCurrentAppName);
  if(ierr<0)
  {
    fprintf(stderr,
        "Got error setting options path for ExaGO application %s.\n",
        ExaGOCurrentAppName);
    return 1;
  }

  const int noptfiles = 4;
  char **optfiles = malloc(sizeof(char*)*noptfiles);
  for(i=0; i<noptfiles; i++)
    optfiles[i] = malloc(PETSC_MAX_PATH_LEN);


  // Get -option_file option from command line
  char options_file[PETSC_MAX_PATH_LEN];
  PetscBool flg = PETSC_FALSE;
  for(i=1; i<*argc; i++)
  {
    // Added null terminator here as this would cause errors with
    if (0==strncmp((*argv)[i],"-options_file\0",13))
    {
      strcpy(optfiles[0],(*argv)[i + 1]);
      flg = PETSC_TRUE;
    }
  } 
  if (!flg)
  {
    strcpy(optfiles[0],"... -options_file not passed");
  }

  sprintf(optfiles[1],"%s/%s",options_pathname,filename);

  ierr=sprintf(optfiles[2],"./%s",filename);
  if(ierr<0)
  {
    fprintf(stderr,
        "Got error setting options path for ExaGO application %s.\n",
        ExaGOCurrentAppName);
    return 1;
  }

  ierr=sprintf(optfiles[3],"./options/%s",filename);
  if(ierr<0)
  {
    fprintf(stderr,
        "Got error setting options path for ExaGO application %s.\n",
        ExaGOCurrentAppName);
    return 1;
  }

  for(i=1; i<*argc; i++)
  {
    // Added null terminator here as this would cause errors with
    if (0==strncmp((*argv)[i],"-v\0",3) || 
        0==strncmp((*argv)[i],"-version\0",4))
    {
      char* versionstr;
      ierr=ExaGOVersionGetFullVersionInfo(&versionstr);CHKERRQ(ierr);
      ExaGOLog(EXAGO_LOG_INFO, "ExaGO Version Info:\n\n%s", versionstr);
      ExaGOLog(EXAGO_LOG_INFO, "ExaGO Help Message:\n%s", help);
      free(versionstr);
      for(i=0; i<noptfiles; i++)
        free(optfiles[i]);
      MPI_Finalize();
      exit(0);
    }
  }

  const int idx = anyFileExist(optfiles, noptfiles);
  if (idx < 0)
  {
    ExaGOLog(EXAGO_LOG_ERROR,
        "Could not find options file for application %s.\n",
        ExaGOCurrentAppName);
    for(i=0; i<noptfiles; i++)
      free(optfiles[i]);
    free(optfiles);
    return 1;
  }
  
  PetscInitialize(argc,argv,optfiles[idx],help);
  
  for(i=0; i<noptfiles; i++)
    free(optfiles[i]);
  free(optfiles);
  return 0;
}


PetscErrorCode ExaGOFinalize()
{
  ExaGOLog(EXAGO_LOG_INFO,"Finalizing %s application.\n",ExaGOCurrentAppName);
  PetscBool flg;
  ExaGOLogIsUsingLogFile(&flg);
  if (flg)
  {
    char* filename;
    ExaGOLogGetLoggingFileName(&filename);
    ExaGOLog(EXAGO_LOG_INFO,"See logfile %s for output.\n",filename);
  }
  ExaGOLogFinalize();
  PetscFinalize();
  MPI_Finalize();
}