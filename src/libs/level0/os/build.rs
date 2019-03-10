extern crate bindgen;

use std::env;
use std::path::PathBuf;

fn main() {
    let basepath = r"../../";
    let levels = [
        "level0",
        "level1",
    ];

    let levelpaths = [
        format!("{}{}", basepath, levels[0]),
        format!("{}{}", basepath, levels[1])
    ];

    let libname = "os";
    let libinc = format!("{}/{}{}", levelpaths[0], libname, "/include/");

    let deps = [["core"]];

    let mut deppath = Vec::<String>::new();

    let mut depth = 0;
    for level in deps.iter() {
        for dep in level {
            deppath.push(format!("{}/{}{}", levelpaths[depth], dep, "/include"));
        }
        depth = depth + 1;
    }

    println!(r"cargo:rustc-link-search=../../../../lib");
    print!("{}", r"cargo:include=[");
    for level in deps.iter() {
        for dep in level {
            print!("{}", dep);
        }
    }
    println!("]");
    println!("{}{}", "cargo:rustc-link-lib=", "core");
    println!("{}{}", "cargo:rustc-link-lib=", libname);

    // The bindgen::Builder is the main entry point
    // to bindgen, and lets you build up options for
    // the resulting bindings.
    let mut bindings = bindgen::Builder::default()
        .header(String::new() + &libinc + &libname + "/" + "file.h")
        .header(String::new() + &libinc + &libname + "/" + "filesystem.h")
        .blacklist_type(r"u??int\d?\d_t")
        .blacklist_type(r"u??int_least\d?\d_t")
        .blacklist_type(r"u??int_fast\d?\d_t")
        .blacklist_type(r"u??intmax_t")
        .blacklist_item(r"PLATFORM_??\D*")
        .blacklist_item(r"OS_??\D*")
        .blacklist_item(r"COMPILER_??\D*")
        .blacklist_item(r"CPU_??.*")
        .blacklist_item(r"true_")
        .blacklist_item(r"false_")
        .blacklist_item(r"__bool_true_false_are_defined")
        .clang_arg(String::new() + "-I" + &libinc)
        .clang_arg("-D_RUST_BINDGEN_");

    for dep in deppath.iter() {
        bindings = bindings.clang_arg("-I".to_owned() + dep);
    }

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings.generate()
        .expect("Unable to generate bindings")
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");
}