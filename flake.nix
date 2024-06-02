{
  description = "flake";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";

  outputs = { self, nixpkgs }:
    let pkgs = nixpkgs.legacyPackages.x86_64-linux;
    in rec {
      formatter.x86_64-linux = pkgs.nixfmt-classic;

      packages.x86_64-linux.default = pkgs.clang18Stdenv.mkDerivation {
        name = "Birdy3d";
        src = ./.;
        nativeBuildInputs = with pkgs; [ cmake ninja pkg-config ];
        buildInputs = with pkgs; [
          assimp
          doctest
          fmt
          fontconfig
          glfw
          glm
          libGL
        ];

        meta.mainProgram = "Birdy3d_sandbox";
      };

      devShells.x86_64-linux.default =
        packages.x86_64-linux.default.overrideAttrs (attrs: {
          name = "Birdy3d";
          packages = with pkgs; [ gdb perf-tools hotspot ];
          nativeBuildInputs = attrs.nativeBuildInputs
            ++ (with pkgs; [ ccache ]);
          NIX_HARDENING_ENABLE = "";
          NIX_ENFORCE_PURITY = 0;
          ASAN_OPTIONS = "symbolize=1";
          ASAN_SYMBOLIZER_PATH = "${pkgs.llvm}/bin/llvm-symbolizer";
        });
    };
}
