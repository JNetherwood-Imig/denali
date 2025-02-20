const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "example",
        .target = target,
        .optimize = optimize,
        .root_source_file = b.path("main.zig"),
    });

    const wl = b.dependency("denali", .{
        .target = target,
        .optimize = optimize,
    });

    exe.root_module.addImport("dwl", wl.module("denali"));

    b.installArtifact(exe);
}
