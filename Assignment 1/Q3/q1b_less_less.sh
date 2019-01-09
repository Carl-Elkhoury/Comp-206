CMD=less

for LINE in "`ps l | grep $CMD |  grep -v grep`"
do
  if [ "${#LINE}" -gt 0 ]
  then
       PID=`echo $LINE | awk '{print $3}'`
       kill $PID
   fi
done

