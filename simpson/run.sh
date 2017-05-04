if [[ $# -ne 3 ]];
then
    echo "Usage: $0 [MIN threads] [MAX threads] [NUM of runs]";
    exit 1;
fi

LOG_FILE="log.txt"
rm "$LOG_FILE" > /dev/null 2>&1;

for ((cpus=$1; cpus<=$2; cpus++));
do
    echo -n -e "$cpus " >> "$LOG_FILE";
    SUM=0; RUNS=$3;
    echo "Running $RUNS times @ $cpus threads";

    for ((n=0; n<$RUNS; n++));
    do
        echo -n "run $n: ";
        TIME="$( { /usr/bin/time -f "%e" ./integral -1 1 $cpus > /dev/null; } 2>&1 )";
        SUM=$(python -c "print($TIME + $SUM)");
        echo $TIME;
        sleep 1;
    done

    AVG_TIME=$(python -c "print(round(float($SUM)/$RUNS, 2))");
    EXE_TIME=$(python -c "print(round(float($SUM)/$RUNS*$cpus, 2))");

    echo -e "Avg time: $AVG_TIME";
    echo -e "Exe time: \033[33m${EXE_TIME}\033[39m\n";
    echo $AVG_TIME >> "$LOG_FILE";
done

