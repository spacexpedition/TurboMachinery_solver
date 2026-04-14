import sqlite3
import datetime

# =====================================================================
# Database Setup for Paywall
# =====================================================================
conn = sqlite3.connect("turbomachinery_users.db", check_same_thread=False)
cursor = conn.cursor()

# Create tables
cursor.execute('''
    CREATE TABLE IF NOT EXISTS users (
        email TEXT PRIMARY KEY,
        has_active_pass BOOLEAN DEFAULT 0,
        free_uses INTEGER DEFAULT 0,
        last_renewal_month INTEGER
    )
''')
cursor.execute('''
    CREATE TABLE IF NOT EXISTS ips (
        ip_address TEXT PRIMARY KEY,
        free_uses INTEGER DEFAULT 0,
        last_renewal_month INTEGER
    )
''')
conn.commit()

class SubscriptionService:
    @staticmethod
    def check_and_increment_usage(email: str, ip_address: str) -> dict:
        """ Returns dict indicating if they are paywalled and uses left."""
        current_month = datetime.datetime.now().month
        
        # 1. Grab User Record
        cursor.execute("SELECT has_active_pass, free_uses, last_renewal_month FROM users WHERE email=?", (email,))
        user = cursor.fetchone()
        if not user:
            cursor.execute("INSERT INTO users (email, has_active_pass, free_uses, last_renewal_month) VALUES (?, 0, 0, ?)", (email, current_month))
            user = (0, 0, current_month)
        
        has_active_pass, u_uses, u_month = user
        
        if u_month != current_month:
            u_uses = 0
            cursor.execute("UPDATE users SET free_uses=0, last_renewal_month=? WHERE email=?", (current_month, email))
            
        # 2. Grab IP Record
        cursor.execute("SELECT free_uses, last_renewal_month FROM ips WHERE ip_address=?", (ip_address,))
        ip_rec = cursor.fetchone()
        if not ip_rec:
            cursor.execute("INSERT INTO ips (ip_address, free_uses, last_renewal_month) VALUES (?, 0, ?)", (ip_address, current_month))
            ip_rec = (0, current_month)
            
        i_uses, i_month = ip_rec
        
        if i_month != current_month:
            i_uses = 0
            cursor.execute("UPDATE ips SET free_uses=0, last_renewal_month=? WHERE ip_address=?", (current_month, ip_address))

        conn.commit()
        
        if has_active_pass:
            return {"paywalled": False, "free_uses_left": "Unlimited"}
            
        # Check bounds (limit 5)
        if u_uses >= 5 or i_uses >= 5:
            return {"paywalled": True, "free_uses_left": "0"}
            
        # Increment usage
        cursor.execute("UPDATE users SET free_uses = free_uses + 1 WHERE email=?", (email,))
        cursor.execute("UPDATE ips SET free_uses = free_uses + 1 WHERE ip_address=?", (ip_address,))
        conn.commit()
        
        uses_left = min(5 - (u_uses + 1), 5 - (i_uses + 1))
        return {"paywalled": False, "free_uses_left": str(uses_left)}

    @staticmethod
    def unlock_semester_pass(email: str):
        cursor.execute("UPDATE users SET has_active_pass=1 WHERE email=?", (email,))
        conn.commit()
