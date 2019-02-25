#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

use std::ffi::CStr;

pub fn version() -> i32 {
    return unsafe { LZ4_versionNumber() }
}
pub fn version_string() -> String {
    unsafe {
        CStr::from_ptr(LZ4_versionString()).to_string_lossy().into_owned()
    }

}


#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn major_version_chk() {
        assert_eq!(version(), 10802);
    }
}
