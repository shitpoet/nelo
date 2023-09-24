# Changelog

Here's the Elo evolution from the "blank engine" state

- 0.27 : Knight outpost eval, 1322 - 1255 - 423    7.76 +/- 11.51

- 0.26 : Added insufficient material eval, reverting old time management

- 0.25 : Passed Pawn eval : 98 - 54 - 4, Elo diff: 100.72 +/- 56.56 (TC 1+0.01)

- 0.24 : Late move reduction : 186 - 145 - 14, Elo diff: 41.49 +/- 36.24

- 0.23 : Null Window pv search : 159 112 12, Elo diff: 58.24 ± 40.28 

- 0.22 : Fix no-pvline bug when searching with a narrow alpha beta window. Best move repetition tracking : 128 77 7, Elo diff: 85.25 ± 47.61

- 0.21 : Check extension condition fix : 125 - 72 - 3, Elo Diff : 94.32 +/- 49.87

- 0.20 : Mobility eval : 60 - 39 - 1, Elo Diff : 74.06 +/- 70.25

- 0.19 : Half open file rook bonus : 46 - 2 - 0, Elo Diff: 544.69 +/- nan

- 0.18 : Handling TC increments. Aspiration window : 376 - 286 - 2, Elo Diff: 47.38 +/- 26.65

- 0.17 : Handling fifty moves draw rule : 138 - 110 - 3, Elo Diff: 38.92 +- 43.16

- 0.16 : Added history heuristic, move order bonus on pawn promotions : 307 - 235 - 10, Elo diff: 45.58 ± 28.99

- 0.15 : Fixed pv first sorting

- 0.14 : Code cleaning

- 0.13 : Added qsearch : 170 - 111 - 4 [0.604] 285, Elo difference: 72.98 +/- 41.08

- 0.12 : Resetting eval.

- 0.11 : Killer heuristic fix : 208 - 148 - 13 [0.581] 369, Elo difference: 57 +/- 35.35

- 0.10 : Move Ordering fix : rewritten MVV/LVA, and used instead of SEE : 122 - 75 - 16  [0.610] 213

- 0.9 : Check extension : 19.65 +/- 15.10 (2000 rounds)

- 0.8 : No null-move pruning on PV nodes : 22.79 +/- 15.11 (2000 rounds)

- 0.7 : Fixed time management : 335.56 +/- 31.16 (1035 rounds)

- 0.6 : Null-Move pruning : 48.43 +/- 15.12 (2000 rounds)

- 0.5 : Updating 3-fold repetition part (previous version never reached draw games) : 24.36 +/- 21.45 (1000 rounds)

- 0.4 : Basic time management : 151.35 +/- 53.34 (200 rounds)

- 0.3 : Code cleaning

- 0.2 : Killer heuristic : 79.53 +/- 34.28 (400 rounds)

- 0.1 : SEE on capture : 148.87 +/- 76.03 (60 rounds)

- 0.0 : Blank engine : Plain alpha/beta, basic eval : material + PQST + mobility
