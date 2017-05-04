rm log.txt;

for ((cpus=$1;cpus<=$2;cpus++));
do
    echo -n -e "$cpus " >> log.txt;
    SUM=0;
    RUNS=$3;
    echo "Running for $cpus threads ($RUNS runs)";

    for ((n=0;n<$RUNS;n++));
    do
        echo -n "run $n: ";
        TIME="$( { /usr/bin/time -f "%e" ./integral -1 1 $cpus > /dev/null; } 2>&1 )";
        SUM=$(python -c "print($TIME+$SUM)");
        echo $TIME;
        sleep 1;
    done

    AVG_TIME=$(python -c "print(float($SUM)/$RUNS)");
    FULL_TIME=$(python -c "print(float($AVG_TIME)*$cpus)");
    echo -e "Average time: $AVG_TIME\n";
    echo -e "Time*N:\033[33m $FULL_TIME\033[39m\n";
    echo "$AVG_TIME" >> log.txt;
done

