import CSingleHeaders

public class StbImage {

    public enum Channels: Int {
        case grey = 1
        case grey_alpha = 2
        case rgb = 3
        case rgba = 4
    }

    public enum Filter: Int {
        case Box = 1
        case Triangle = 2
        case CubicBSpline = 3
        case CatmullRom = 4
        case MitchellNetrevalli = 5
    }

    public enum ColorSpace: Int {
        case Linear
        case sRGB
    }

    public struct RGBA {
        let R: Float
        let G: Float
        let B: Float
        let A: Float
    }

    public init?(bytes: [UInt8], desiredChannels dc: Channels? = nil) {
        let wantedChannels = (dc != nil) ? Int32(dc!.rawValue) : 0;

        var channelsInFile: Int32 = 0;
        var w: Int32 = 0
        var h: Int32 = 0
        var rd: UnsafeMutablePointer<stbi_uc>? = nil

        bytes.withUnsafeBufferPointer {
            (ptr: UnsafeBufferPointer<stbi_uc>) -> Void in

            rd = stbi_load_from_memory(ptr.baseAddress, Int32(bytes.count),
                    &w, &h, &channelsInFile, wantedChannels)
        }

        if (rd == nil) {
            return nil
        }
        rawData = rd!;

        width = Int(w)
        height = Int(h)

        if let chan = Channels(rawValue: Int(channelsInFile)) {
            channels = chan
        } else {
            return nil
        }

    }

    public init?(filename: String, desiredChannels: Channels? = nil) {
        let wantedChannels = (desiredChannels != nil) ? Int32(desiredChannels!.rawValue) : 0;

        var channelsInFile: Int32 = 0;
        var w: Int32 = 0
        var h: Int32 = 0
        var rd: UnsafeMutablePointer<stbi_uc>? = nil

        rd = stbi_load(filename, &w, &h, &channelsInFile, wantedChannels)
        if (rd == nil) {
            return nil
        }
        rawData = rd!;

        width = Int(w)
        height = Int(h)

        if let chan = Channels(rawValue: Int(channelsInFile)) {
            channels = chan
        } else {
            return nil
        }
    }

    private init(width w: Int, height h: Int, channels c: Channels, rawData rd: UnsafeMutablePointer<stbi_uc>) {
        width = w
        height = h
        channels = c
        rawData = rd
    }

    deinit {
        stbi_image_free(rawData)
    }

    public static func info(bytes: [UInt8]) -> (width: Int, height: Int, channels: Channels)? {
        var channelsInFile: Int32 = 0;
        var w: Int32 = 0
        var h: Int32 = 0
        var okay: Int32 = 0;

        bytes.withUnsafeBufferPointer {
            (ptr: UnsafeBufferPointer<stbi_uc>) -> Void in
            okay = stbi_info_from_memory(ptr.baseAddress, Int32(bytes.count),
                    &w, &h, &channelsInFile)
        }
        if (okay != 0) {
            if let chan = Channels(rawValue: Int(channelsInFile)) {
                return (Int(w), Int(h), chan)
            }
        }
        return nil
    }

    public static func info(filename: String) -> (width: Int, height: Int, channels: Channels)? {
        var channelsInFile: Int32 = 0;
        var w: Int32 = 0
        var h: Int32 = 0
        let okay: Int32 = stbi_info(filename, &w, &h, &channelsInFile)
        if (okay != 0) {
            if let chan = Channels(rawValue: Int(channelsInFile)) {
                return (Int(w), Int(h), chan)
            }
        }
        return nil
    }

    public func resize(newWidth nw: Int, newHeight nh: Int, filter f: Filter? = nil) -> StbImage {
        let filter = (f != nil) ? UInt32(f!.rawValue) : 0;
        let newsize = nw * nh * channels.rawValue

        // this should really use malloc directly as stbi_image_free is using free
        // but seems to work currently as allocate is probably just calling malloc
        let nmem = UnsafeMutablePointer<stbi_uc>.allocate(capacity: newsize)
        stbir_resize_uint8_generic(
                rawData, Int32(width), Int32(height), Int32(channels.rawValue),
                nmem, Int32(nw), Int32(nh), Int32(channels.rawValue),
                Int32(channels.rawValue), 3, 0, STBIR_EDGE_CLAMP,
                stbir_filter(filter), STBIR_COLORSPACE_LINEAR, nil)

        return StbImage(width: nw, height: nh, channels: channels, rawData: nmem)
    }

    public subscript(x: Int, y: Int) -> RGBA {
        let index = (y * width) + x
        return self[index]
    }

    private subscript(index: Int) -> RGBA {
        let numChans = channels.rawValue
        let ptr = rawData + (index * numChans)

        switch (channels) {
        case .grey:
            let val: Float = Float((ptr + 0).pointee) / 255
            return RGBA(R: val, G: val, B: val, A: 1.0)
        case .grey_alpha:
            let val: Float = Float((ptr + 0).pointee) / 255
            let alpha: Float = Float((ptr + 1).pointee) / 255
            return RGBA(R: val, G: val, B: val, A: alpha)
        case .rgb:
            let r: Float = Float((ptr + 0).pointee) / 255
            let g: Float = Float((ptr + 1).pointee) / 255
            let b: Float = Float((ptr + 2).pointee) / 255
            return RGBA(R: r, G: g, B: b, A: 1.0)
        case .rgba:
            let r: Float = Float((ptr + 0).pointee) / 255
            let g: Float = Float((ptr + 1).pointee) / 255
            let b: Float = Float((ptr + 2).pointee) / 255
            let a: Float = Float((ptr + 3).pointee) / 255
            return RGBA(R: r, G: g, B: b, A: a)
        }
    }


    public let width: Int
    public let height: Int
    public let channels: Channels
    public let rawData: UnsafeMutablePointer<stbi_uc>
}
