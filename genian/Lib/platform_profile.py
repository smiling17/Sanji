# -*- coding: utf-8 -*-

import platform

"""
    profile 변수는 아래와 같이 저장됨.
    우분투.
    [
        ('64bit', 'ELF'), 
        ('Ubuntu', '16.04', 'xenial'), 
        ('glibc', '2.7'), 
        'Linux', 
        'ubuntu-B250M-DS3H', 
        '4.4.0-127-generic', 
        '#153-Ubuntu SMP Sat May 19 10:58:46 UTC 2018', 
        'x86_64', 
        'x86_64', 
        'Linux-4.4.0-127-generic-x86_64-with-Ubuntu-16.04-xenial', 
        'Linux-4.4.0-127-generic-x86_64-with-Ubuntu-16.04-xenial', 
        'Linux-4.4.0-127-generic-x86_64-with-glibc2.7', 
        '2.7.12', 
        ('2', '7', '12'), 
        'GCC 5.4.0 20160609', 
        ('default', 'Dec  4 2017 14:50:18')
    ]
    
    레드햇.
    [
        ('64bit', 'ELF'), 
        ('redhat', '7.3', 'Maipo'), 
        ('glibc', '2.2.5'), 
        'Linux', 
        'localhost.localdomain', 
        '3.10.0-514.el7.x86_64', 
        '#1 SMP Wed Oct 19 11:24:13 EDT 2016', 
        'x86_64', 
        'x86_64', 
        'Linux-3.10.0-514.el7.x86_64-x86_64-with-redhat-7.3-Maipo', 
        'Linux-3.10.0-514.el7.x86_64-x86_64-with-redhat-7.3-Maipo', 
        'Linux-3.10.0-514.el7.x86_64-x86_64-with-glibc2.2.5', 
        '2.7.5', 
        ('2', '7', '5'), 
        'GCC 4.8.5 20150623 (Red Hat 4.8.5-4)', 
        ('default', 'Aug  2 2016 04:20:16')
    ]
"""
profile = [
    platform.architecture(),
    platform.dist(),
    platform.libc_ver(),
    platform.system(),
    platform.node(),
    platform.release(),
    platform.version(),
    platform.machine(),
    platform.processor(),

    platform.platform(),
    platform.platform(aliased=True),
    platform.platform(terse=True),
    platform.python_version(),
    platform.python_version_tuple(),
    platform.python_compiler(),
    platform.python_build(),
]


