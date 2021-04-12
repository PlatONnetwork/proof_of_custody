
## Overview

Step 1

```sh
bash ./cbft_install_deps.sh
```

Step 2

```sh
bash ./cbft_compile.sh
```

Step 3, Test

```sh
bash ./cbft_run.sh
```

n - r < f
n = 3f + 2c + 1


n is total number of replicas in the system (both running and not)
r is number of actually running replicas (r <= n and r >= 2f+c+1 )
f is max. number of faulty replicas
c is max. number of slow replicas
cl is number of clients to run

replicas n  = 4 6 20
clients  cl = 1 2  4

client id, start from the max index of the replicas

