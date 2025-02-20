const std = @import("std");
const dwl = @import("dwl");

pub fn main() !void {
    const disp = try dwl.Display.connect();
    defer disp.destroy();
    std.debug.print("Connected!\n", .{});
}
