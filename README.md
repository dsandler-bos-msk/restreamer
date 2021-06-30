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

To restream UDP packets coming to port 5000 to port 5005 on localhost and 5008 on 8.8.8.8 you can use the following command:
```
./restreamer 5000 127.0.0.1 5005 8.8.8.8 5008
```

# License

See the LICENSE file.