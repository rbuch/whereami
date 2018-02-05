#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "whereami.decl.h"

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

  int numPes, msgsRcvd;
  int* cpuArray;
  std::string* hostnameArray;
  
  Main(CkArgMsg* m)
  {
    numPes = CkNumPes();
    msgsRcvd = 0;

    cpuArray = new int[numPes];
    hostnameArray = new std::string[numPes];
    
    CkArrayOptions opts;
    opts.setNumInitial(numPes);
    opts.setStaticInsertion(true);
    CProxy_Where::ckNew(thisProxy, opts);
  }

  void receiveCPU(int sender, int cpuId, std::string hostname) {
    cpuArray[sender] = cpuId;
    hostnameArray[sender] = hostname;
    if (++msgsRcvd == numPes) {
      thisProxy.done();
    }
  }

  void done(void) {
    for (int i = 0; i < numPes; ++i) {
      CkPrintf("[%d]: %d, %s\n", i, cpuArray[i], hostnameArray[i].c_str());
    }
    CkExit();
  }
};

/*array [1D]*/
class Where : public CBase_Where
{
public:
  Where(CProxy_Main mainProxy)
  {
    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    mainProxy.receiveCPU(thisIndex, get_cpu_id(), std::string(hostname));
  }

  Where(CkMigrateMessage *m) {}
};

#include "whereami.def.h"
