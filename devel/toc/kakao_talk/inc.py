#!/usr/bin/python3

import os
import sys
import urllib.request
import tarfile
import zipfile
from shutil import copyfile
import subprocess
from threading import Timer

home_dir = os.path.expanduser('~')
download_path = home_dir + '/compat_app'

def timeout(appname, proc):
  print('Complete execute ' + appname)
  proc.terminate()

def check_application(appname, proc):
  t = Timer(20, timeout, [appname, proc])
  t.start()
  out, err = proc.communicate()
  if proc.returncode != -15:
    print('Error occured in ' + appname)
    print('Error Code : ', proc.returncode)
    t.cancel()
    return 100
  return 0

def change_permissions_recursive(path, mode):
  for root, dirs, files in os.walk(path, topdown=False):
    for dir in [os.path.join(root,d) for d in dirs]:
      os.chmod(dir, mode)
    for file in [os.path.join(root,f) for f in files]:
      os.chmod(file, mode)

def download_application():
  if not os.path.exists(download_path):
    os.makedirs(download_path)

  kakao_download_path = download_path + '/KakaoTalk_0802.zip'
  if not os.path.exists(kakao_download_path):
    print('Start download KakaoTalk_0802.zip')
    kakao_url = 'http://192.168.105.119:8989/ms/KakaoTalk_0802.zip'
    urllib.request.urlretrieve(kakao_url, kakao_download_path)
  print('Complete download KakaoTalk_0802.zip')

def extract_application():
  print('Start extract KakaoTalk')
  if not os.path.exists('/tos/KakaoTalk.tap'):
    #kakao_zip_file = zipfile.ZipFile(download_path + '/KakaoTalk_0802.zip')
    #kakao_zip_file.extractall('/zone/normal/rootfs/tos/users/AllUsers/AppList/KakaoTalk.tap')
    #kakao_zip_file.close()
    kakao_zip_file = zipfile.ZipFile(download_path + '/KakaoTalk_0802.zip')
    kakao_zip_file.extractall('/tos/KakaoTalk.tap')
    kakao_zip_file.close()
    change_permissions_recursive('/tos/KakaoTalk.tap', 0o777)
  print('Complete extract KakaoTalk')

def execute_application():
  environ_val = os.environ
  proc_kakao = subprocess.Popen(['/tos/KakaoTalk.tap/KakaoTalk.exe'])
  #proc_kakao = subprocess.Popen(['lxc-attach', '-n', 'normal', '--', '/tos/users/AllUsers/AppList/KakaoTalk.tap/KakaoTalk.exe'])
  ret = check_application('KakaoTalk', proc_kakao)
  if ret == 100:
    return 100

if __name__ == '__main__':
  download_application()
  extract_application()
  ret = execute_application()
  if ret == 100:
    sys.exit(100)
  sys.exit(0)
