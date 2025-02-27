# TODO

## Subsystems

- [ ] Display
	- Handles connection to wayland socket, sends, recieves, and parses messages,
	holds handles to all singleton objects
	- Encapsulates wl_display, wl_registry, wl_shm, wl_compositor, xdg_wm_base
- [ ] Window
	- Abstraction for various types of windows (toplevel, popup, etc)
	- Encapsulates wl_surface, xdg_surface, xdg_toplevel
- [ ] Surface
	- Abstraction for various types of buffers which can be attached to windows
	and displayed, bridging the gap between CPU shm buffers and
	GPU dmabuf buffers
	Encapsulates wl_shm_pool, wl_buffer, zwp_linux_dmabuf
- [ ] Messages
	- Read display socket and parse into messages, which can be handled and
	transformed into events, as well as cleanly build messages and write them to
	the display socket
- [ ] Events
	- User interface for recieving and handling relevant messages
- [ ] Logger
	- Logs messages with 3-5 verbosity levels to a function pointer supplied by the
	user, or the stderr if none is provided

## Data Structures

- [X] Vector
- [X] Linked list
- [ ] Protocol array
