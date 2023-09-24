// SADEST JAVASCRIPT EVER.
// You're welcome.
//
// MrPingouin

const _settings = { 
    version : "Version v0.18",
    debugUrl : true,
    cgi : "./chessika.cgi",
    movesToGo: 40, 
    searchTime: 60000
}

var _board = null;
var _history = null;


class Move {
    /***
     * uci protocol uses coords like e2e4
     * chessboard.js uses e2-e4
     * we store a move using its uci style, and its algebraic notation (Nf3, exf6...)
     ***/

    constructor(uci, algebraic, duration, castlingRookMove, pawnPromotion) {
        this.uci = uci;
        this.algebraic = algebraic;
        this.duration = duration;
        this.castlingRookMove = castlingRookMove;
        this.pawnPromotion = pawnPromotion;
    }
}

class History {
    constructor() {
        this.moves = [];
        this.stackIndex = 0;
    }

    getMovesAsUci(limit) {
        var s = "";
        this.moves.forEach(function(item, index) {
            if (limit != undefined && index >= limit)
                return;
            s += item.uci + " ";
        });
        return s;
    }

    pushMove(uci, algebraic, duration, castlingRookMove, pawnPromotion) {
        this.moves.push(new Move(uci, algebraic, duration, castlingRookMove, pawnPromotion));
        this.stackIndex++;
    }

    reset() {
        this.moves = [];
        this.stackIndex = 0;
    }

    loadPly(ply) {
        var plyLimit = ply + 1;
        this.stackIndex = ply;

        _board.start();

        this.moves.forEach(function(item, index) {

            if (index >= plyLimit) {
                return;
            }

            var src = item.uci.substring(0,2);
            var dst = item.uci.substring(2,4);
            _board.move(src + "-" + dst);

            if (item.castlingRookMove != undefined) {
                src = item.castlingRookMove.substring(0,2);
                dst = item.castlingRookMove.substring(2,4);
                _board.move(src + "-" + dst);
            }

            if (item.pawnPromotion != undefined) {
                dst = item.uci.substring(2,4);
                let pos = _board.position();
                if (dst[1] == 8)
                    pos[dst] = "wQ";
                else
                    pos[dst] = "bQ";
                _board.position(pos, false);
            }
        });

        $("#genStatus").html("History ply " + plyLimit);
    }

    updateHTMLlist() {

        $("#history").html("");
        $("#moveList").val("");
        
        let historyInstance = this;

        this.moves.forEach(function(item, ply) {
            if (ply%2 == 0) {
                var li = $('<li>');
                $("#history").append(li);
            }
            else {
                var li = $("ul#history").children().last();
            }

            $("#moveList").val(($("#moveList").val() + " " + item.uci).trim());

            var a = $('<a>', {
                        text: item.algebraic,
                        href: "#",
            }).appendTo(li);
            a.data("ply", ply);
            a.on("click", function() {
                historyInstance.loadPly(ply);
            });
        });
    }

    cutHistoryToStackIndex() {
        //console.log(this.stackIndex + " / " + this.moves.length);
        if ((this.stackIndex+1) < this.moves.length) {
            for (var i=0; i < (this.moves.length - this.stackIndex); i++) {
                this.moves.pop();
            }
            this.updateHTMLlist();
        }
    }
}

