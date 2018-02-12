#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "whereami.decl.h"

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
      CkPrintf("[%3d]: %3d, %s\n", i, cpuArray[i], hostnameArray[i].c_str());
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
    mainProxy.receiveCPU(thisIndex, sched_getcpu(), std::string(hostname));
  }

  Where(CkMigrateMessage *m) {}
};

#include "whereami.def.h"
