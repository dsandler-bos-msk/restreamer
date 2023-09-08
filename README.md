# restreamer
A simple UDP repeater.

## Building

```
mkdir cmake-build-debug 
cd cmake-build-debug 
cmake ../
make
```

# Example

The first argument indicates the port to listen to.
After the first argument, arguments are specified pairwise as the IPs and and their respective port to forward to.
```
./restreamer 5000 127.0.0.1 5005 8.8.8.8 5008
```
In this example: UDP packets coming to port 5000 will be forwarded to: port 5005 on localhost, as well as to port 5008 on 8.8.8.8.

# License

See the LICENSE file.
