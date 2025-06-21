{
  lib,
  hyprland,
  hyprlandPlugins,
}:
hyprlandPlugins.mkHyprlandPlugin hyprland {
  pluginName = "hypredge";
  version = "0.1";
  src = ./.;

  inherit (hyprland) nativeBuildInputs buildInputs;

  meta = with lib; {
    homepage = "https://github.com/CyrenArkade/hypredge";
    description = "Mouse! Edge! wow..";
    license = licenses.gpl3;
    platforms = platforms.linux;
  };
}