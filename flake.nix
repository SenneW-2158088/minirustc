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
            clang

            # Parser generators
            bison
            flex
          ];

          shellHook = ''
            echo "C++ development environment loaded!"
            echo "Available tools:"
            echo " - CMake: $(cmake --version | head -n1)"
            echo " - Clang: $(clang --version | head -n1)"
            echo " - Bison: $(bison --version | head -n1)"
            echo " - Flex:  $(flex --version | head -n1)"
          '';
        };

        packages.default = pkgs.stdenv.mkDerivation {
          name = "mini-rustc";
          src = ./.;

          nativeBuildInputs = with pkgs; [
            cmake
            clang
            bison
            flex
          ];

          buildPhase = ''
            cmake -B build
            cmake --build build
          '';

          installPhase = ''
            mkdir -p $out/bin
            cp build/* $out/bin/
          '';
        };
      }
    );
}
