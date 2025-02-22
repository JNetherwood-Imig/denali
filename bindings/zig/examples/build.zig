const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const denali = b.dependency("denali", .{
        .target = target,
        .optimize = optimize,
    });

    const simple = b.addExecutable(.{
        .name = "simple",
        .target = target,
        .optimize = optimize,
        .root_source_file = b.path("simple.zig"),
    });

    simple.root_module.addImport("dwl", denali.module("root"));

    b.installArtifact(simple);
}
