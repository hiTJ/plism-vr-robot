# plism-vr-robot
This repository is for plism.

# Comment
とりあえずxwiimoteをビルドすれば良い  
toolsあたりをうまく使えそう  
core.cをいじるのもあり

# hou to build xwiimote

first time only
```
sudo apt-get install autoconf automake libtool libudev-dev ncurses-dev
cd xwiimote
./autogen.sh
```

if successeeded :
```
Run "make" to start compilation process
```

```
make
sudo make install
```


# connecting bluetooth

Wii remo-con
```
[CHG] Device 8C:CD:E8:60:18:53 Name: Nintendo RVL-CNT-01-TR
```

connect
```
bluetoothctl
pair 8C:CD:E8:60:18:53
connect 8C:CD:E8:60:18:53
trust 8C:CD:E8:60:18:53
```
