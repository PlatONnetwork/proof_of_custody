[TOC]

Under Development ...

## Quick start

- First, clone the code from Github. (`with --recurse`)

```bash
git clone --recurse https://github.com/PlatONnetwork/proof_of_custody.git
```

- Next, install some dependencies by executing the following script.

```bash
sudo bash ./pre_install.sh
```

- Next, compile `mcl`/`mpir`/`SimpleOT`.

```bash
./pre_compile.sh
```

- Next, compile this project.

```bash
./compile.sh
```

- Now, run the test.

```bash
./test.sh [<parties> [<host>]]
```
> parties: the number of parties. default `2`. 
> host: the host or ip of the party 0. default `localhost`. 



[OLD,TODO]

## Introduction
This project implements the proof of custody algorithm in Ethereum 2.0 with secure multi-party computation.

[Custody game](https://github.com/ethereum/eth2.0-specs/blob/dev/specs/phase1/custody-game.md) is a mechnism will be adpoted in Ethereum 2.0 (Phase 1) to solve the data availability problem. It is designed to be mpc-friendly, since the mpc implementation will avoid single point of failure, reduce the staking cost and enable the possibility of trustless validator pools, et al.

This project aims to estimate the feasiblity and performance of the mpc implementation of proof of custody.
The implementation is built on and highly customized from [SCALE-MAMBA](https://github.com/KULeuven-COSIC/SCALE-MAMBA) with optimizations specific to proof of custody.

**The project is officially granted by Ethereum Foundation.**

## Dependencies
- openssl
- gmp
- mpir
- mcl
- yasm

Run the following script to install all the dependencies.

```bash
sudo ./install_deps.sh
```

## Compile

Run the following command to compile.

```bash
./compile.sh
```

## Init

Run the following command to initiate the system.
```bash
./init.sh n t
```
- n : the number of parties.
- t : the number of threshold, and 0 < t < n/2
- Shamir type of LSSS is used.

- The default modulus (under BLS12-381 curve) is 
```bash
4002409555221667393417789825735904156556882819939007885332058136124031650490837864442687629129015664037894272559787
```

## Run the demo

You could run the demo in localhost, with the following command. 
Note: `init`  should be run first.

```bash
./run.sh n
```
- run once of the protocol.
- n : the number of parties, should be the same as the one in `init`.

```bash
./runbatch.sh n
```
- run 50 times (default) of the protocol.
- n : the number of parties, should be the same as the one in `init`.
 
## Progresses

### Stage I

1. Remove the compiler part of  SCALE-MAMBA (100%)
2. DKG of BLS (100%)
3. Distributed sign of BLS (100%)
4. Universal hash function (UHF) (100%)
5. Legendre PRF (100%)

## Performance
1. the total time for 3-1 case is less than 0.5s in LAN.

## Acknowledgements
- Special thanks to Dankrad Feist for introducing proof of custody and deep discussions of the technique details. 

- Thanks to Nigel Smart for discussions of PRSS implementation in SCALE-MAMBA.

- Thanks to Yu Yu for discussions of universal hash function and the security of Legendre PRF.

## Contact
xiang.xie@platon.network