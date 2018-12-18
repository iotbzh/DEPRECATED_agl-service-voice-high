# 1. High Level Voice Service (VSHL)
This repository hosts the code for the AGL's high level voice service binding also known as VSHL.
Please refer to the [architecture](https://confluence.automotivelinux.org/display/SPE/Speech+EG+Architecture) for more information.

# 2. Build Dependencies and License Information

During the build time, the following dependencies are fetched and run by the build system. Please refer to each of the individual entities for the particular licenses.
* [Google Test v1.8.0](https://github.com/google/googletest) when compiled with ENABLE_UNIT_TESTS option.

# 3. Getting the Source Code
```
export MY_PROJECTS_DIR = <Your Project Directory>
pushd $MY_PROJECTS_DIR
git clone --recursive https://gerrit.automotivelinux.org/gerrit/apps/agl-service-voice-high
```

# 4. Renesas R-Car M3 board
## 4.1 Building VSHL

```
pushd agl-voiceservice-highlevel
mkdir build
pushd build
source /opt/agl-sdk/6.0.1-aarch64/environment-setup-aarch64-agl-linux
cmake ..
make autobuild
popd
./conf.d/autobuild/agl/autobuild package
```
* The build output will be located at $MY_PROJECTS_DIR/agl-voiceservice-highlevel/build/vshl.wgt

## 4.2 Running VSHL
```
# afm-util install vshl.wgt
# afm-util start vshl@1.0
```

# 5. Ubuntu 16.04
## 5.1 Building VSHL

```
pushd agl-voiceservice-highlevel/
mkdir build
pushd build
cmake ..
make autobuild
popd
./conf.d/autobuild/linux/autobuild package
````
To build the included unit tests modify the cmake step as following:
cmake .. -DENABLE_UNIT_TESTS=ON

## 5.2 Running VSHL
```
afb-daemon --port=1111 --name=afb-vshl --workdir=$MY_PROJECTS_DIR/agl-voice-service/build/package --ldpaths=lib --roothttp=htdocs --token= -vvv
```

# 6. Running the Unit Tests
## 6.1 Ubuntu 16.04
```
pushd agl-voiceservice-highlevel/
./build/src/plugins/vshl-api_Test
popd
```

# 7. Testing VSHL
* The binding can be tested by launching the HTML5 sample application that is bundled with the package in a browser.

```
http://localhost:1111
```

# 8. Contributing code
Before contributing the source, its recommended to format the code with clang-format. This is done automatically during commit step if following instructions are followed.
**Prerequisite**: Install clang-format-6.0 or greater.
There are following 2 options.

* Before commit, manually run clang-format on the file (It will use local .clang-format file for checking the rules)
```
clang-format -i <path to source file>
```

* Setup clang-format as pre-commit git hook. This is one time step after you clone the repo
```
cd ${VSHL_ROOT}
cp tools/pre-commit .git/hooks/pre-commit
```

* With the hook in place, everytime you try to commit, it will check the format and disallow commit if format doesn't abide by the .clang-format rules.
It will also give you the option to apply a patch (it creates a patch in /tmp folder) to make the source abide by the .clang-format rules. Apply the patch and proceed to commit
```
git apply /tmp/<patch>
git add <source files>
git commit
```