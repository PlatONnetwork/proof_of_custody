import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

np.set_printoptions(suppress=True)
pd.set_option('display.width', 500)

csv = pd.read_csv('0.csv')
elapsed = dict()
elapsed['mult_phase'] = []
elapsed['square_phase'] = []
elapsed['bit_phase'] = []
elapsed['inputs_phase'] = []
elapsed['setup'] = []
elapsed['ephem_key'] = []
elapsed['gen_proof_pre'] = []
elapsed['gen_proof'] = []


def getx(nnn):
    mult_phase = nnn[nnn['phase name'] == 'mult_phase']['elapsed'].mean()
    square_phase = nnn[nnn['phase name'] == 'square_phase']['elapsed'].mean()
    bit_phase = nnn[nnn['phase name'] == 'bit_phase']['elapsed'].mean()
    inputs_phase = nnn[nnn['phase name'] == 'inputs_phase']['elapsed'].mean()
    setup = nnn[nnn['phase name'] == 'setup']['elapsed'].mean()
    ephem_key = nnn[nnn['phase name'] == 'ephem_key']['elapsed'].mean()
    gen_proof_pre = nnn[nnn['phase name'] == 'gen_proof_pre']['elapsed'].mean()
    gen_proof = nnn[nnn['phase name'] == 'gen_proof']['elapsed'].mean()
    elapsed['mult_phase'].append(mult_phase)
    elapsed['square_phase'].append(square_phase)
    elapsed['bit_phase'].append(bit_phase)
    elapsed['inputs_phase'].append(inputs_phase)
    elapsed['setup'].append(setup)
    elapsed['ephem_key'].append(ephem_key)
    elapsed['gen_proof_pre'].append(gen_proof_pre)
    elapsed['gen_proof'].append(gen_proof)


x = [i for i in range(3, 21)]
for i in x:
    nnn = csv[csv['n'] == i]
    if len(nnn) == 0:
        x.remove(i)
for i in x:
    nnn = csv[csv['n'] == i]
    getx(nnn)

print(x)
for k in elapsed.keys():
    print(k, elapsed[k])

fig, ax = plt.subplots()
plt.xlabel('No. of Parties, t = floor((n-1)/2)')
plt.ylabel('Time in LAN (s)')
"""set interval for y label"""
yticks = range(0, 40, 5)
ax.set_yticks(yticks)
"""set min and max value for axes"""
ax.set_ylim([0, 40])
ax.set_xlim([2, 20])
plt.plot(x, elapsed['mult_phase'], "--", label="MultPhase")
plt.plot(x, elapsed['square_phase'], "--", label="SquarePhase")
plt.plot(x, elapsed['bit_phase'], "*-", label="BitPhase")
plt.plot(x, elapsed['inputs_phase'], "*-", label="InputPhase")
plt.plot(x, elapsed['ephem_key'], "--", label="EphemKey")
plt.plot(x, elapsed['gen_proof_pre'], "+-", label="GenProofPre")
plt.plot(x, elapsed['gen_proof'], "--", label="GenProof")

"""open the grid"""
plt.grid(True)
# plt.legend(bbox_to_anchor=(1.0, 1), loc=l, borderaxespad=0.)
plt.legend()
plt.show()
