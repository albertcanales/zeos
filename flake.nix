{
  inputs = {
    nixpkgs = {
      url = "github:nixos/nixpkgs/nixos-unstable";
    };
  };
  outputs = { self, nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in
    {
      packages.x86_64-linux.default =
        with import nixpkgs { system = "x86_64-linux"; };

        stdenv.mkDerivation {
          pname = "bochs_nogdb";
          version = "2.6.7";
          src = fetchurl {
            url = "mirror://sourceforge/project/bochs/bochs/2.6.7/bochs-2.6.7.tar.gz";
            hash = "sha256-pDVO62mWYtqGmwOOIlFg7HJriD8bT0JaBs9Wy/+7goI=";
          };
          nativeBuildInputs = [
            docbook_xml_dtd_45
            docbook_xsl
            libtool
            pkg-config
          ];
          buildInputs = with pkgs; [
            curl
            readline
            wget
            ncurses
            gtk2
            wxGTK32
            libGL
            libGLU
            xorg.libX11
            xorg.libXpm
          ];
          configureFlags = [
            "--enable-debugger"
            "--enable-disasm"
            "--enable-x86-debugger"
            "--enable-readline"
            "--with-x"
          ];
          postInstall = ''
            mv $out/bin/bochs $out/bin/bochs_nogdb
          '';

          enableParallelBuilding = true;
        };

      packages.x86_64-linux.bochs =
        with import nixpkgs { system = "x86_64-linux"; };

        stdenv.mkDerivation {
          pname = "bochs";
          version = "2.6.7";
          src = fetchurl {
            url = "mirror://sourceforge/project/bochs/bochs/2.6.7/bochs-2.6.7.tar.gz";
            hash = "sha256-pDVO62mWYtqGmwOOIlFg7HJriD8bT0JaBs9Wy/+7goI=";
          };
          nativeBuildInputs = [
            docbook_xml_dtd_45
            docbook_xsl
            libtool
            pkg-config
          ];
          buildInputs = with pkgs; [
            curl
            readline
            wget
            ncurses
            gtk2
            wxGTK32
            libGL
            libGLU
            xorg.libX11
            xorg.libXpm
          ];
          configureFlags = [
            "--enable-gdb-stub"
            "--with-x"
          ];

          enableParallelBuilding = true;
        };

      devShells.${system}.default = pkgs.stdenvNoCC.mkDerivation {
        name = "soa-bochs";
        buildInputs = [
          self.packages.${system}.default
          self.packages.${system}.bochs
          pkgs.dev86
          pkgs.gcc11
          pkgs.gdb
        ];
      };


    };
}
