import pandas as pd
import numpy as np
import time

pd.set_option('display.width', 500)


def fgen(n, t):
    key = str(n) + "." + str(t)
    csv = pd.read_csv('hosts.csv')
    grp = csv[csv[key].isnull() == False]
    # print(grp)
    # print(grp['IP'])
    if n != len(grp['IP']):
        return
    ips = grp['IP'].values.tolist()

    fname = 'vm/xdata/NetworkData-' + str(n) + "-" + str(t) + '.txt'
    with open(fname, 'w') as f:
        f.write("RootCA.crt\n")
        f.write(str(n) + "\n")
        for i in range(n):
            s = str(i) + " " + ips[i] + " Player" + \
                str(i + 1) + ".crt P" + str(i + 1) + "\n"
            f.write(s)
        f.write(str(0) + "\n")
        f.write(str(0) + "\n")
        f.write(str(t) + "\n")


for n in range(3, 21, 2):
    if n == 19:
        n = 20
    t = (n - 1) // 2
    fgen(n, t)

"""
RootCA.crt
20
0 127.0.0.1 Player1.crt P1
1 127.0.0.1 Player2.crt P2
2 127.0.0.1 Player3.crt P3
3 127.0.0.1 Player4.crt P4
4 127.0.0.1 Player5.crt P5
5 127.0.0.1 Player6.crt P6
6 127.0.0.1 Player7.crt P7
7 127.0.0.1 Player8.crt P8
8 127.0.0.1 Player9.crt P9
9 127.0.0.1 Player10.crt P10
10 127.0.0.1 Player11.crt P11
11 127.0.0.1 Player12.crt P12
12 127.0.0.1 Player13.crt P13
13 127.0.0.1 Player14.crt P14
14 127.0.0.1 Player15.crt P15
15 127.0.0.1 Player16.crt P16
16 127.0.0.1 Player17.crt P17
17 127.0.0.1 Player18.crt P18
18 127.0.0.1 Player19.crt P19
19 127.0.0.1 Player20.crt P20
0
0
9
"""
