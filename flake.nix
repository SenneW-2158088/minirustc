{
  description = "C++ development environment for minirust compiler";
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = {
    self,
    nixpkgs,
    flake-utils,
  }: let
    supportedSystems = ["x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin"];
    forEachSupportedSystem = f:
      nixpkgs.lib.genAttrs supportedSystems (system:
        f {
          pkgs = import nixpkgs {inherit system;};
        });
  in {
    devShells = forEachSupportedSystem ({pkgs}: {
      default =
        pkgs.mkShell.override {
          stdenv = pkgs.llvmPackages_17.stdenv; # Use LLVM 17 toolchain
        }
        {
          packages = with pkgs; [
            # C++ toolchain
            clang
            libcxx
            clang-tools # for clangd

            # Build tools
            cmake
            bison
            flex
          ];

          shellHook = ''
            export CXXFLAGS="-nostdinc++ -I${pkgs.libcxx}/include/c++/v1"
            export LDFLAGS="-L${pkgs.libcxx}/lib"
          '';
        };
    });
  };
}
