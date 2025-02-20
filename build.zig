const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const lib = b.addStaticLibrary(.{
        .name = "better_wayland_c",
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });

    lib.addCSourceFiles(.{
        .files = &.{
            "src/display.c",
        },
        .flags = &.{
            "-std=c23",
        },
    });

    lib.addIncludePath(b.path("include"));
    lib.addIncludePath(b.path("include/wl"));

    const mod = b.addModule("better-wayland", .{
        .root_source_file = b.path("bindings/zig/src/root.zig"),
        .target = target,
        .optimize = optimize,
    });

    mod.linkLibrary(lib);
    mod.addIncludePath(b.path("include/wl"));
}
