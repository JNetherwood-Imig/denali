const std = @import("std");
const dwl = @import("dwl");

pub fn main() !void {
    std.debug.print("Hello, world!\n", .{});
    const disp = try dwl.Display.connect();
    defer disp.destroy();
}
