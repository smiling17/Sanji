# -*- coding: utf-8 -*-

"""
    네트워크 소켓 관련 상수 정의.
"""
IFF_UP = 0x1
IFF_BROADCAST = 0x2
IFF_DEBUG = 0x4
IFF_LOOPBACK = 0x8
IFF_POINTOPOINT = 0x10
IFF_NOTRAILERS = 0x20
IFF_RUNNING = 0x40
IFF_NOARP = 0x80
IFF_PROMISC = 0x100
IFF_ALLMULTI = 0x200
IFF_MASTER = 0x400
IFF_SLAVE = 0x800
IFF_MULTICAST = 0x1000
IFF_PORTSEL = 0x2000
IFF_AUTOMEDIA = 0x4000
IFF_DYNAMIC = 0x8000

IFNAMSIZ = 16
IFHWADDRLEN = 6

# Incomplete list of most IOCTLs used to control interfaces

# ['const', 'struct', 'rtentry', '*', '//', 'MORE']
SIOCADDRT = 0x0000890B

# ['const', 'struct', 'rtentry', '*', '//', 'MORE']
SIOCDELRT = 0x0000890C

# ['char', '[]']
SIOCGIFNAME = 0x00008910

# ['void']
SIOCSIFLINK = 0x00008911

# ['struct', 'ifconf', '*', '//', 'MORE', '//', 'I-O']
SIOCGIFCONF = 0x00008912

# ['struct', 'InterfaceReq', '*', '//', 'I-O']
SIOCGIFFLAGS = 0x00008913

# ['const', 'struct', 'InterfaceReq', '*']
SIOCSIFFLAGS = 0x00008914

# [ifr_ifindex]
SIOCGIFINDEX = 0x00008933

# ['struct', 'InterfaceReq', '*', '//', 'I-O']
SIOCGIFADDR = 0x00008915

# ['const', 'struct', 'InterfaceReq', '*']
SIOCSIFADDR = 0x00008916

# ['struct', 'InterfaceReq', '*', '//', 'I-O']
SIOCGIFDSTADDR = 0x00008917

# ['const', 'struct', 'InterfaceReq', '*']
SIOCSIFDSTADDR = 0x00008918

# ['struct', 'InterfaceReq', '*', '//', 'I-O']
SIOCGIFBRDADDR = 0x00008919

# ['const', 'struct', 'InterfaceReq', '*']
SIOCSIFBRDADDR = 0x0000891A

# ['struct', 'InterfaceReq', '*', '//', 'I-O']
SIOCGIFNETMASK = 0x0000891B

# ['const', 'struct', 'InterfaceReq', '*']
SIOCSIFNETMASK = 0x0000891C

# ['struct', 'InterfaceReq', '*', '//', 'I-O']
SIOCGIFMETRIC = 0x0000891D

# ['const', 'struct', 'InterfaceReq', '*']
SIOCSIFMETRIC = 0x0000891E

# ['struct', 'InterfaceReq', '*', '//', 'I-O']
SIOCGIFMEM = 0x0000891F

# ['const', 'struct', 'InterfaceReq', '*']
SIOCSIFMEM = 0x00008920

# ['struct', 'InterfaceReq', '*', '//', 'I-O']
SIOCGIFMTU = 0x00008921

# ['const', 'struct', 'InterfaceReq', '*']
SIOCSIFMTU = 0x00008922

# ['struct', 'InterfaceReq', '*', '//', 'I-O']
OLD_SIOCGIFHWADDR = 0x00008923

# ['const', 'struct', 'InterfaceReq', '*', '//', 'MORE']
SIOCSIFHWADDR = 0x00008924
# ['int', '*']
SIOCGIFENCAP = 0x00008925

# ['const', 'int', '*']
SIOCSIFENCAP = 0x00008926

# ['struct', 'InterfaceReq', '*', '//', 'I-O']
SIOCGIFHWADDR = 0x00008927

# ['void']
SIOCGIFSLAVE = 0x00008929

# ['void']
SIOCSIFSLAVE = 0x00008930

# ['const', 'struct', 'InterfaceReq', '*']
SIOCADDMULTI = 0x00008931

# ['const', 'struct', 'InterfaceReq', '*']
SIOCDELMULTI = 0x00008932

# ['void']
SIOCADDRTOLD = 0x00008940

# ['void']
SIOCDELRTOLD = 0x00008941

# ['ifr_ifqlen']
SIOCGIFTXQLEN = 0x00008942

# ['ifr_ifqlen']
SIOCSIFTXQLEN = 0x00008943

# ['const', 'struct', 'arpreq', '*']
SIOCDARP = 0x00008950

# ['struct', 'arpreq', '*', '//', 'I-O']
SIOCGARP = 0x00008951

# ['const', 'struct', 'arpreq', '*']
SIOCSARP = 0x00008952

# ['const', 'struct', 'arpreq', '*']
SIOCDRARP = 0x00008960

# ['struct', 'arpreq', '*', '//', 'I-O']
SIOCGRARP = 0x00008961

# ['const', 'struct', 'arpreq', '*']
SIOCSRARP = 0x00008962

# ['struct', 'InterfaceReq', '*', '//', 'I-O']
SIOCGIFMAP = 0x00008970

# ['const', 'struct', 'InterfaceReq', '*']
SIOCSIFMAP = 0x00008971

FLAGS = {
    IFF_UP: 'Interface is up',
    IFF_BROADCAST: 'Broadcast address valid',
    IFF_DEBUG: 'Turn on debugging',
    IFF_LOOPBACK: 'Is a loopback net',
    IFF_POINTOPOINT: 'Interface is point-to-point link',
    IFF_NOTRAILERS: 'Avoid use of trailers',
    IFF_RUNNING: 'Resources allocated',
    IFF_NOARP: 'No address resolution protocol',
    IFF_PROMISC: 'Receive all packets',
    IFF_ALLMULTI: 'Receive all multicast packets',
    IFF_MASTER: 'Master of a load balancer',
    IFF_SLAVE: 'Slave of a load balancer',
    IFF_MULTICAST: 'Supports multicast',
    IFF_PORTSEL: 'Can set media type',
    IFF_AUTOMEDIA: 'Auto media select active',
    IFF_DYNAMIC: 'Dialup device with changing addresses'
}
