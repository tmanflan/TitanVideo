#!/usr/bin/env python3
"""
TitanVideo Owner License Generator
Creates a permanent enterprise license for owner access
Usage: python create_owner_license.py
"""

import sys
import os
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from license_manager import LicenseManager
import base64
from datetime import datetime, timedelta
import json

def create_owner_license():
    """Create permanent enterprise license for owner"""
    print("🔑 Creating TitanVideo Owner License")
    print("=" * 40)

    # Initialize license manager
    lm = LicenseManager()

    # Create enterprise license valid for 10 years
    expiry_days = 3650  # 10 years
    expiry_date = datetime.now() + timedelta(days=expiry_days)

    license_data = {
        'package': 'enterprise',
        'machine_id': lm.machine_id,
        'expiry': expiry_date.isoformat(),
        'activated': datetime.now().isoformat(),
        'owner_access': True,  # Special flag for owner license
        'created_by': 'owner_script'
    }

    # Generate signature
    license_data['signature'] = lm._generate_signature(license_data)

    # Save license file
    os.makedirs(os.path.dirname(lm.license_file), exist_ok=True)
    with open(lm.license_file, 'w') as f:
        json.dump(license_data, f, indent=2)

    print("✅ Owner license created successfully!"    print(f"📁 License file: {lm.license_file}")
    print(f"🔐 Machine ID: {lm.machine_id}")
    print(f"📅 Expires: {expiry_date.strftime('%Y-%m-%d')}")
    print("🎯 Package: Enterprise (All features unlocked)"
    print("\n⚠️  This license is for owner use only")
    print("🚫 Do not distribute or share this license")

if __name__ == "__main__":
    # Simple security check - require confirmation
    print("This will create a permanent owner license for TitanVideo.")
    confirm = input("Are you the owner? (type 'yes' to confirm): ").strip().lower()

    if confirm == 'yes':
        create_owner_license()
    else:
        print("❌ Access denied - owner confirmation required")