"""
    리눅스 에이전트에서 사용하는 OSID 값 범위는 2000 ~ 2999.
    OSID 값 범위가 벗어 나면 다른 OS(MAC, ios)의 OSID와 중복 될 수 있음. 
"""
OSID_DEFAULT_VALUE_LINUX = "2401"
OSIDS = [
    ("2101", "centos 7.0"),
    ("2102", "centos 7.1"),
    ("2103", "centos 7.2"),
    ("2104", "centos 7.3"),
    ("2105", "centos 7.4"),
    ("2106", "centos 6.0"),
    ("2107", "centos 6.1"),
    ("2108", "centos 6.2"),
    ("2109", "centos 6.3"),
    ("2110", "centos 6.4"),
    ("2111", "centos 6.5"),
    ("2112", "centos 6.6"),
    ("2113", "centos 6.7"),
    ("2114", "centos 6.8"),
    ("2115", "centos 6.9"),
    ("2116", "centos 6.10"),
    ("2117", "centos 7.5"),
    ("2201", "debian 7.0"),
    ("2202", "debian 7.1"),
    ("2203", "debian 7.2"),
    ("2204", "debian 7.3"),
    ("2205", "debian 7.4"),
    ("2206", "debian 7.5"),
    ("2207", "debian 7.6"),
    ("2208", "debian 7.7"),
    ("2209", "debian 7.8"),
    ("2210", "debian 7.9"),
    ("2211", "debian 7.10"),
    ("2212", "debian 7.11"),
    ("2213", "debian 8.0"),
    ("2214", "debian 8.1"),
    ("2215", "debian 8.2"),
    ("2216", "debian 8.3"),
    ("2217", "debian 8.4"),
    ("2218", "debian 8.5"),
    ("2219", "debian 8.6"),
    ("2220", "debian 8.7"),
    ("2221", "debian 8.8"),
    ("2222", "debian 8.9"),
    ("2223", "debian 8.10"),
    ("2224", "debian 8.11"),
    ("2225", "debian 9.0"),
    ("2226", "debian 9.1"),
    ("2227", "debian 9.2"),
    ("2228", "debian 9.3"),
    ("2229", "debian 9.4"),
    ("2230", "debian 9.5"),
    ("2301", "Gooroom 1.0"),
    ("2401", "Linux"),
    ("2501", "SuSE 11.0"),
    ("2502", "SuSE 11.1"),
    ("2503", "SuSE 11.2"),
    ("2504", "SuSE 11.3"),
    ("2505", "SuSE 11.4"),
    ("2506", "SuSE 12.1"),
    ("2507", "SuSE 12.2"),
    ("2508", "SuSE 12.3"),
    ("2509", "SuSE 13.1"),
    ("2510", "SuSE 13.2"),
    ("2511", "SuSE 42.1"),
    ("2512", "SuSE 42.2"),
    ("2513", "SuSE 42.3"),
    ("2514", "SuSE 15.0"),
    ("2601", "redhat 7.1"),
    ("2602", "redhat 7.2"),
    ("2603", "redhat 7.3"),
    ("2604", "redhat 7.4"),
    ("2605", "redhat 6.1"),
    ("2606", "redhat 6.2"),
    ("2607", "redhat 6.3"),
    ("2608", "redhat 6.4"),
    ("2609", "redhat 6.5"),
    ("2610", "redhat 6.6"),
    ("2611", "redhat 6.7"),
    ("2612", "redhat 6.8"),
    ("2613", "redhat 6.9"),
    ("2614", "redhat 6.10"),
    ("2701", "Ubuntu 12.04"),
    ("2702", "Ubuntu 12.10"),
    ("2703", "Ubuntu 13.04"),
    ("2704", "Ubuntu 13.10"),
    ("2705", "Ubuntu 14.04"),
    ("2706", "Ubuntu 14.10"),
    ("2707", "Ubuntu 15.04"),
    ("2708", "Ubuntu 15.10"),
    ("2709", "Ubuntu 16.04"),
    ("2710", "Ubuntu 16.10"),
    ("2711", "Ubuntu 17.04"),
    ("2712", "Ubuntu 17.10"),
    ("2713", "Ubuntu 18.04"),
]


def get_system_osid():
    ret_osid = OSID_DEFAULT_VALUE_LINUX
    try:
        platform_info = platform.dist()
        if platform_info[0] in "centos":
            """ 'centos', '7.4.1708', 'Core' => centos 7.4 """
            dist = ''.join([platform_info[0], " ", platform_info[1].split('.')[0], '.', platform_info[1].split('.')[1]])
        else:
            """ 
                >>> platform.dist()
                ('Ubuntu', '16.04', 'xenial')
                ('debian', '9.4', '')
            """
            dist = ''.join([platform_info[0], " ", platform_info[1]])

        for osid, os_name in OSIDS:
            if os_name.upper() == dist.upper():
                ret_osid = osid
                break
    except Exception as e:
        print("Unknown osid. Exception MSG=" + str(e))

    return ret_osid


def get_os_name():
    """
        platform.dist()
        ('Ubuntu', '16.04', 'xenial')
    :return:
    """
    return platform.dist()[0]


def get_os_version():
    """
        platform.dist()
        ('Ubuntu', '16.04', 'xenial')
    :return:
    """
    return platform.dist()[1]
