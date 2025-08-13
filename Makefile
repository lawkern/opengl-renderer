CFLAGS = -g3 -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter
LDLIBS = -lGL

WL_SCANNER   = $$(pkg-config wayland-scanner --variable=wayland_scanner)
WL_PROTOCOLS = $$(pkg-config wayland-protocols --variable=pkgdatadir)
WL_CLIENT    = $$(pkg-config wayland-client --cflags --libs)
WL_EGL       = $$(pkg-config wayland-egl --cflags --libs) $$(pkg-config egl --cflags --libs)

WL_SHELL_PATH = stable/xdg-shell/xdg-shell.xml
WL_DECORATION_PATH = unstable/xdg-decoration/xdg-decoration-unstable-v1.xml

compile:
	eval $(WL_SCANNER) client-header $(WL_PROTOCOLS)/$(WL_SHELL_PATH) src/xdg-shell-client-protocol.h
	eval $(WL_SCANNER) private-code  $(WL_PROTOCOLS)/$(WL_SHELL_PATH) src/xdg-shell-protocol.c

	eval $(WL_SCANNER) client-header $(WL_PROTOCOLS)/$(WL_DECORATION_PATH) src/xdg-decoration-unstable-v1-client-protocol.h
	eval $(WL_SCANNER) private-code  $(WL_PROTOCOLS)/$(WL_DECORATION_PATH) src/xdg-decoration-unstable-v1-protocol.c

	eval $(CC) -o opengl_renderer_wayland src/main_wayland.c src/xdg-shell-protocol.c src/xdg-decoration-unstable-v1-protocol.c $(CFLAGS) $(LDLIBS) $(WL_CLIENT) $(WL_EGL)

debug:
	gdb opengl_renderer_wayland
