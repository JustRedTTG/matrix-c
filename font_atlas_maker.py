import os.path
import struct

import pygameextra as pe

FONT = "JiyunoTsubasa.ttf"
NAME = "matrix_font"
INFO = "matrixFontInfo"
CHARACTERS = "ﾊﾐﾋｰｳｼﾅﾓﾆｻﾜﾂｵﾘｱﾎﾃﾏｹﾒｴｶｷﾑﾕﾗｾﾈｽﾀﾇﾍ012345789Z:・.\"=*+-<>¦｜"
FONT_SIZE = 20
h_file = """
#ifndef FONT_ATLAS_INFO_H
#define FONT_ATLAS_INFO_H
#include "fonts.h"

static constexpr FontInfo {} = {{
    .width = {},
    .height = {},
    .size = {},
    .characterCount = {},
    .characterInfoList = {{
        {}
    }}
}};

#endif //FONT_ATLAS_INFO_H

"""

os.makedirs(INCLUDE_DIRECTORY := os.path.join('assets', 'fonts', 'include'), exist_ok=True)


class CustomMetricsHandler(pe.SheetHandler):
    def __init__(self, metrics):
        x = 0
        self.mapping = []
        for c in metrics:
            self.mapping.append((x, 0, c[4], FONT_SIZE))
            x += c[4]

    def map(self, surface):
        pass


# Initialize PGE
print("Using PGE version", pe.__version__)
pe.init((0, 0))

text = pe.Text(CHARACTERS, os.path.join('assets', 'fonts', FONT), FONT_SIZE, colors=[(255, 0, 0), None])
metrics = text.font.metrics(text.text)

# Pack the characters into a texture atlas
sheet = pe.Sheet(text.obj, CustomMetricsHandler(metrics))
atlas = pe.Atlas.from_sheets({
    "_": sheet,
})

# Create a PNG file with the atlas
# with atlas.surface:
#     for mapping in atlas.mappings['_']:
#         pe.draw.rect(pe.colors.blue, mapping, 1)
#
# atlas.surface.save_to_file(f"{NAME}.png")

with open(os.path.join('assets', 'fonts', f'{NAME}.raw'), 'wb') as file:
    for y in range(atlas.surface.height):
        for x in range(atlas.surface.width):
            r, g, b, a = atlas.surface.get_at((x, y))
            intensity = int(r * (a / 255))

            file.write(struct.pack('B', intensity))

with open(os.path.join(INCLUDE_DIRECTORY, f'{NAME}_info.h'), 'w') as file:
    file.write(
        h_file.format(
            INFO, atlas.surface.width, atlas.surface.height, FONT_SIZE, len(CHARACTERS),
            ',\n\t\t'.join(
                f'{{{mapping_element[0]},{mapping_element[1]},{mapping_element[2]},{mapping_element[3]}}}'
                for mapping_element in atlas.mappings['_']
            )
        )
    )
3