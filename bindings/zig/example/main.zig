const std = @import("std");
const wl = @import("wl");

pub fn main() !void {
    std.debug.print("Hello, world!\n", .{});
    const disp = try wl.Display.connect();
    defer disp.destroy();
}
