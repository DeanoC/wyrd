#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

use std::io;
use std::io::ErrorKind;

pub struct Handle(Os_FileHandle);

impl Drop for Handle {
    fn drop(&mut self) {
        unsafe { Os_FileClose(self.0); }
    }
}

pub fn open(path: &str) -> Result<Handle, io::Error> {
    use std::ffi::CString;
    let cs = CString::new(path).expect("CString::new failed");
    unsafe {
        let fh = Os_FileOpen(cs.as_ptr(), Os_FileMode_Os_FM_Read);
        if fh == std::ptr::null_mut() {
            Err(std::io::Error::new(ErrorKind::NotFound, "err"))
        } else {
            Ok(Handle(fh))
        }
    }
}

pub fn flush(handle: &Handle) {
    unsafe {
        Os_FileFlush(handle.0);
    }
}

pub fn size(handle: &Handle) -> usize {
    unsafe {
        Os_FileSize(handle.0)
    }
}

pub fn read(handle: &Handle, mem: &mut Vec<u8>) -> usize {
    read_into(handle, mem, 0, mem.len())
}

pub fn read_all(handle: &Handle) -> Vec<u8> {
    let size = size(handle);
    let mut mem : Vec<u8> = Vec::new();
    mem.resize(size, 0);
    read_into(handle, &mut mem, 0, size);
    return mem;
}

pub fn read_into(handle: &Handle, mem: &mut Vec<u8>, offset: usize, amount: usize) -> usize {
    if mem.len() < offset + amount {
        return 0;
    }

    unsafe {
        let ptr = mem.as_mut_ptr().offset(offset as isize);
        Os_FileRead(handle.0,
                    ptr as *mut std::ffi::c_void,
                    amount)
    }
}

pub fn seek_from_begin(handle: &Handle, offset: isize) {
    unsafe {
        Os_FileSeek(handle.0, offset as i64, Os_FileSeekDir_Os_FSD_Begin);
    }
}

pub fn seek_from_current(handle: &Handle, offset: isize) {
    unsafe {
        Os_FileSeek(handle.0, offset as i64, Os_FileSeekDir_Os_FSD_Current);
    }
}

pub fn seek_from_end(handle: &Handle, offset: isize) {
    unsafe {
        Os_FileSeek(handle.0, offset as i64, Os_FileSeekDir_Os_FSD_End);
    }
}
pub fn tell(handle: &Handle) -> isize {
    unsafe {
        Os_FileTell(handle.0) as isize
    }
}