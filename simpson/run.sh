rm log.txt;

for cpus in {1..4};
do
    echo -n -e "$cpus " >> log.txt;
    SUM=0;
    RUNS=$1;
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
    echo "Average time: $AVG_TIME";
    echo "$AVG_TIME" >> log.txt;
done

