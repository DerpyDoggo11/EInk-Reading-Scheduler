{
  description = "Kindle GTK devshell";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};

      target = "kindlehf";

      cross = pkgs.pkgsCross.${{
        kindlehf = "armv7l-hf-multiplatform";
        kindlepw2 = "armv7l-multiplatform";
      }.${target}};
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        buildInputs = [
          pkgs.meson
          pkgs.ninja
          pkgs.pkg-config
          pkgs.glib
          pkgs.gtk2
          pkgs.sqlite
          cross.buildPackages.gcc
          pkgs.vim
          pkgs.tinyxxd
        ];

        shellHook = ''
          echo "Kindle target: ${target}"
          echo "Cross CC: ${cross.buildPackages.gcc}/bin/arm-linux-gnueabihf-gcc"
        '';
      };
    };
}
