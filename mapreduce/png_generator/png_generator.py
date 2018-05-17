import sys
import os

from PIL import Image

if __name__ == '__main__':
    input_path = sys.argv[1]
    output_path = sys.argv[2]

    with open(input_path) as f:
        content = f.readlines()

    content = [x.strip() for x in content]

    first_element = content.pop(0)
    first_element = (first_element.split(' '))[1].split(',')
    width = int(first_element[0])
    height = int(first_element[1])

    pixel_data = []
    i = 0
    im = Image.new('RGB', (width, height))

    for line in content:
        line = line.split(' ')
        pixel = tuple(map(int, line[1].split(',')))
        pixel_data.append(pixel)
        i += 1
        if i == width:
            im.putdata(pixel_data)
            i = 0

    save_path = os.path.join(output_path, os.path.basename(input_path) + '.png')
    print(save_path)
    im.save(save_path)
