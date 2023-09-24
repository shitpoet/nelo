;; Generates Engine.
(define my-engine (gen-engine))

;; Configures Engine.
(my-engine (quote @material) (quote (0 100 400 400 600 1200 1000000)))
(my-engine (quote @enable-quiesce-search) (quote #t))
(my-engine (quote @enable-repetition-check) (quote #t))
(my-engine (quote @enable-check-extension) (quote #t))
(my-engine (quote @ybwc-limit-depth) (quote 4))
(my-engine (quote @ybwc-invalid-moves) (quote 3))
(my-engine (quote @enable-aspiration-windows) (quote #t))
(my-engine (quote @aspiration-windows-limit-depth) (quote 5))
(my-engine (quote @aspiration-windows-delta) (quote 15))
(my-engine (quote @enable-see) (quote #t))
(my-engine (quote @enable-history) (quote #t))
(my-engine (quote @enable-killer) (quote #t))
(my-engine (quote @enable-hash-table) (quote #t))
(my-engine (quote @enable-iid) (quote #t))
(my-engine (quote @iid-limit-depth) (quote 5))
(my-engine (quote @iid-search-depth) (quote 4))
(my-engine (quote @enable-nmr) (quote #t))
(my-engine (quote @nmr-limit-depth) (quote 4))
(my-engine (quote @nmr-search-reduction) (quote 4))
(my-engine (quote @nmr-reduction) (quote 3))
(my-engine (quote @enable-probcut) (quote #f))
(my-engine (quote @probcut-limit-depth) (quote 4))
(my-engine (quote @probcut-margin) (quote 400))
(my-engine (quote @probcut-search-reduction) (quote 3))
(my-engine (quote @enable-history-pruning) (quote #f))
(my-engine (quote @history-pruning-limit-depth) (quote 4))
(my-engine (quote @history-pruning-move-threshold) (quote 0.6))
(my-engine (quote @history-pruning-invalid-moves) (quote 10))
(my-engine (quote @history-pruning-threshold) (quote 0.5))
(my-engine (quote @history-pruning-reduction) (quote 1))
(my-engine (quote @enable-lmr) (quote #t))
(my-engine (quote @lmr-limit-depth) (quote 4))
(my-engine (quote @lmr-move-threshold) (quote 0.3))
(my-engine (quote @lmr-invalid-moves) (quote 4))
(my-engine (quote @lmr-search-reduction) (quote 1))
(my-engine (quote @enable-futility-pruning) (quote #t))
(my-engine (quote @futility-pruning-depth) (quote 3))
(my-engine (quote @futility-pruning-margin) (quote 400))
(my-engine (quote @pawn-square-table-opening) (quote (0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 4 9.5 16 19 19 16 9.5 4 8 19 32 38 38 32 19 8 12 28.5 48 57 57 48 28.5 12 16 38 64 76 76 64 38 16 20 47.5 80 95 95 80 47.5 20 24 57 96 114 114 96 57 24)))
(my-engine (quote @knight-square-table-opening) (quote (-13.5 -11.2 -9 -6.7 -6.7 -9 -11.2 -13.5 -8.9 -6.5 -4 -1.6 -1.6 -4 -6.5 -8.9 -4.3 -1.7 1 3.6 3.6 1 -1.7 -4.3 0.3 3.1 5.9 8.7 8.7 5.9 3.1 0.3 4.9 7.9 10.9 13.9 13.9 10.9 7.9 4.9 9.5 12.7 15.8 19 19 15.8 12.7 9.5 4.9 7.9 10.9 13.9 13.9 10.9 7.9 4.9 0.3 3.1 5.9 8.7 8.7 5.9 3.1 0.3)))
(my-engine (quote @bishop-square-table-opening) (quote (8 5.3 2.7 0 0 2.7 5.3 8 6.9 9.6 6.9 4.3 4.3 6.9 9.6 6.9 5.9 8.5 11.2 8.5 8.5 11.2 8.5 5.9 4.8 7.5 10.1 12.8 12.8 10.1 7.5 4.8 6.4 9.1 11.7 14.4 14.4 11.7 9.1 6.4 10.7 13.3 16 13.3 13.3 16 13.3 10.7 11.7 14.4 11.7 9.1 9.1 11.7 14.4 11.7 12.8 10.1 7.5 4.8 4.8 7.5 10.1 12.8)))
(my-engine (quote @rook-square-table-opening) (quote (1 1.9 3.6 6.7 6.7 3.6 1.9 1 1 1.9 3.6 6.7 6.7 3.6 1.9 1 1 1.9 3.6 6.7 6.7 3.6 1.9 1 1 1.9 3.6 6.7 6.7 3.6 1.9 1 1 1.9 3.6 6.7 6.7 3.6 1.9 1 1 1.9 3.6 6.7 6.7 3.6 1.9 1 22.6 22.6 22.6 22.6 22.6 22.6 22.6 22.6 22.6 22.6 22.6 22.6 22.6 22.6 22.6 22.6)))
(my-engine (quote @queen-square-table-opening) (quote (0 0 0 0 0 0 0 0 0 4 4 4 4 4 4 0 0 4 8 8 8 8 4 0 0 4 8 8 8 8 4 0 0 4 8 8 8 8 4 0 0 4 8 8 8 8 4 0 0 4 4 4 4 4 4 0 0 0 0 0 0 0 0 0)))
(my-engine (quote @king-square-table-opening) (quote (32 32 16 -22.6 -22.6 16 32 32 8 8 4 -22.6 -22.6 4 8 8 -32 -32 -32 -32 -32 -32 -32 -32 -38.1 -38.1 -38.1 -38.1 -38.1 -38.1 -38.1 -38.1 -45.3 -45.3 -45.3 -45.3 -45.3 -45.3 -45.3 -45.3 -53.8 -53.8 -53.8 -53.8 -53.8 -53.8 -53.8 -53.8 -64 -64 -64 -64 -64 -64 -64 -64 -76.1 -76.1 -76.1 -76.1 -76.1 -76.1 -76.1 -76.1)))
(my-engine (quote @pawn-square-table-ending) (quote (0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 32 32 32 32 32 32 32 32 64 64 64 64 64 64 64 64 96 96 96 96 96 96 96 96 128 128 128 128 128 128 128 128 160 160 160 160 160 160 160 160 192 192 192 192 192 192 192 192)))
(my-engine (quote @knight-square-table-ending) (quote (0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 8 8 8 8 0 0 0 0 8 8 8 8 0 0 0 0 8 8 8 8 0 0 0 0 8 8 8 8 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0)))
(my-engine (quote @bishop-square-table-ending) (quote (0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0)))
(my-engine (quote @rook-square-table-ending) (quote (0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0)))
(my-engine (quote @queen-square-table-ending) (quote (0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0)))
(my-engine (quote @king-square-table-ending) (quote (-16 -10.7 -5.3 0 0 -5.3 -10.7 -16 -10.7 -5.3 0 5.3 5.3 0 -5.3 -10.7 -5.3 0 5.3 10.7 10.7 5.3 0 -5.3 0 5.3 10.7 16 16 10.7 5.3 0 0 5.3 10.7 16 16 10.7 5.3 0 -5.3 0 5.3 10.7 10.7 5.3 0 -5.3 -10.7 -5.3 0 5.3 5.3 0 -5.3 -10.7 -16 -10.7 -5.3 0 0 -5.3 -10.7 -16)))
(my-engine (quote @pawn-attack-table) (quote (0 0 8 8 8 8 8)))
(my-engine (quote @knight-attack-table) (quote (0 2 0 2.8 8 8 8)))
(my-engine (quote @bishop-attack-table) (quote (0 4 5.7 0 16 16 16)))
(my-engine (quote @rook-attack-table) (quote (0 4 8 8 0 16 16)))
(my-engine (quote @queen-attack-table) (quote (0 4 8 8 8 0 16)))
(my-engine (quote @king-attack-table) (quote (0 0 0 0 0 0 0)))
(my-engine (quote @pawn-defense-table) (quote (0 5.7 4 0 0 0 0)))
(my-engine (quote @knight-defense-table) (quote (0 6.7 3.4 0 0 0 0)))
(my-engine (quote @bishop-defense-table) (quote (0 4.8 0 0 0 0 0)))
(my-engine (quote @rook-defense-table) (quote (0 4 0 0 0 1 0)))
(my-engine (quote @queen-defense-table) (quote (0 2.8 1 1 1 0 0)))
(my-engine (quote @king-defense-table) (quote (0 19 0 0 0 0 0)))
(my-engine (quote @pawn-shield-table) (quote (0 0 0 0 0 0 0 0 32 32 32 32 32 32 38.1 32 0 0 0 0 0 0 0 0 -22.6 -22.6 -22.6 -22.6 -22.6 -22.6 -22.6 -22.6 -45.2 -45.2 -45.2 -45.2 -45.2 -45.2 -45.2 -45.2 -67.8 -67.8 -67.8 -67.8 -67.8 -67.8 -67.8 -67.8 -90.4 -90.4 -90.4 -90.4 -90.4 -90.4 -90.4 -90.4 -113 -113 -113 -113 -113 -113 -113 -113)))
(my-engine (quote @weight-pawn-opening-position) (quote ((2 0) (14 0) (24 1) (32 1))))
(my-engine (quote @weight-knight-opening-position) (quote ((2 0) (14 0) (24 1) (32 1))))
(my-engine (quote @weight-bishop-opening-position) (quote ((2 0) (14 0) (24 1) (32 1))))
(my-engine (quote @weight-rook-opening-position) (quote ((2 0) (14 0) (24 1) (32 1))))
(my-engine (quote @weight-queen-opening-position) (quote ((2 0) (14 0) (24 1) (32 1))))
(my-engine (quote @weight-king-opening-position) (quote ((2 0) (14 0) (24 1) (32 1))))
(my-engine (quote @weight-pawn-ending-position) (quote ((2 1) (14 1) (24 0) (32 0))))
(my-engine (quote @weight-knight-ending-position) (quote ((2 1) (14 1) (24 0) (32 0))))
(my-engine (quote @weight-bishop-ending-position) (quote ((2 1) (14 1) (24 0) (32 0))))
(my-engine (quote @weight-rook-ending-position) (quote ((2 1) (14 1) (24 0) (32 0))))
(my-engine (quote @weight-queen-ending-position) (quote ((2 1) (14 1) (24 0) (32 0))))
(my-engine (quote @weight-king-ending-position) (quote ((2 1) (14 1) (24 0) (32 0))))
(my-engine (quote @weight-pawn-mobility) (quote ((2 2.4) (14 2.4) (24 1.2) (32 1.2))))
(my-engine (quote @weight-knight-mobility) (quote ((2 1.7) (14 1.7) (24 0.8) (32 0.8))))
(my-engine (quote @weight-bishop-mobility) (quote ((2 13.5) (14 13.5) (24 9.5) (32 9.5))))
(my-engine (quote @weight-rook-mobility) (quote ((2 8) (14 8) (24 4.8) (32 4.8))))
(my-engine (quote @weight-queen-mobility) (quote ((2 6.7) (14 6.7) (24 2.8) (32 2.8))))
(my-engine (quote @weight-king-mobility) (quote ((2 1.4) (14 1.4) (24 0) (32 0))))
(my-engine (quote @weight-pawn-center-control) (quote ((2 1) (14 1) (24 4.8) (32 4.8))))
(my-engine (quote @weight-knight-center-control) (quote ((2 1.2) (14 1.2) (24 8) (32 8))))
(my-engine (quote @weight-bishop-center-control) (quote ((2 1) (14 1) (24 4.8) (32 4.8))))
(my-engine (quote @weight-rook-center-control) (quote ((2 1.7) (14 1.7) (24 2) (32 2))))
(my-engine (quote @weight-queen-center-control) (quote ((2 1.2) (14 1.2) (24 1.4) (32 1.4))))
(my-engine (quote @weight-king-center-control) (quote ((2 6.7) (14 6.7) (24 0) (32 0))))
(my-engine (quote @weight-pawn-sweet-center-control) (quote ((2 0) (14 0) (24 4) (32 4))))
(my-engine (quote @weight-knight-sweet-center-control) (quote ((2 0) (14 0) (24 3.4) (32 3.4))))
(my-engine (quote @weight-bishop-sweet-center-control) (quote ((2 0) (14 0) (24 1.4) (32 1.4))))
(my-engine (quote @weight-rook-sweet-center-control) (quote ((2 1) (14 1) (24 0.6) (32 0.6))))
(my-engine (quote @weight-queen-sweet-center-control) (quote ((2 0) (14 0) (24 1) (32 1))))
(my-engine (quote @weight-king-sweet-center-control) (quote ((2 2.8) (14 2.8) (24 0) (32 0))))
(my-engine (quote @weight-pawn-development) (quote ((2 0) (14 0) (24 1.2) (32 1.2))))
(my-engine (quote @weight-knight-development) (quote ((2 0) (14 0) (24 38.1) (32 38.1))))
(my-engine (quote @weight-bishop-development) (quote ((2 0) (14 0) (24 32) (32 32))))
(my-engine (quote @weight-rook-development) (quote ((2 0) (14 0) (24 0) (32 0))))
(my-engine (quote @weight-queen-development) (quote ((2 0) (14 0) (24 0) (32 0))))
(my-engine (quote @weight-king-development) (quote ((2 0) (14 0) (24 0) (32 0))))
(my-engine (quote @weight-pawn-attack) (quote ((2 1) (14 1) (24 1) (32 1))))
(my-engine (quote @weight-knight-attack) (quote ((2 1) (14 1) (24 1) (32 1))))
(my-engine (quote @weight-bishop-attack) (quote ((2 1) (14 1) (24 0.8) (32 0.8))))
(my-engine (quote @weight-rook-attack) (quote ((2 1) (14 1) (24 1) (32 1))))
(my-engine (quote @weight-queen-attack) (quote ((2 1) (14 1) (24 1) (32 1))))
(my-engine (quote @weight-king-attack) (quote ((2 1) (14 1) (24 1) (32 1))))
(my-engine (quote @weight-pawn-defense) (quote ((2 1) (14 1) (24 1) (32 1))))
(my-engine (quote @weight-knight-defense) (quote ((2 1) (14 1) (24 1) (32 1))))
(my-engine (quote @weight-bishop-defense) (quote ((2 1) (14 1) (24 1) (32 1))))
(my-engine (quote @weight-rook-defense) (quote ((2 1) (14 1) (24 1) (32 1))))
(my-engine (quote @weight-queen-defense) (quote ((2 1) (14 1) (24 1) (32 1))))
(my-engine (quote @weight-king-defense) (quote ((2 1) (14 1) (24 0.5) (32 0.5))))
(my-engine (quote @weight-pawn-attack-around-king) (quote ((2 0) (14 0) (24 0) (32 0))))
(my-engine (quote @weight-knight-attack-around-king) (quote ((2 0) (14 0) (24 0) (32 0))))
(my-engine (quote @weight-bishop-attack-around-king) (quote ((2 4) (14 4) (24 8) (32 8))))
(my-engine (quote @weight-rook-attack-around-king) (quote ((2 4.8) (14 4.8) (24 8) (32 8))))
(my-engine (quote @weight-queen-attack-around-king) (quote ((2 4) (14 4) (24 8) (32 8))))
(my-engine (quote @weight-king-attack-around-king) (quote ((2 4) (14 4) (24 0) (32 0))))
(my-engine (quote @weight-pass-pawn) (quote ((2 45.3) (14 45.3) (24 22.6) (32 22.6))))
(my-engine (quote @weight-protected-pass-pawn) (quote ((2 16) (14 16) (24 11.3) (32 11.3))))
(my-engine (quote @weight-double-pawn) (quote ((2 -13.5) (14 -13.5) (24 -6.7) (32 -6.7))))
(my-engine (quote @weight-iso-pawn) (quote ((2 -8) (14 -8) (24 -13.5) (32 -13.5))))
(my-engine (quote @weight-pawn-shield) (quote ((2 0) (14 0) (24 1) (32 1))))
(my-engine (quote @weight-bishop-pair) (quote ((2 32) (14 32) (24 16) (32 16))))
(my-engine (quote @weight-bad-bishop) (quote ((2 0) (14 0) (24 -4) (32 -4))))
(my-engine (quote @weight-rook-pair) (quote ((2 16) (14 16) (24 8) (32 8))))
(my-engine (quote @weight-rook-semiopen-fyle) (quote ((2 8) (14 8) (24 16) (32 16))))
(my-engine (quote @weight-rook-open-fyle) (quote ((2 0) (14 0) (24 8) (32 8))))
(my-engine (quote @weight-early-queen-starting) (quote ((2 0) (14 0) (24 -26.9) (32 -26.9))))
(my-engine (quote @weight-weak-square) (quote ((2 0) (14 0) (24 -1) (32 -1))))
(my-engine (quote @weight-castling) (quote ((2 0) (14 0) (24 16) (32 16))))
(my-engine (quote @weight-abandoned-castling) (quote ((2 0) (14 0) (24 -90.5) (32 -90.5))))

;; Runs Engine.
(my-engine (quote @run))