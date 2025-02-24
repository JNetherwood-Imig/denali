use std::path::PathBuf;

fn main() {
    let src_dir = PathBuf::from("../../../src");
    let include_dir = PathBuf::from("../../../include");

    cc::Build::new()
        .include(include_dir)
        .file(src_dir.join("display.c"))
        .file(src_dir.join("message.c"))
        .std("c23")
        .compile("denali");
}
