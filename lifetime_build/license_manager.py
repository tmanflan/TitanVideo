"""
TitanVideo Licensing System
Manages different subscription tiers and feature access
STRICT ENFORCEMENT: No usage without valid license, no refunds policy
"""

import json
import os
import hashlib
import platform
from datetime import datetime, timedelta
import requests
from typing import Dict, List, Optional
import sys

class LicenseManager:
    def __init__(self):
        self.license_file = os.path.join(os.path.expanduser("~"), ".titanvideo", "license.json")
        self.machine_id = self._get_machine_id()
        self.packages = self._load_packages()
        self.current_license = self._load_license()
        self._enforce_no_refunds_policy()

    def _enforce_no_refunds_policy(self):
        """Display and enforce no refunds policy"""
        print("=" * 60)
        print("⚠️  TITANVIDEO NO REFUNDS POLICY ⚠️")
        print("=" * 60)
        print("ALL SALES ARE FINAL. NO REFUNDS UNDER ANY CIRCUMSTANCES.")
        print("By using TitanVideo, you agree to our no-refunds policy.")
        print("Make sure you understand the features before purchasing.")
        print("=" * 60)

    def _get_machine_id(self) -> str:
        """Generate unique machine identifier"""
        system_info = platform.uname()
        machine_string = f"{system_info.system}-{system_info.node}-{system_info.machine}"
        return hashlib.sha256(machine_string.encode()).hexdigest()[:16]

    def _load_packages(self) -> Dict:
        """Load available packages and their features"""
        return {
            "basic": {
                "name": "Basic",
                "price": 9.99,
                "features": [
                    "basic_video_generation",
                    "standard_resolution",
                    "watermark",
                    "basic_audio_analysis",
                    "export_mp4",
                    "community_support"
                ],
                "limits": {
                    "videos_per_month": 5,
                    "max_duration": 30,  # seconds
                    "max_resolution": "720p"
                }
            },
            "pro": {
                "name": "Pro",
                "price": 19.99,
                "features": [
                    "advanced_video_generation",
                    "high_resolution",
                    "no_watermark",
                    "advanced_audio_analysis",
                    "export_multiple_formats",
                    "ai_lyrics_generation",
                    "priority_support",
                    "cloud_storage"
                ],
                "limits": {
                    "videos_per_month": 50,
                    "max_duration": 300,  # 5 minutes
                    "max_resolution": "1080p"
                }
            },
            "premium": {
                "name": "Premium",
                "price": 39.99,
                "features": [
                    "premium_video_generation",
                    "ultra_high_resolution",
                    "no_watermark",
                    "professional_audio_analysis",
                    "export_all_formats",
                    "ai_script_generation",
                    "custom_effects",
                    "batch_processing",
                    "api_access",
                    "premium_support",
                    "unlimited_cloud_storage"
                ],
                "limits": {
                    "videos_per_month": 500,
                    "max_duration": 1800,  # 30 minutes
                    "max_resolution": "4K"
                }
            },
            "enterprise": {
                "name": "Enterprise",
                "price": 99.99,
                "features": [
                    "enterprise_video_generation",
                    "unlimited_resolution",
                    "no_watermark",
                    "enterprise_audio_analysis",
                    "export_all_formats",
                    "ai_director_mode",
                    "custom_models",
                    "white_label",
                    "api_unlimited",
                    "dedicated_support",
                    "on_premise_deployment",
                    "custom_integration"
                ],
                "limits": {
                    "videos_per_month": -1,  # unlimited
                    "max_duration": -1,  # unlimited
                    "max_resolution": "8K"
                }
            }
        }

    def _load_license(self) -> Optional[Dict]:
        """Load license from file"""
        try:
            if os.path.exists(self.license_file):
                with open(self.license_file, 'r') as f:
                    license_data = json.load(f)
                    if self._validate_license(license_data):
                        return license_data
        except Exception as e:
            print(f"Error loading license: {e}")
        return None

    def enforce_license_or_exit(self) -> bool:
        """Strict license enforcement - exit app if no valid license"""
        if not self.current_license:
            self._show_license_required_message()
            print("❌ APPLICATION TERMINATED: Valid license required")
            print("💳 Purchase a subscription at: https://titanvideo.ai/pricing")
            print("⚠️  REMEMBER: ALL SALES ARE FINAL - NO REFUNDS")
            sys.exit(1)
            return False

        if self.is_expired():
            self._show_license_expired_message()
            print("❌ APPLICATION TERMINATED: License expired")
            print("💳 Renew your subscription at: https://titanvideo.ai/pricing")
            print("⚠️  REMEMBER: ALL SALES ARE FINAL - NO REFUNDS")
            sys.exit(1)
            return False

        # Additional validation
        if not self._validate_license_integrity():
            self._show_license_invalid_message()
            print("❌ APPLICATION TERMINATED: License validation failed")
            print("💳 Contact support at: support@titanvideo.ai")
            print("⚠️  REMEMBER: ALL SALES ARE FINAL - NO REFUNDS")
            sys.exit(1)
            return False

        return True

    def _show_license_required_message(self):
        """Display license required message"""
        print("\n" + "=" * 60)
        print("🚫 LICENSE REQUIRED - ACCESS DENIED 🚫")
        print("=" * 60)
        print("TitanVideo requires a valid subscription to operate.")
        print("This is a professional AI-powered application.")
        print()
        print("📋 Available Plans:")
        for pkg_name, pkg_info in self.packages.items():
            print(f"   • {pkg_info['name']}: ${pkg_info['price']}/month")
        print()
        print("💳 Purchase: https://titanvideo.ai/pricing")
        print("⚠️  IMPORTANT: ALL SALES ARE FINAL - NO REFUNDS")
        print("=" * 60)

    def _show_license_expired_message(self):
        """Display license expired message"""
        print("\n" + "=" * 60)
        print("⏰ LICENSE EXPIRED - ACCESS DENIED ⏰")
        print("=" * 60)
        print("Your TitanVideo subscription has expired.")
        print("Renew now to continue using the application.")
        print()
        print("💳 Renew: https://titanvideo.ai/pricing")
        print("⚠️  IMPORTANT: ALL SALES ARE FINAL - NO REFUNDS")
        print("=" * 60)

    def _show_license_invalid_message(self):
        """Display license invalid message"""
        print("\n" + "=" * 60)
        print("❌ INVALID LICENSE - ACCESS DENIED ❌")
        print("=" * 60)
        print("Your license could not be validated.")
        print("This may be due to tampering or system changes.")
        print()
        print("💬 Support: support@titanvideo.ai")
        print("💳 Repurchase: https://titanvideo.ai/pricing")
        print("⚠️  IMPORTANT: ALL SALES ARE FINAL - NO REFUNDS")
        print("=" * 60)

    def _validate_license_integrity(self) -> bool:
        """Additional license integrity checks"""
        if not self.current_license:
            return False

        # Check for required fields
        required_fields = ['package', 'machine_id', 'expiry', 'signature']
        for field in required_fields:
            if field not in self.current_license:
                return False

        # Verify signature
        expected_signature = self._generate_signature(self.current_license)
        if self.current_license.get('signature') != expected_signature:
            return False

        # Check if machine ID matches
        if self.current_license.get('machine_id') != self.machine_id:
            return False

        return True

    def _validate_license(self, license_data: Dict) -> bool:
        """Validate license integrity and expiration"""
        if license_data.get('machine_id') != self.machine_id:
            return False

        expiry = datetime.fromisoformat(license_data.get('expiry', '2000-01-01'))
        if datetime.now() > expiry:
            return False

        # Validate signature (simplified)
        expected_signature = self._generate_signature(license_data)
        if license_data.get('signature') != expected_signature:
            return False

        return True

    def _generate_signature(self, license_data: Dict) -> str:
        """Generate license signature"""
        data_string = f"{license_data['package']}{license_data['machine_id']}{license_data['expiry']}"
        return hashlib.sha256(data_string.encode()).hexdigest()

    def activate_license(self, license_key: str) -> bool:
        """Activate license with key"""
        try:
            # Decode license key (simplified - in real app, this would validate with server)
            import base64
            decoded = base64.b64decode(license_key).decode()
            package, expiry_days = decoded.split('|')

            if package not in self.packages:
                return False

            expiry_date = datetime.now() + timedelta(days=int(expiry_days))

            license_data = {
                'package': package,
                'machine_id': self.machine_id,
                'expiry': expiry_date.isoformat(),
                'activated': datetime.now().isoformat()
            }

            license_data['signature'] = self._generate_signature(license_data)

            # Save license
            os.makedirs(os.path.dirname(self.license_file), exist_ok=True)
            with open(self.license_file, 'w') as f:
                json.dump(license_data, f, indent=2)

            self.current_license = license_data
            return True

        except Exception as e:
            print(f"License activation failed: {e}")
            return False

    def has_feature(self, feature: str) -> bool:
        """Check if current license has specific feature"""
        if not self.current_license:
            return False

        package = self.current_license['package']
        return feature in self.packages[package]['features']

    def get_limits(self) -> Dict:
        """Get current package limits"""
        if not self.current_license:
            return self.packages['basic']['limits']  # Free tier limits

        package = self.current_license['package']
        return self.packages[package]['limits']

    def is_expired(self) -> bool:
        """Check if license is expired"""
        if not self.current_license:
            return True

        expiry = datetime.fromisoformat(self.current_license['expiry'])
        return datetime.now() > expiry

    def get_package_info(self, package_name: str = None) -> Dict:
        """Get package information"""
        if package_name:
            return self.packages.get(package_name, {})
        elif self.current_license:
            return self.packages.get(self.current_license['package'], {})
        else:
            return self.packages['basic']

    def get_remaining_usage(self) -> Dict:
        """Get remaining usage for current billing period"""
        limits = self.get_limits()
        # In a real app, this would track actual usage
        return {
            'videos_remaining': limits['videos_per_month'],
            'time_remaining': limits['max_duration']
        }

    def upgrade_package(self, new_package: str) -> bool:
        """Upgrade to a higher tier package"""
        if not self.current_license:
            return False

        current_package = self.current_license['package']
        packages_order = ['basic', 'pro', 'premium', 'enterprise']

        if packages_order.index(new_package) <= packages_order.index(current_package):
            return False

        # Update license
        self.current_license['package'] = new_package
        self.current_license['signature'] = self._generate_signature(self.current_license)

        # Save updated license
        with open(self.license_file, 'w') as f:
            json.dump(self.current_license, f, indent=2)

        return True

    def require_valid_license_for_operation(self, operation_name: str = "this operation") -> bool:
        """Check license validity before allowing any operation"""
        if not self.current_license:
            print(f"❌ OPERATION BLOCKED: {operation_name} requires a valid license")
            self._show_license_required_message()
            return False

        if self.is_expired():
            print(f"❌ OPERATION BLOCKED: {operation_name} - license expired")
            self._show_license_expired_message()
            return False

        if not self._validate_license_integrity():
            print(f"❌ OPERATION BLOCKED: {operation_name} - invalid license")
            self._show_license_invalid_message()
            return False

        return True
