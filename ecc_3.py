'''
equation of curve= a*x^2+y^2=1+dx^2y^2
moduo for curve is the pow
'''
import hashlib
p=pow(2,255)-19
a=-1
d=-121665*pow(121666,-1,p)
G = (15112221349535400772501151409588531511454012693041857206046113283949847762202,46316835694926478169428394003475163141307993866256225615783033603165251855960)
from typing import Tuple
def is_on_curve(P,p):
    x,y=P
    assert((a*x*x)+(y*y))%p==(1+d*x*x*y*y)%p
def add_points(P,Q):
    x1,y1=P
    x2,y2=Q
    x3 =(((x1*y2+y1*x2)%p)*pow(1+d*x1*x2*y1*y2,-1,p))%p
    y3 =(((y1*y2-a*x1*x2)%p)*pow(1-d*x1*x2*y1*y2,-1,p))%p
    is_on_curve((x3,y3),p)
    return x3,y3
def apply_double_and_add_method(G: Tuple[int, int], k: int):
    target_point = G     
    k_binary = bin(k)[2:] #0b1111111001     
    for i in range(1, len(k_binary)):
        current_bit = k_binary[i: i+1]         
        # doubling - always
        target_point = add_points(target_point, target_point)         
        if current_bit == "1":
            target_point = add_points(target_point, G)     
    is_on_curve(target_point, p)     
    return target_point





import random
 
# Alice's private key
ka = random.getrandbits(256)
 
# Alice's public key
Qa = apply_double_and_add_method(G = G, k = ka)
# Bob's random key
rb = random.getrandbits(256)
 
# Point U will be sent to Alice
U = apply_double_and_add_method(G = G, k = rb)
 
# Point T will keep secret and use for encryption purposes
T = apply_double_and_add_method(G = Qa, k = rb)







def derive_keys(T):   
    tx, ty = T
    # get x coordinate of point T as binary
    tx_binary = bin(tx)[2:]
    # get its first 192-bit value
    tx_binary_cropped = tx_binary[0:192]
    # restore 192-bit x coordinate
    tx_restored = str(int(tx_binary_cropped, 2))
    # use sha-256 to hash
    hashed_tx = bin(int(hashlib.sha256(str.encode(tx_restored)).hexdigest(), 16))[2:]
 
    assert len(hashed_tx) == 256
 
    # split the hash into 128-bit and 128-bit as k1 and k2
 
    k1 = int(hashed_tx[0:128], 2).to_bytes(16, byteorder='big')
    k2 = int(hashed_tx[128:], 2).to_bytes(16, byteorder='big')
     
    return k1, k2
 
k1, k2 = derive_keys(T)






msg = "attack tomorrow!"
 
# Bob uses k1 to encrypt a message m with aes-128 and obtain c
obj_bob = AES.new(k1)
c = base64.b64encode(obj_bob.encrypt(msg))
def find_mac(message, key):
    return hmac.new(key,message,hashlib.sha256).hexdigest()
 
r = find_mac(c, k2)







T_prime = apply_double_and_add_method(G = U, k = ka)
k1_prime, k2_prime = derive_keys(T_prime)
assert r == find_mac(c, k2_prime)
obj_alice = AES.new(k1_prime)
plaintext = obj_alice.decrypt(base64.b64decode(c))
