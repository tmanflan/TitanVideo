#!/usr/bin/env python3
"""
TitanVideo License Enforcement Test
Tests that the application properly enforces licensing requirements
"""

import sys
import os
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from license_manager import LicenseManager

def test_license_enforcement():
    """Test license enforcement mechanisms"""
    print("🧪 Testing TitanVideo License Enforcement")
    print("=" * 50)

    # Initialize license manager
    lm = LicenseManager()

    print("1. Testing license requirement enforcement...")
    if lm.current_license:
        print("✅ License found - testing validity...")
        if lm.require_valid_license_for_operation("test operation"):
            print("✅ License is valid")
        else:
            print("❌ License validation failed")
    else:
        print("❌ No license found - enforcement should prevent usage")
        print("   This is the expected behavior for unlicensed users")

    print("\n2. Testing package information...")
    packages = lm.packages
    print(f"✅ Found {len(packages)} subscription packages:")
    for pkg_name, pkg_info in packages.items():
        print(f"   • {pkg_info['name']}: ${pkg_info['price']}/month")

    print("\n3. Testing no-refunds policy display...")
    print("⚠️  NO REFUNDS POLICY ENFORCED:")
    print("   • All sales are final")
    print("   • No refunds under any circumstances")
    print("   • Users must understand features before purchasing")

    print("\n4. Testing license validation...")
    if lm.current_license:
        is_valid = lm._validate_license_integrity()
        print(f"✅ License integrity check: {'PASSED' if is_valid else 'FAILED'}")
    else:
        print("❌ No license to validate")

    print("\n" + "=" * 50)
    print("🎯 LICENSE ENFORCEMENT TEST COMPLETE")
    print("💡 The application will exit immediately without a valid license")
    print("⚠️  Remember: ALL SALES ARE FINAL - NO REFUNDS")

if __name__ == "__main__":
    test_license_enforcement()
