
while true; 
do
NOW_DATE_TIME=`date '+%Y-%m-%d %H:%M:%S'`
NOW_DATE=`date '+%Y-%m-%d'`
ONMW=$(/usr/bin/ps -aux | grep onmw | grep -v grep | grep -v sh)
echo "$NOW_DATE_TIME  $ONMW" >> "/data/onpia/onmw/log/ps_info_${NOW_DATE}.log"
sleep 1; 
done

