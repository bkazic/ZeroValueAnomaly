# How to create a project

## Setting up existing solutin
- First time, we have to build with node-gyp `npm install --build-from-source`
- After the first build you can open the Visual Studio solution `\build\binding.sln` and build from IDE.
- See QMiner homepage for detailed instructions (https://qminer.github.io/setup/)

## From Sctrach (Not recomended!)
- Create new Windows Console Application project

- Remove pch files from Header and Source Files (e.g. `pch.h` and `pch.cpp`)
    - `Options -> Configureation Properties -> C/C++ -> Precompiled Headers -> Not Using Precompiled Headers`

- Setup Multy-Byte Character set
    - `Options -> Configureation Properties -> General -> Character Set -> Use Multi-Byte Character Set`

- Add Additional Include Directories
	- `Options -> Configureation Properties -> C/C++ -> Additional Include Directories`
	- Add required glib directories (etc. `glib/base, glib/mine, third_party/sole`)
	- Add required qminer directories (etc. `src/qminer`)
	- You can use relative paths (e.g `$(SolutionDir)..\qminer\src\glib\base`)

- Setup Linker 
	- `Options -> Configureation Properties -> Linker -> Input`
	- Add path to glib lib (e.g. `$(SolutionDir)..\qminer\$(Configuration)\glib.lib`) 

- Set prject to Debug in x64 platform

- Sync Code Generation with qminer
    - `Options -> Configureationa Properties -> C/C++ -> Code Generation -> Runtime Library -> Multi-threaded Debug (/MTd)`
