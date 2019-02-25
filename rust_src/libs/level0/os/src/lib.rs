#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));
pub mod Os {
    pub mod File {
        use crate::Os_FileHandle;
        pub fn open(path: &str) -> Os_FileHandle {
            use std::ffi::CString;
            let cs = CString::new(path).expect("CString::new failed");
            unsafe {
                crate::Os_FileOpen(cs.as_ptr(), crate::Os_FileMode_Os_FM_Read)
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use std::ptr;

    #[test]
    fn test_file_open() {
        use crate::Os_FileHandle;
        let fail0 = crate::Os::File::open("test");
        assert_eq!(fail0, ptr::null_mut());
    }
}
