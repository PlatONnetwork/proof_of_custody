[TOC]

## Dependencies
Besides the required lib for scale mamba, you also need to install the [mcl](https://github.com/herumi/mcl) library.

## Compile

```bash
./compile.sh
```

## Init

```bash
./init.sh <n> <t>
```

- The default LSSS is `Shamir`.
- The default modulus is `4002409555221667393417789825735904156556882819939007885332058136124031650490837864442687629129015664037894272559787`.

## Run the demo

You should `init` first, and then

```bash
./run.sh <n>
```

## Progresses (Stage I)
1. Seperate Scale-Mamba (100%)
2. DKG of BLS (100%)
3. Distributed sign of BLS (100%)
4. Universal hash function (UHF) (100%)
5. Legendre PRF (100%)

## Performance
1. the total time for (1-3) case is less than 0.5s.