# Demonstrates async work of thread pools because messages should finish out of order.
# Obviously server must be started first on the same port. 

for i in {1..250}
do
    ./client --port=12000 --msg=hello-$i
done
