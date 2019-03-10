#[cfg(test)]
mod tests {
    use os::file;

    #[test]
    fn test_file_open() {
        use std::fs;
        let livepath = concat!(env!("OUT_DIR"), "/../../../../../../live");
        let livepath = fs::canonicalize(livepath).expect("live dir can't be canonicalized");
        std::env::set_current_dir(livepath).expect("can't set current dir to live");

        let fail0 = file::open("test");
        assert_eq!(fail0.is_err(), true);
        let ok0 = file::open("test_data/test.txt");
        assert_ne!(ok0.is_err(), true);
    }

    #[test]
    fn test_file_flush() {
        use std::fs;
        let livepath = concat!(env!("OUT_DIR"), "/../../../../../../live");
        let livepath = fs::canonicalize(livepath).expect("live dir can't be canonicalized");
        std::env::set_current_dir(livepath).expect("can't set current dir to live");

        let ok0 = file::open("test_data/test.txt");
        assert_ne!(ok0.is_err(), true);
        let handle = ok0.unwrap();
        file::flush(&handle);
    }

    #[test]
    fn test_file_size() {
        use std::fs;
        let livepath = concat!(env!("OUT_DIR"), "/../../../../../../live");
        let livepath = fs::canonicalize(livepath).expect("live dir can't be canonicalized");
        std::env::set_current_dir(livepath).expect("can't set current dir to live");

        let ok0 = file::open("test_data/test.txt");
        assert_ne!(ok0.is_err(), true);
        let handle = ok0.unwrap();
        assert_eq!(file::size(&handle), 15);
    }

    #[test]
    fn test_file_read() {
        use std::fs;
        let livepath = concat!(env!("OUT_DIR"), "/../../../../../../live");
        let livepath = fs::canonicalize(livepath).expect("live dir can't be canonicalized");
        std::env::set_current_dir(livepath).expect("can't set current dir to live");

        {
            let ok0 = file::open("test_data/test.txt");
            assert_ne!(ok0.is_err(), true);
            let handle = ok0.unwrap();
            let mut v: Vec<u8> = Vec::new();
            v.resize(100, 0);

            let amount = file::read(&handle, &mut v);
            assert_eq!(amount, 15);
            v.resize(amount, 0);
            assert_eq!(v, "Testing 1, 2, 3".as_bytes());
        }

        {
            let ok1 = file::open("test_data/test.txt");
            assert_ne!(ok1.is_err(), true);
            let handle = ok1.unwrap();
            let mut mem: Vec<u8> = Vec::new();
            mem.resize(15, 0);

            let amount = file::read(&handle, &mut mem);
            assert_eq!(amount, 15);
            assert_eq!(mem, "Testing 1, 2, 3".as_bytes());
        }
        {
            let ok1 = file::open("test_data/test.txt");
            assert_ne!(ok1.is_err(), true);
            let handle = ok1.unwrap();
            let mut mem: Vec<u8> = Vec::new();
            mem.resize(100, 0);

            let amount0 = file::read_into(&handle, &mut mem, 0, 7);
            let amount1 = file::read_into(&handle, &mut mem, 7, 8);
            assert_eq!(amount0 + amount1, 15);
            mem.resize(15, 0);
            assert_eq!(mem, "Testing 1, 2, 3".as_bytes());
        }
        {
            let ok0 = file::open("test_data/test.txt");
            assert_ne!(ok0.is_err(), true);
            let handle = ok0.unwrap();
            let mem = file::read_all(&handle);
            assert_eq!(mem, "Testing 1, 2, 3".as_bytes());
        }
    }

    #[test]
    fn test_file_seek() {
        use std::fs;
        let livepath = concat!(env!("OUT_DIR"), "/../../../../../../live");
        let livepath = fs::canonicalize(livepath).expect("live dir can't be canonicalized");
        std::env::set_current_dir(livepath).expect("can't set current dir to live");

        let ok0 = file::open("test_data/test.txt");
        assert_ne!(ok0.is_err(), true);
        let handle = ok0.unwrap();
        assert_eq!(file::tell(&handle), 0);
        file::seek_from_begin(&handle, 0);
        assert_eq!(file::tell(&handle), 0);
        file::seek_from_current(&handle, 0);
        assert_eq!(file::tell(&handle), 0);

        let mut v: Vec<u8> = Vec::new();
        v.resize(100, 0);
        file::read(&handle, &mut v);
        assert_eq!(file::tell(&handle), 15);
        file::seek_from_begin(&handle, 0);
        assert_eq!(file::tell(&handle), 0);
        file::seek_from_end(&handle, -1);
        assert_eq!(file::tell(&handle), 14);
        let amt = file::read(&handle, &mut v);
        v.resize(amt, 0);
        assert_eq!(v, "3".as_bytes());
    }
}