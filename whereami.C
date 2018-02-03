#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "hello.decl.h"

/*readonly*/ CProxy_Main mainProxy;
/*readonly*/ CProxy_Hello arrProxy;
/*readonly*/ int nElements;


int get_cpu_id()
{
    /* Get the the current process' stat file from the proc filesystem */
    FILE* procfile = fopen("/proc/self/stat", "r");
    long to_read = 8192;
    char buffer[to_read];
    int read = fread(buffer, sizeof(char), to_read, procfile);
    fclose(procfile);

    // Field with index 38 (zero-based counting) is the one we want
    char* line = strtok(buffer, " ");
    for (int i = 1; i < 38; i++)
    {
        line = strtok(NULL, " ");
    }

    line = strtok(NULL, " ");
    int cpu_id = atoi(line);
    return cpu_id;
}


/*mainchare*/
class Main : public CBase_Main
{
public:
  Main(CkArgMsg* m)
  {
    int numPes = CkNumPes();

    CkArrayOptions opts;
    opts.setNumInitial(numPes);
    CkCallback initCB(CkIndex_Main::done(), thisProxy);
    opts.setInitCallback(initCB);
    opts.setStaticInsertion(true);
    arrProxy = CProxy_Hello::ckNew(opts);
  };

  void done(void) {
    CkExit();
  }
};

/*array [1D]*/
class Hello : public CBase_Hello
{
public:
  Hello()
  {
    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    CkPrintf("[%d] on PE %d, host %s, cpu %d\n",thisIndex, CkMyPe(), hostname, get_cpu_id());
  }

  Hello(CkMigrateMessage *m) {}
  };

#include "hello.def.h"
