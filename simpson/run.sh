rm log.txt;

for cpus in {1..16};
do
    echo -n -e "$cpus " >> log.txt;
    SUM=0;
    RUNS=5;
    echo "Running for $cpus threads ($RUNS runs)";

    for ((n=0;n<$RUNS;n++));
    do
        TIME="$( { /usr/bin/time -f "%e" ./integral -1 1 $cpus > /dev/null; } 2>&1 )";
        SUM=$(python -c "print($TIME+$SUM)");
        echo $SUM;
        echo $TIME;
    done

    AVG_TIME=$(python -c "print(float($SUM)/$RUNS)");
    echo "$AVG_TIME" >> log.txt;
done

