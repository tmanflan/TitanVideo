#!/usr/bin/env python3
"""
Generate QR code for TitanVideo subscription payment
"""

import qrcode
from PIL import Image, ImageDraw, ImageFont
import os

def generate_subscription_qr():
    # Subscription payment URL
    payment_url = "https://titanvideo.ai/subscribe"

    # Generate QR code
    qr = qrcode.QRCode(
        version=1,
        error_correction=qrcode.constants.ERROR_CORRECT_H,
        box_size=10,
        border=4,
    )
    qr.add_data(payment_url)
    qr.make(fit=True)

    # Create QR code image
    qr_img = qr.make_image(fill_color="black", back_color="white")

    # Create a larger image with text
    width, height = qr_img.size
    new_height = height + 100
    combined_img = Image.new('RGB', (width, new_height), 'white')
    combined_img.paste(qr_img, (0, 0))

    # Add text
    draw = ImageDraw.Draw(combined_img)
    try:
        # Try to use a nice font
        font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 20)
    except:
        # Fallback to default font
        font = ImageFont.load_default()

    text = "Scan to Subscribe to TitanVideo"
    bbox = draw.textbbox((0, 0), text, font=font)
    text_width = bbox[2] - bbox[0]
    text_x = (width - text_width) // 2
    text_y = height + 20

    draw.text((text_x, text_y), text, fill="black", font=font)

    # Save the image
    combined_img.save("subscription_qr.png")
    print("✅ QR code generated: subscription_qr.png")
    print(f"📱 Payment URL: {payment_url}")

if __name__ == "__main__":
    generate_subscription_qr()
