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
  }:
    flake-utils.lib.eachDefaultSystem (
      system: let
        pkgs = nixpkgs.legacyPackages.${system};
      in {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            # Basic development tools
            cmake

            # Clang and LSP
            clang-tools
            libcxx
            clang

            # Parser generators
            bison
            flex
          ];

          shellHook = ''
            export CPLUS_INCLUDE_PATH="${pkgs.libcxx}/include/c++/v1:${pkgs.clang}/resource-root/include"
            echo "C++ development environment loaded!"
            echo "Available tools:"
            echo " - CMake: $(cmake --version | head -n1)"
            echo " - Clang: $(clang --version | head -n1)"
            echo " - Bison: $(bison --version | head -n1)"
            echo " - Flex:  $(flex --version | head -n1)"
          '';
        };

        packages.default = pkgs.stdenv.mkDerivation {
          name = "minirust-0.1";
          src = ./.;

          nativeBuildInputs = with pkgs; [
            cmake
            clang-tools
            bison
            flex
          ];

          buildPhase = ''
            cmake --build .
          '';

          installPhase = ''
            mkdir -p $out
            cp ./bin/* $out/
          '';
        };
      }
    );
}
