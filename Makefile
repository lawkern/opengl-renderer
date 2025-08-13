CFLAGS = -g3 -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter
LDLIBS = -lGL

WL_SCANNER   = $$(pkg-config wayland-scanner --variable=wayland_scanner)
WL_PROTOCOLS = $$(pkg-config wayland-protocols --variable=pkgdatadir)
WL_CLIENT    = $$(pkg-config wayland-client --cflags --libs)
WL_EGL       = $$(pkg-config wayland-egl --cflags --libs)
EGL          = $$(pkg-config egl --cflags --libs)

compile:
	eval $(WL_SCANNER) client-header $(WL_PROTOCOLS)/stable/xdg-shell/xdg-shell.xml src/xdg-shell-client-protocol.h
	eval $(WL_SCANNER) private-code  $(WL_PROTOCOLS)/stable/xdg-shell/xdg-shell.xml src/xdg-shell-protocol.c
	eval $(CC) -o opengl_renderer_wayland src/main_wayland.c src/xdg-shell-protocol.c $(CFLAGS) $(LDLIBS) $(WL_CLIENT) $(WL_EGL) $(EGL)

debug:
	gdb opengl_renderer_wayland
