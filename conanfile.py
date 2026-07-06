import os
from conan import ConanFile
from conan.tools.files import copy


class FlowBrokerConanProject(ConanFile):
    name = "conan_project"
    version = "1.0"

    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    # Direct dependencies, declared once and reused in both
    # requirements() and configure().
    direct_requires = (
        "boost/1.91.0",
        "qt/6.11.1",
    )

    # Option controlling shared/static linking of direct dependencies.
    options = {
        "isShared": [True, False],
    }
    default_options = {
        "isShared": True,
        # Qt modules the client actually needs.
        "qt/*:widgets": True,     # live value table
        "qt/*:qtcharts": True,    # real-time chart

        # Disable Qt SQL database drivers we do not use.
        # The PostgreSQL driver pulls in libpq, which fails to link against
        # OpenSSL during Qt's from-source build. Turning these off removes
        # libpq entirely and fixes the link error.
        "qt/*:with_pq": False,
        "qt/*:with_mysql": False,
        "qt/*:with_odbc": False,
    }

    def requirements(self):
        for dep in self.direct_requires:
            self.requires(dep)

    def configure(self):
        """
        Force the shared option only on direct dependencies.
        Transitive dependencies (zlib, freetype, etc.) are left untouched.
        """
        for dep in self.direct_requires:
            pkg_name = dep.split("/")[0].strip()
            try:
                self.options[pkg_name].shared = self.options.isShared
                self.output.info(f"Setting {pkg_name}:shared={self.options.isShared}")
            except Exception as e:
                self.output.warning(f"Could not set shared for {pkg_name}: {e}")

    def layout(self):
        """Define the folder structure for build and generated files."""
        self.folders.build = "build"
        self.folders.generators = "build/generators"

    def generate(self):
        """Copy dependency shared libraries next to the final executable."""
        exe_dir = os.path.join(self.source_folder, "bin")
        os.makedirs(exe_dir, exist_ok=True)
        for dep in self.dependencies.values():
            for lib_path in dep.cpp_info.libdirs:
                copy(self, "*.so*", src=lib_path, dst=exe_dir)   # Linux
                copy(self, "*.dylib", src=lib_path, dst=exe_dir)  # macOS
            for bin_path in dep.cpp_info.bindirs:
                copy(self, "*.dll", src=bin_path, dst=exe_dir)   # Windows