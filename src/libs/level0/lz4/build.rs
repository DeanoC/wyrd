extern crate bindgen;

use std::env;
use std::path::PathBuf;

fn main() {
    println!(r"cargo:rustc-link-search=../../../../lib");
    println!(r"cargo:include=../../../../cpp_src/libs/level0/lz4/include");
    println!("cargo:rustc-link-lib=lz4");

    // The bindgen::Builder is the main entry point
    // to bindgen, and lets you build up options for
    // the resulting bindings.
    let bindings = bindgen::Builder::default()
        .header("../../../../src/libs/level0/lz4/include/lz4/lz4.h")
        .generate()
        .expect("Unable to generate bindings");

    // Write the bindings to the $OUT_DIR/bindings.rs file.
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");
}