import numpy as np
from PIL import Image, ImageDraw, ImageFont, ImageEnhance
import random

from PIL.Image import Resampling
from tensorflow.keras.utils import to_categorical

FONT_PATH = "DejaVuSans.ttf"
IMG_SIZE = 30
SAMPLES_PER_CLASS = 1000
NOISE_LEVEL = 0.07

symbols = [
    "∩",
    "∪",
    "∖",
    "Δ",
    "∅",
    "U",
    "⊆",
    "⊂"
]

class_names = [
    "Intersection", "Union", "Difference", "SymmetricDiff",
    "EmptySet", "Universal", "Inclusion", "StrictInclusion"
]

font_size = 24
font = ImageFont.truetype(FONT_PATH, font_size)

X = []
y = []

print("Генерация датасета...")

for class_idx, symbol in enumerate(symbols):
    print(f"Обрабатывается {class_names[class_idx]} ({symbol}) — {class_idx + 1}/{len(symbols)}")

    for i in range(SAMPLES_PER_CLASS):

        img = Image.new('L', (IMG_SIZE, IMG_SIZE), 0)
        draw = ImageDraw.Draw(img)

        scale = random.uniform(0.7, 1.2)
        angle = 0
        offset_x = 0
        offset_y = 0

        temp_size = 80
        temp = Image.new('L', (temp_size, temp_size), 0)
        temp_draw = ImageDraw.Draw(temp)

        bbox = temp_draw.textbbox((0, 0), symbol, font=font)
        w = bbox[2] - bbox[0]
        h = bbox[3] - bbox[1]
        pos = ((temp_size - w) // 2, (temp_size - h) // 2)

        temp_draw.text(pos, symbol, font=font, fill=255)

        new_size = int(temp_size * scale)
        temp = temp.resize((new_size, new_size), Resampling.BICUBIC)

        temp = temp.rotate(angle, resample=Resampling.BICUBIC, expand=True)

        paste_x = (IMG_SIZE - temp.width) // 2 + offset_x
        paste_y = (IMG_SIZE - temp.height) // 2 + offset_y

        img.paste(temp, (paste_x, paste_y))

        arr = np.array(img, dtype=np.float32)

        vector = arr.ravel() / 255.0

        X.append(vector)
        y.append(class_idx)

        if (i + 1) % 1000 == 0:
            print(f"    → {i + 1}/{SAMPLES_PER_CLASS}")

X = np.array(X, dtype=np.float32)
y = np.array(y, dtype=np.int32)

np.savez_compressed("dataset.npz", X=X, Y=to_categorical(y, 8))
