OPENINGS=books/gaviota-starters.pgn
OLD=bin/chessika_old
NEW=obj/chessika
TC="10+0.1s"
CONCURRENCY=3
cutechess-cli -engine cmd=${NEW} name="NEW" -engine cmd=${OLD} name="OLD" -each tc="${TC}" proto=uci -openings file=${OPENINGS} order=random -repeat -recover -concurrency ${CONCURRENCY} -rounds $1#-debug
