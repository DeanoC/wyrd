extern crate bindgen;

use std::env;
use std::path::PathBuf;

fn main() {
    let basepath = r"../../";
    let level0 = String::new() + basepath + r"level0/";

    let libname = "core";
    let libinc = String::new() + &level0 + &libname + "/include/";

    let deps: [&str; 0] = [];

    println!(r"cargo:rustc-link-search=../../../../lib");
    print!("{}", r"cargo:include=[");
    for dep in deps.iter() {
        print!("{}", dep);
    }
    println!("]");
    println!("{}{}", "cargo:rustc-link-lib=", libname);

    // The bindgen::Builder is the main entry point
    // to bindgen, and lets you build up options for
    // the resulting bindings.
    let mut bindings = bindgen::Builder::default()
        .header(String::new() + &libinc + &libname + "/" + "core.h")
        .header(String::new() + &libinc + &libname + "/" + "logger.h")
        .clang_arg(String::new() + "-I" + &libinc)
        .clang_arg("-D_RUST_BINDGEN_");

    for dep in deps.iter() {
        bindings = bindings.clang_arg("-I".to_owned() + dep);
    }

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings.generate()
        .expect("Unable to generate bindings")
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");
}