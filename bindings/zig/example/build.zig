const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "better_wayland_example",
        .target = target,
        .optimize = optimize,
        .root_source_file = b.path("main.zig"),
    });

    const wl = b.dependency("better_wayland", .{
        .target = target,
        .optimize = optimize,
    });

    exe.root_module.addImport("wl", wl.module("better-wayland"));

    b.installArtifact(exe);
}
