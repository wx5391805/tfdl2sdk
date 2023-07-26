import numpy as np
import cv2
import argparse
from typing import Text


if __name__ == '__main__':
    """
    This program is a example to generate binary input data for calibration.
    In this program, we use OpenCV to read input images, if your input data
    are not images, you can use any other way as same.
    """
    parser = argparse.ArgumentParser(description='calibration binary input data generator example')
    parser.add_argument(
        '--image_list',
        type=Text, required=True,
        help='list to input images'
    )
    parser.add_argument(
        '--input_height',
        type=int, default=224
    )
    parser.add_argument(
        '--input_width',
        type=int, default=224
    )
    args = parser.parse_args()
    image_list = args.image_list
    height = args.input_height
    width = args.input_width

    batch = len(open(image_list, "r").readlines())
    in_file = open(image_list, "r")
    imgs = np.zeros((batch, 3, height, width), dtype=np.float32)

    i = 0
    for line in in_file.readlines():
        img = cv2.imread(line[: -1])
        img = cv2.resize(img, (width, height), interpolation=cv2.INTER_LINEAR)
        img = np.array(img, dtype=np.float32).transpose((2, 0, 1))
        imgs[i] = img
        i += 1

    imgs.tofile('calibration.bin')
