# 🌠 CppProjectBase

[![CMake](https://img.shields.io/badge/CMake-064F8C?logo=cmake&logoColor=fff)](https://cmake.org/)
[![Conan](https://img.shields.io/badge/-Conan-6699CB?style=flat&logo=conan&logoColor=white)](https://conan.io/)
[![Platforms](https://img.shields.io/badge/Platforms-Windows%20%F0%9F%AA%9F%20%7C%20%F0%9F%90%A7%20Linux-F7C59F?labelColor=0D1117)](#-build--run-instructions)
[![Programming languages](https://img.shields.io/badge/C++-027FDE?style=flat-square&logo=C%2B%2B&logoColor=white)](https://isocpp.org/)

**CppProjectBase** is a minimal **C++ template** designed to quickly bootstrap **cross-platform (Windows / Linux)** projects using **CMake** and **Conan**.  
It currently integrates SFML packages added via Conan, includes a simple `main.cpp`, a basic `CMakeLists.txt`, and build scripts for Windows and Linux.  
<br>

## ⚙️ Features & Goals
This repository serves as a **clean, modern foundation** for new C++ projects:
- Portable setup using **CMake**  
- Dependency management with **Conan**  
- Ready-to-use build scripts for **Windows** and **Linux**  
- Ideal for any C++ application
- Continuous Integration with **GitHub Actions**
<br>

## 📁 Project Structure
| Path | Description |
|------|--------------|
| `CMakeLists.txt` | Main CMake configuration file |
| `conanfile.py` | Conan dependency definitions (includes SFML) |
| `.gitignore` | Excludes build and temporary files |
| `src/` | Project source files |
| `include/` | Public header files - In case you're building a library or a package |
| `lib/` | Third party librairies |
| `docs/` | Documentation files |
| `.github/` | Configuration files for GitHub Actions and other GitHub features |
| `scripts/` | Helper scripts for setup, builds or other tasks |

<br>

## 🧰 Prerequisites

#### 🪟 Windows
- [Python 3](https://www.python.org/downloads/) — required for Conan
- [CMake](https://cmake.org/download/) ≥ 3.27
- [Visual Studio](https://visualstudio.microsoft.com/) — with C++ desktop development tools   
 

#### 🐧 Linux
- **GCC** (added with build essentials)
- **CMake** ≥ 3.27
- **Python 3**  
- **Make**
<br>

## 🧩 How to use this template in your Project
Follow these steps to adapt the template to your own project:

#### 1. Add your dependencies in conanfile.py
Edit the `requires` variable to include the libraries you want to use:

```
requires = (
    "sfml/2.6.1",
    # Add or remove libraries here...
)
```
<br>

#### 2. Add your source and header files
- Place .cpp files in src/
- Place .hpp / .h files in include/
- Organize directories as you like
<br>

#### 3. Update the CMakeLists.txt
##### a -Set your project and executable name 
Update the project name and version here
```
project(YourProjectName VERSION 0.0.1 LANGUAGES CXX)
```
Update the executable name by modifying the variable `MY_TARGET`:
```
set(MY_TARGET your_executable_name)
```
##### b - Add your source files 
Add your .cpp files to the executable with the `add_executable` command here:
```
add_executable(${MY_TARGET}
   src/main.cpp
   # Add your source files here
)
```

##### c - Find and Add required libraries
Use  the `find_package` command to locate each dependency you want to use:
```
find_package(SFML REQUIRED COMPONENTS system window graphics)
```
Then link them to your executable with the `target_link_libraries` command:
```
target_link_libraries(${MY_TARGET}
   PUBLIC sfml::sfml
   # Add your libraries here
)
```
<br>

#### 4. Build the project
Run the appropriate build script:
```
./scripts/build_linux.sh
```
or
```
.\scripts\build_windows.bat
```
Conan will install your dependencies and CMake will configure the project automatically. 

<br>

#### 5. Update the GitHub Actions release name
If you want your release archives to use your project name, edit the file:
```
.github/actions/compress_artifacts/action.yml
``` 
Replace CppProjectBaseApp with your chosen name here:
```
run: powershell Compress-Archive -Path bin -DestinationPath "CppProjectBaseApp-${{ github.ref_name }}-Windows.zip"
```
and here:
```
run: tar -czf "CppProjectBaseApp-${{ github.ref_name }}-Linux.tar.gz" bin
```

<br>

## 💻 Build an run Instructions
#### 1. Clone the repository
```bash
git clone https://github.com/Cesca19/CppProjectBase.git
cd CppProjectBase
```
<br>

#### 2. Launch the build scripts
Open your terminal at the root of the repository and launch the following commands

######  On Linux
```
./scripts/build_linux.sh
```

######  On Windows
```
.\scripts\build_windows.bat
```
<br>

#### 3. Execute the binaries
Ater a successful build, execute the binaries by launching the following commands

######  On Linux
```
./bin/cpp_project_base
```

######  On Windows
```
.\bin\cpp_project_base.exe
```
<br>

## 🛠️ Build Options
The build scripts support configurable options:
* Build Type: Debug (default) or Release
* Runtime Link: dynamic (default) or static
Example:
```
./scripts/build_linux.sh Release static
```
This will produce a Release build with static libgcc/libstdc++, leaving system libraries dynamic. Only the direct dependencies listed in conanfile.py will follow this shared/static setting. 

<br>

## ➿ Continuous Integration (GitHub Actions)
This project includes a ready-to-use CI/CD pipeline powered by GitHub Actions. This pipeline contain
two functionalities : a **build and test of your project on windows and linux** and the creation of a **release 
of your project for windows and linux platforms**
<br><br>

#### 📦 Automated Build & Test on Windows and Linux
Every time you push to the main branch with a commit message containing [build],
GitHub Actions will:
* Automatically run builds on Windows and Linux.
* Cache Conan dependencies (SFML, etc.) to speed up builds.
* Run basic tests and compress the binaries.
* Upload the build artifacts for download.

##### ▶️ How to Trigger it
```
git add .
git commit -m "Update something [build]"
git push origin main
```

##### 🧭 Where to Find Build Artifacts 
Go to your repository’s Actions tab.
* Open the latest workflow run (titled “Build, Test and Release”).
* Scroll to the bottom of the page — you’ll find a section named **“Artifacts”.**
* Download the artifact corresponding to your system:
  * Windows-build → contains the .exe and required files
  * Linux-build → contains the ELF binary and dependencies
<br>

#### 🚀 Create a Release for Windows and Linux 
When you push a tag starting with v (for example v1.0.0), GitHub Actions will automatically:
* Build and test on the project both platforms.
* Download the generated artifacts.
* Create a new GitHub Release with .zip files for Windows and Linux attached.

##### ▶️ How to Trigger it
```
git tag v1.0.0
git push origin v1.0.0
```

##### 🧭 Where to find the realease
Once complete, your release will appear in the release section of your repository home page

<br>

## 🗒️ Notes
* You can customize dependencies directly in `conanfile.py`.
* The build scripts automatically create and configure the `build/` directory.
* Ideal starting point for SFML, SDL, ImGui, boost or pure C++ projects.

## IDE Setup for Autocompletion and Code Navigation

To ensure your IDE correctly recognizes any Conan-installed libraries (like SFML, Boost) 
while programming, follow these steps:

Before launching your IDE make sure to run the right build script
######  On Linux
```
./scripts/build_linux.sh
```

######  On Windows
```
.\scripts\build_windows.bat
```
This command regenerates the build preset, including all paths to the
include directories that will be used by your IDE.
<br>

After that follow the next steps depending on your IDE.

<br>

### VS Code
With VsCode you have two options

#### A - Using CMake Tools Preset
* Make sure the `CMake Tools` extension(ms-vscode.cmake-tools) is installed.
* Open the Command Palette with Ctrl+Shift+P.
* Select hte option: `CMake: Select a Configure Preset`
* Choose `compile-commands` profile preset
  This will then automatically configure autocompletion, include paths, and all other settings.

<br>

#### B - Manual settings.json configuration

In your workspace go to the file `.vscode/settings.json.` and add the following line:
```
"C_Cpp.default.compileCommands": "${workspaceFolder}/build/compile_commands.json"
```
Autocompletion and IntelliSense should now work for all Conan-installed libraries.

<br>

### CLion
#### A - First-time project setup (recommended)
When opening the project for the first time, CLion may show the `Open Project Wizard`.

If so, then: 
* Right under the profile section select the `compile-commands` preset
* Check the `Enable profile` box and apply your changes by pressing th `Ok` button
CLion will then automatically configure autocompletion, include paths, and all other settings.
This step can eliminate most manual setup steps below.

<br>

#### B - Manual setup (if needed)

###### 1. Invalidate caches for safety
   * Go to File → Invalidate Caches
   * Ensure `Clear File System Cache` and `Reset CMake Cache` are enabled for a full reset.
   * Click on the `Invalidate and Restart` button
###### 2. Reload the CMake project
   * Right-click on the CMakeLists.txt file.
   * Select Load / Reload CMake Project
###### 3. Set CMake configure profile preset
   * Go to Settings → Build, Execution, Deployment → CMake.
   * In the Profiles section, select the `compile-commands` profile preset
   * Check the `Enable profile` box and apply your changes

CLion will then automatically configure autocompletion, include paths, and all other settings.

<br>

### Notes
* Both CLion and VS Code will correctly index all Conan dependencies, even if you never build from the IDE.
* The compile_commands.json file is generated automatically by the build script.




