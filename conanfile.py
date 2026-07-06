import os
from conan import ConanFile
from conan.tools.cmake import cmake_layout
from conan.tools.files import copy
from conan.tools.cmake import CMakeToolchain, CMakeDeps

class ConanProjectConan(ConanFile):
    name = "conan_project"
    version = "1.0"

    settings = "os", "compiler", "build_type", "arch"
    requires = "sfml/2.6.2"
    generators = "CMakeDeps", "CMakeToolchain"
    
    # Option to control shared/static linking of direct dependencies
    options = {
        "isShared": [True, False]
    }
    default_options = {
        "isShared": True,
        "sfml/*:graphics": True,
        "sfml/*:window": True,
        "sfml/*:audio": True,
        "sfml/*:network": True,
        "sfml/*:system": True,
    }
    
    def configure(self):
        """
        Force shared option only for DIRECT dependencies.
        Transitive dependencies (zlib, freetype, etc.) are not affected.
        """
        # List of direct dependencies (parsed from requires attribute)
        direct_deps = []
        if hasattr(self.__class__, 'requires'):
            requires_attr = self.__class__.requires
            if isinstance(requires_attr, str):
                direct_deps = [requires_attr]
            elif isinstance(requires_attr, (tuple, list)):
                direct_deps = list(requires_attr)
        
        # Apply shared option to each direct dependency
        for dep in direct_deps:
            pkg_name = dep.split("/")[0].strip()
            try:
                self.options[pkg_name].shared = self.options.isShared
                self.output.info(f"Setting {pkg_name}:shared={self.options.isShared}")
            except Exception as e:
                self.output.warn(f"Could not set shared for {pkg_name}: {e}")
        
    def layout(self):
        """Defines folder structure for build and generated files"""
        self.folders.build = "build"
        self.folders.generators = "build/generators"

    def generate(self):
        """Copies dependency DLLs (like SFML) next to the final executable."""
        exe_dir = os.path.join(self.source_folder, "bin")
        os.makedirs(exe_dir, exist_ok=True)
        for dep in self.dependencies.values():
            # SO (Linux)
            for lib_path in dep.cpp_info.libdirs:
                copy(self, "*.so*", src=lib_path, dst=exe_dir)
            # DYLIB (MacOS)
            for lib_path in dep.cpp_info.libdirs:
                copy(self, "*.dylib", src=lib_path, dst=exe_dir)
            # DLL (Windows)
            for bin_path in dep.cpp_info.bindirs:
                copy(self, "*.dll", src=bin_path, dst=exe_dir)








        
