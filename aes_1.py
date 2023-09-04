'''symmetric encryption AES
'''
import pyaes, pbkdf2,binascii, os, secrets

# Derive a 256-bit AES encryption key from the password
password = "s3cr3t*c0d3"
passwordSalt = os.urandom(16)
key = pbkdf2.PBKDF2(password, passwordSalt).read(32)
print('AES encryption key:',binascii.hexlify(key))