$(document).ready(function() {
    var _initialized = false;
    var _btime;
    var _wtime;

    function resetEngine() {
        _wtime = _settings.searchTime;
        _btime = _wtime;
        _history.reset();
        _board.start();

        $("#genStatus").html("ready");
        $("#history").html("");
    }

    var START_POS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    var EMPTY_POS = "8/8/8/8/8/8/8/8";

    /***
     * This is the main event : human players drag'n'dropped a piece.
     * If the board changed, we first check that the move exists, 
     * and then we trigger a search by the cpu.
     ***/
    var onDrop = function(source, target, piece, newPos, oldPos, orientation) {
        let oldPosStr = ChessBoard.objToFen(oldPos);
        let newPosStr = ChessBoard.objToFen(newPos);

        if (newPosStr != START_POS && newPosStr != EMPTY_POS && newPosStr != oldPosStr) {

            // If the player picks a position from history and drops a new move,
            // then we reduce the history length accordingly.
            _history.cutHistoryToStackIndex();
            pushPlayerMove(source, target, oldPosStr, true);
        }
    }

    $("div#version").html(_settings.version);
        
    var cfg = {
        draggable: true,
        //onDragStart: onDragStart,
        position:'start',
        onDrop:onDrop,
        sparePieces: false,
        moveSpeed: 'fast',
        orientation: ($("#orientation").val().toLowerCase())
    };

    _board = ChessBoard('board', cfg);
    _history = new History();

    /***
     * This function is needed to complete rook move during castling
     * Caution : the function does move the rook
     ***/
    function doCastlingRookMove(data) {
        if (data.match(/castling_rook/)) {
            const castlingRookRxp = /castling_rook ([a-h][1-8][a-h][1-8])/;
            let m = castlingRookRxp.exec(data);
            let src = m[1].substring(0,2);
            let dst = m[1].substring(2,4);
            _board.move(src + "-" + dst);
            return (src + dst);
        }
        else
            return undefined;
    }

    /***
     * This function is needed to complete pawn promotion 
     ***/
    function doPawnPromotion(data) {
        if (data.match(/webmove .*?=[QRNBqrnb]/)) {
            const pawnPromotionRxp = /webmove [a-h][x27]([a-h][18])=([QRNBrqnb])/;
            let m = pawnPromotionRxp.exec(data);
            let pos = _board.position();
            let square = m[1];
            if (square[1] == 8)
                pos[square] = "wQ";
            else
                pos[square] = "bQ";
            _board.position(pos, false);
            return m[2];
        }
        else
            return undefined;
    }

    function pushPlayerMove(source, target, oldPosStr, autoPlayCpu) {
        return new Promise((resolve, reject) => {
            let newHistory = (_history.getMovesAsUci() + " " + source + target).trim();

            let movesToGo = _settings.movesToGo - _history.moves.length;
            let url = _settings.cgi + "?h=" + newHistory + "&checkOnly=true&wtime=" + _wtime + "&btime=" + _btime + "&movestogo=" + movesToGo;

            if (_settings.debugUrl) {
                console.log("PLAYER : " + url);
            }

            $.ajax({
                url:url,
            }).done(function(data) {

                if (data.match(/INVALID/)) {
                    _board.position(oldPosStr);
                    $("#genStatus").html("invalid move");
                    return;
                }

                let castlingRookMove = doCastlingRookMove(data);
                let pawnPromotion = doPawnPromotion(data);

                let webMoveRxp = /webmove (.*)/;
                m = webMoveRxp.exec(data);
                let webMove = m[1];
                _history.pushMove(source + target, webMove, 0, castlingRookMove, pawnPromotion);
                _history.updateHTMLlist();

                if (autoPlayCpu == true)
                    triggerSearch(_history, _wtime, _btime);
                
                resolve("okay");
            });
        });
    }

    function triggerSearch(history, wtime, btime) {
        let movesToGo = _settings.movesToGo - history.moves.length;
        let url = _settings.cgi + "?h=" + history.getMovesAsUci() + "&wtime=" + _wtime + "&btime=" + _btime + "&movestogo=" + movesToGo;
        
        if (_settings.debugUrl) {
            console.log("CPU : " + url);
        }

        $("#genStatus").html("go wtime " + _wtime + " btime " + _btime + " movestogo " + movesToGo);

        var refTimeStart = Date.now();

        $.ajax({
            url:url
        }).done(function(data) {
            var durationMs = Date.now() - refTimeStart;

            if (_board.orientation() == "white") {
                _btime -= durationMs;
            }
            else {
                _wtime -= durationMs;
            }

            if (data.match(/bestmove/)) {

                // Retrieving longest PV
                let pvRxp = /info depth ([0-9]+).*/g;
                while (pvm = pvRxp.exec(data)) {
                    $("#genStatus").html(pvm[0]);
                }

                // Retrieving move in uci coords
                let bestMoveRxp = /bestmove (([a-h][1-8])([a-h][1-8])[RNBQrnbq]?)/;
                let bestMove = bestMoveRxp.exec(data);
                _board.move(bestMove[2] + "-" + bestMove[3]);

                // Retrieving move in algebraic notation
                let webMoveRxp = /webmove (.*)/;
                m = webMoveRxp.exec(data);
                let webMove = m[1];

                let castlingRookMove = doCastlingRookMove(data);
                let pawnPromotion = doPawnPromotion(data);
                history.pushMove(bestMove[2] + bestMove[3], webMove, durationMs, castlingRookMove, pawnPromotion);
                history.updateHTMLlist();
                $("span#wtime").html(_wtime);
                $("span#btime").html(_btime);
                $("span#movestogo").html(movesToGo);
            }
            else {
                if (data.match(/lost_on_time/)) {
                    $("#genStatus").html("lost on time");
                }
                if (data.match(/checkmate_P1/)) {
                    $("#genStatus").html("P1 wins");
                }
                if (data.match(/checkmate_P2/)) {
                    $("#genStatus").html("P2 wins");
                }
            }
        });
    }

    function changeOrientation() {
        if(_initialized == true) {
            _board.flip();
            triggerSearch(_history, _wtime, _btime);
        }
    }

    function clearMoveList() {
        $("#moveList").val("");
    }

    async function loadMoveList() {
        resetEngine();
        let tokens = $("#moveList").val().split(" ");
        for (var i=0; i<tokens.length; i++) {
            let move = tokens[i].trim();
            var src = move.substring(0,2);
            var dst = move.substring(2,4);

            let oldPosStr = ChessBoard.objToFen(_board.position());
            await pushPlayerMove(src, dst, oldPosStr, false);
            _board.move(src + "-" + dst);
        }
    }

    $('#clearBtn').on('click', resetEngine);
    $('#loadPosBtn').on('click', loadMoveList);
    $('#clearMoveList').on('click', clearMoveList);

    $('#orientation').on('change', changeOrientation);

    // Game start ?

    if (_board.orientation() == "black") {
        triggerSearch(_history, _wtime, _btime);
    }

    let tc = $("#maxSearchTime").val();
    switch (tc) {
        case "40/60": _settings.searchTime = 60000; break;
        case "40/300": _settings.searchTime = 300000; break;
    }

    resetEngine();
    _initialized = true;
});
