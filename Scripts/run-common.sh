
gdb_screen()
{
    prog=$1
    shift
    IFS=
    name=${*/-/}
    IFS=' '
    screen -S :$name -d -m bash -l -c "echo $*; echo $LIBRARY_PATH; gdb $prog -ex \"run $*\""
}

lldb_screen()
{
    prog=$1
    shift
    IFS=
    name=${*/-/}
    IFS=' '
    echo debug $prog with arguments $*
    echo name: $name
    tmp=/tmp/$RANDOM
    echo run > $tmp
    screen -S :$i -d -m bash -l -c "lldb -s $tmp $prog -- $*"
}

run_player() {
    port=$((RANDOM%10000+10000))
    bin=$1
    shift
    if ! test -e $SPDZROOT/logs; then
        mkdir $SPDZROOT/logs
    fi
    if [[ $bin = Player-Online.x || $bin =~ 'party.x' ]]; then
	params="$* -pn $port -h localhost"
	if [[ ! ($bin =~ 'rep' || $bin =~ 'brain') ]]; then
	    params="$params -N $players"
	fi
    else
	params="$port localhost $*"
    fi
    rem=$(($players - 2))
    if test "$1"; then
	log_prefix=$1-
    fi
    for i in $(seq 0 $rem); do
      >&2 echo Running $prefix $SPDZROOT/$bin $i $params
      log=$SPDZROOT/logs/$log_prefix$i
      $prefix $SPDZROOT/$bin $i $params 2>&1 |
	  { if test $i = 0; then tee $log; else cat > $log; fi; } &
    done
    last_player=$(($players - 1))
    i=$last_player
    >&2 echo Running $prefix $SPDZROOT/$bin $last_player $params
    $prefix $SPDZROOT/$bin $last_player $params > $SPDZROOT/logs/$log_prefix$last_player 2>&1 || return 1
    wait
}

sleep 0.5

#mkdir /dev/shm/Player-Data

players=${PLAYERS:-2}

SPDZROOT=${SPDZROOT:-.}

#. Scripts/setup.sh

mkdir logs 2> /dev/null
