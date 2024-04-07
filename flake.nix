{
  description = "A template for Nix based C++ project setup.";

  inputs = {
    # Pointing to the current stable release of nixpkgs. You can
    # customize this to point to an older version or unstable if you
    # like everything shining.
    #
    # E.g.
    #
    # nixpkgs.url = "github:NixOS/nixpkgs/unstable";
    #    hstherac25.url = "github:pillowtrucker/hstherac25";
    nixpkgs.url = "github:NixOS/nixpkgs/master";

    utils.url = "github:numtide/flake-utils";

  };

  outputs = { self, nixpkgs, ... }@inputs:
    inputs.utils.lib.eachSystem [
      # Add the system/architecture you would like to support here. Note that not
      # all packages in the official nixpkgs support all platforms.
      "x86_64-linux"
      "i686-linux"
      "aarch64-linux"
      "x86_64-darwin"
    ] (system:
      let
        #        opencv-pkgs = import nixpkgs-opencv { inherit system; };
        pkgs = import nixpkgs {
          inherit system;

          # Add overlays here if you need to override the nixpkgs
          # official packages.
          overlays = [
            (final: prev: rec {
              llvmPackages = prev.llvmPackages_18;
              x265 = prev.x265.override {
                stdenv = pkgs.llvmPackages_18.libcxxStdenv;
              };
              srt = prev.srt.override {
                stdenv = pkgs.llvmPackages_18.libcxxStdenv;
              };
              openexr = prev.openexr.override {
                stdenv = pkgs.llvmPackages_18.libcxxStdenv;
              };
              opencv = prev.opencv.override {
                stdenv = pkgs.llvmPackages_18.libcxxStdenv;
                protobuf_21 = prev.protobuf_21.override {
                  gtest = prev.gtest.override {
                    stdenv = pkgs.llvmPackages_18.libcxxStdenv;
                  };
                  stdenv = pkgs.llvmPackages_18.libcxxStdenv;
                };
              };
              giflib = prev.giflib.overrideAttrs rec {
                version = "5.1.8";
                src = prev.fetchurl {
                  url = "mirror://sourceforge/giflib/giflib-${version}.tar.gz";
                  hash = "sha256-0QWpBd80p4IhctE2V82uPUsMjoxwZ8zwXjmkAET4ylM=";
                  #                  hash = "sha256-vn/70FfK3r4qoURUL9kMaDjGoIO16KkEi47jtmsp1fs=";
                };
                patches = [ ];
              };

              boost174 = prev.callPackage
                "${nixpkgs}/pkgs/development/libraries/boost/generic.nix" (rec {
                  version = "1.74.0";
                  src = prev.fetchurl {
                    urls = [
                      "mirror://sourceforge/boost/boost_${
                        builtins.replaceStrings [ "." ] [ "_" ] version
                      }.tar.bz2"
                      "https://boostorg.jfrog.io/artifactory/main/release/${version}/source/boost_${
                        builtins.replaceStrings [ "." ] [ "_" ] version
                      }.tar.bz2"
                    ];
                    # SHA256 from http://www.boost.org/users/history/version_1_74_0.html
                    sha256 =
                      "83bfc1507731a0906e387fc28b7ef5417d591429e51e788417fe9ff025e116b1";
                  };
                });
            })

          ];

          # Uncomment this if you need unfree software (e.g. cuda) for
          # your project.
          #
          config.allowUnfree = true;
        };
      in {
        devShells.default = pkgs.llvmPackages_18.libcxxStdenv.mkDerivation rec {

          # Update the name to something that suites your project.
          name = "GAME_shell";
          #            stdenv = pkgs.llvmPackages_18.libcxxStdenv;

          packages = with pkgs; [
            #              inputs.hstherac25.packages.${system}.lib
            #              inputs.hstherac25.packages.${system}.default
            #              inputs.hstherac25.devShells.${system}.default.buildInputs
            #              inputs.hstherac25.devShells.${system}.default.nativeBuildInputs
            #              inputs.hstherac25.devShells.${system}.default.ghc
            #              inputs.hstherac25.devShells.${system}.default.all
            #              inputs.hstherac25.devShells.${system}.default.ghc
            #              inputs.hstherac25.outputs.devShells.x86_64-linux.default.ghc
            haskell.compiler.ghc982
            # Development Tools
            #            llvmPackages_18.clang
            (clang-tools.override { llvmPackages = llvmPackages_18; })
            llvmPackages_18.bintools
            python3
            git
            #            llvmPackages_18.libraries.libcxx
            #              llvmPackages_18.libcxx
            llvmPackages_18.compiler-rt
            cmake
            cmakeCurses
            ninja

            #            stdenv.ld
            # Development time dependencies
            #            gtest
            vulkan-validation-layers
            # Build time and Run time dependencies
            vulkan-loader
            vulkan-headers
            vulkan-tools
            pkg-config
            xorg.libX11
            xorg.libX11.dev
            libdrm
            libxkbcommon
            libxkbcommon.dev
            libGL
            libGL.dev
            xorg.libXext
            xorg.libXv
            xorg.libXrandr
            xorg.xinput
            ffmpeg
            mesa_glu
            alsa-lib
            curl
            #              boost178
            boost174
            libmpg123
            opencv
            opusfile
            soundtouch
            libwebp
            giflib
            #              util
            #              (giflib.overrideAttrs { version = "5.1.8"; })
            libogg
            libopus
            libvorbis
            xorg.libxcb
            xorg.libXext.dev
            xorg.libXv.dev
            xorg.libXrandr.dev
            xorg.libxcb.dev
            xorg.libXi
            xorg.libXi.dev
            xorg.libXinerama
            xorg.libXinerama.dev
            zlib
            gtk3
            libuuid
            wayland
            pulseaudio
            #            spdlog
            #            abseil-cpp
          ];
          buildInputs = packages;
          #            propagatedBuildInputs = packages;
          # Setting up the environment variables you need during
          # development.
          #      shellHook = let
          #        icon = "f121";
          #      in ''
          #        export PS1="$(echo -e '\u${icon}') {\[$(tput sgr0)\]\[\033[38;5;228m\]\w\[$(tput sgr0)\]\[\033[38;5;15m\]} (${name}) \\$ \[$(tput sgr0)\]"
          #      '';
        };

        #        packages.default = pkgs.callPackage ./default.nix {
        #          stdenv = pkgs.llvmPackages_18.libcxxStdenv;
        #        };
      });
}
