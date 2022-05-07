const sharp = require('sharp');
const fs = require('fs');

var files = [...process.argv];
files.splice(0, 2);
files.forEach((path) => {
    var img = sharp(path);
    var width = 0;
    var height = 0;
    var channels = 0;
    img
        .metadata()
        .then(metadata => {
            width = metadata.width;
            height = metadata.height;
            channels = metadata.channels;

            return img.raw().toBuffer();
        })
        .then(data => {
            var colors = [];
            var alpha = [];
            for (var x = 0; x < width; x++)
                for (var y = 0; y < height; y++) {
                    var pixelr = data.readUInt8((y * width + x) * channels);
                    var pixelg = data.readUInt8((y * width + x) * channels + 1);
                    var pixelb = data.readUInt8((y * width + x) * channels + 2);
                    var pixela = data.readUInt8((y * width + x) * channels + 3);
                    colors[y * width + x] = ((pixelr & 0b11111000) << 8) + ((pixelg & 0b11111100) << 3) + (pixelb >> 3);
                    alpha[y * width + x] = pixela >> 3;
                }

            return {
                colors: colors,
                alpha: alpha
            };
        })
        .then((data) => {
            var colors = data.colors;
            var alpha = data.alpha;
            
            var newPath = path.substring(0, path.lastIndexOf("."));
            var name = newPath.substring(Math.max(newPath.lastIndexOf("/"), newPath.lastIndexOf("\\")) + 1);

            var string = `const color_t ${name}[${colors.length}] = {\n`;

            for (var y = 0; y < height; y++) {
                string += "    ";
                for (var x = 0; x < width; x++) {
                    string += `0x${colors[y * width + x].toString(16).padStart(4, "0")}`;
                    if (x < width || y < height)
                        string += ",";
                }
                string += "\n";
            }

            string += `};\n\nconst unsigned char ${name}[${alpha.length}] = {\n`;

            for (var y = 0; y < height; y++) {
                string += "    ";
                for (var x = 0; x < width; x++) {
                    string += `0x${alpha[y * width + x].toString(16).padStart(2, "0")}`;
                    if (x < width || y < height)
                        string += ",";
                }
                string += "\n";
            }

            string += "};";

            fs.writeFile(newPath + ".h", string, undefined, (err) => {
                if (err)
                    console.log(err);
            });
        });
});